
#include <map>
#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <fstream>
#include <stdint.h>

#include "read.hpp"
#include "../../mem.hpp"

using namespace std;

static ofstream *fp;
static map<node_t, double> heat_values;
static map<node_t, list<node_t>> edge_values;

static void
read_edge(fact_arguments args)
{
	node_t src = read_node(args[0]);
	node_t dst = read_node(args[1]);
	double weight = read_float(args[2]);
   uint32_t srco = (uint32_t)src;
   uint32_t dsto = (uint32_t)dst;
   (void)weight;

   auto it(edge_values.find(src));
   if(it == edge_values.end()) {
      list<node_t> ls;
      ls.push_back(dst);
      edge_values[src] = ls;
   } else {
      it->second.push_back(dst);
   }
}

static void
read_heat(fact_arguments args)
{
   node_t src = read_node(args[0]);
   double h = read_float(args[1]);
   heat_values[src] = h;
}

int
main(int argc, char **argv)
{
	if(argc != 3) {
		cout << "usage: compile_graph <meld data file> <output file>" << endl;
		return EXIT_FAILURE;
	}

	char *file(argv[1]);

   fp = new ofstream(argv[2]);

	add_read_handler("edge", read_edge);
   add_read_handler("heat", read_heat);
	read_file(file);

   uint32_t nnodes(heat_values.size());
   cout << "Writing " << nnodes << " nodes\n";
   fp->write((char *)&nnodes, sizeof(uint32_t));
   for(auto p : heat_values) {
      node_t node(p.first);
      double heat(p.second);
      list<node_t> edges(edge_values[node]);
      uint32_t total(edges.size());

      fp->write((char *)&node, sizeof(uint32_t));
      fp->write((char *)&heat, sizeof(double));
      fp->write((char *)&total, sizeof(uint32_t));

      for(node_t dst : edges) {
         fp->write((char *)&dst, sizeof(uint32_t));
      }
   }
   fp->close();
   delete fp;

	return EXIT_SUCCESS;
}



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

using namespace std;

static ofstream *fp;
map<uint32_t, uint32_t> node_map;
static uint32_t node_count{0};

static void
read_edge(fact_arguments args)
{
	node_t src = read_node(args[0]);
	node_t dst = read_node(args[1]);
	int weight = read_int(args[2]);
   uint32_t srco = (uint32_t)src;
   uint32_t dsto = (uint32_t)dst;
   uint32_t weighto = (uint32_t)weight;
   auto it1(node_map.find(srco));
   if(it1 == node_map.end()) {
      node_map[srco] = node_count;
      srco = node_count++;
   } else
      srco = it1->second;
   auto it2(node_map.find(dsto));
   if(it2 == node_map.end()) {
      node_map[dsto] = node_count;
      dsto = node_count++;
   } else
      dsto = it2->second;
   (*fp) << srco << "\t" << dsto << "\t" << weighto << "\n";
}

int
main(int argc, char **argv)
{
	if(argc != 3) {
		cout << "usage: meld_to_txt <.meld file> <.txt file>" << endl;
		return EXIT_FAILURE;
	}

	char *file(argv[1]);

   fp = new ofstream(argv[2]);

	add_read_handler("edge", read_edge);
	read_file(file);

   fp->close();
   delete fp;

	return EXIT_SUCCESS;
}


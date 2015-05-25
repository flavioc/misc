
#include <iostream>
#include <map>
#include <list>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <fstream>
#include <stdint.h>
#include <math.h>

using namespace std;

class node {
   public:

      uint32_t id;
      double heat{0.0};
      size_t inbound{0};
      std::map<uint32_t, node*> neighbors;

      explicit node(const uint32_t _id): id(_id) {}
};

static map<uint32_t, node*> nodes;
static double delta;

static bool
update_heat(node *n)
{
   double i((double)(n->inbound + 1));
   double oldheat(n->heat);
   double sum{0.0};
   for(auto p : n->neighbors) {
      node *neighbor(p.second);
      sum += neighbor->heat;
   }
   double new_heat((oldheat + sum)/i);
   n->heat = new_heat;
   return (new_heat - oldheat >= 0.0 ? new_heat - oldheat : oldheat - new_heat) >= delta;
}

static void
process_nodes()
{
   uint64_t updates{0};
   bool cont{true};
   while(cont) {
      cont = false;
      for(auto p : nodes) {
         auto n = p.second;
         cont |= update_heat(n);
         updates++;
      }
   }
   cout << updates << " updates\n";
}

int
main(int argc, char **argv)
{
	if(argc != 3) {
		cout << "usage: ht <meld data file> <delta>" << endl;
		return EXIT_FAILURE;
	}

   delta = atof(argv[2]);
   cout << "Using delta " << delta << endl;
   cout << "Reading file " << argv[1] << endl;
   ifstream fp(argv[1], ifstream::in);

   uint32_t nnodes;
   fp.read((char *)&nnodes, sizeof(uint32_t));
   cout << "To read " << nnodes << endl;
   for(size_t x(0); x < nnodes; ++x) {
      uint32_t node, total, dst;
      double heat;
      fp.read((char *)&node, sizeof(uint32_t));
      fp.read((char *)&heat, sizeof(double));
      fp.read((char *)&total, sizeof(uint32_t));
      class node *n(new class node(node));
      n->heat = heat;
      for(size_t i(0); i < total; ++i) {
         fp.read((char *)&dst, sizeof(uint32_t));
         n->neighbors[dst] = nullptr;
         n->inbound++;
      }
      nodes[node] = n;
   }
   cout << "Read " << nodes.size() << " nodes\n";

   // fix nodes
   for(auto p : nodes) {
      node *n(p.second);
      for(auto& p : n->neighbors) {
         auto x(nodes.find(p.first));
         node *o(x->second);
         p.second = o;
      }
   }

   process_nodes();
}

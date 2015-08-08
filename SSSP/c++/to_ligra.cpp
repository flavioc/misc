
#include <map>
#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <fstream>
#include <stdint.h>
#include <map>
#include <unordered_map>

#include "read.hpp"
#include "../../mem.hpp"

using namespace std;

int
main(int argc, char **argv)
{
	if(argc != 3) {
		cout << "usage: to_ligra <meld data file> <ligra file>" << endl;
		return EXIT_FAILURE;
	}

   ifstream fp(argv[1], ifstream::in);
   ofstream out(argv[2], ifstream::out);
   typedef std::pair<uint32_t, uint32_t> edge_pair;
   typedef std::vector<edge_pair> edge_vector;
   unordered_map<uint32_t, edge_vector> m;
   uint32_t edge_count{0};

   out << "WeightedAdjacencyGraph\n";
   uint32_t max_seen{0};

   while(!fp.eof()) {
      uint32_t src, dst, weight;
      fp.read((char *)&src, sizeof(uint32_t));
      fp.read((char *)&dst, sizeof(uint32_t));
      fp.read((char *)&weight, sizeof(uint32_t));
      max_seen = std::max(max_seen, std::max(src, dst));
      auto it(m.find(src));
      if(it == m.end()) {
         edge_vector v;
         v.push_back(make_pair(dst, weight));
         m[src] = v;
      } else {
         edge_vector& v(it->second);
         v.push_back(make_pair(dst, weight));
      }
      edge_count++;
   }
   for(size_t i(0); i <= max_seen; ++i) {
      auto it(m.find(i));
      if(it == m.end())
         m[i] = edge_vector();
   }

   out << m.size() << "\n";
   out << edge_count << "\n";
   uint32_t offset{0};
   size_t count{0};
#define INTERVAL 50000
   for(auto p : m) {
      out << offset << "\n";
      offset += p.second.size();
      count++;
      if(count % INTERVAL == 0) {
         count = 0;
         out.flush();
      }
   }
   for(auto p : m) {
      for(auto edge : p.second)
         out << edge.first << "\n";
      count++;
      if(count % INTERVAL == 0) {
         count = 0;
         out.flush();
      }
   }
   for(auto p : m) {
      for(auto edge : p.second)
         out << edge.second << "\n";
      count++;
      if(count % INTERVAL == 0) {
         count = 0;
         out.flush();
      }
   }
	return EXIT_SUCCESS;
}


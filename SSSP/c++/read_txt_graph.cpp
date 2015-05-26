
#include <map>
#include <vector>
#include <list>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <time.h>
#include <fstream>
#include <stdint.h>
#include <functional>

#include "read.hpp"

using namespace std;

void
add_edge(ofstream& fp, const uint32_t srco, const uint32_t dsto, const uint32_t weighto)
{
   fp.write((char *)&srco, sizeof(uint32_t));
   fp.write((char *)&dsto, sizeof(uint32_t));
   fp.write((char *)&weighto, sizeof(uint32_t));
}

int
main(int argc, char **argv)
{
	if(argc != 3) {
		cout << "usage: read_txt_graph <meld data file> <output file>" << endl;
		return EXIT_FAILURE;
	}

   ifstream rp(argv[1]);
   ofstream fp(argv[2]);

   string line;
	while(!rp.eof()) {
		getline(rp, line);
      auto vec(split(line, "\t "));
      if(vec.size() == 3) {
         uint32_t src(atoi(vec[0].c_str()));
         uint32_t dst(atoi(vec[1].c_str()));
         uint32_t weight(atoi(vec[2].c_str()));
         cout << src << " " << dst << " -> " << weight << endl;
         add_edge(fp, src, dst, weight);
      }
   }

   rp.close();

	return EXIT_SUCCESS;
}


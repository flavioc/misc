
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

static void
read_edge(fact_arguments args)
{
	node_t src = read_node(args[0]);
	node_t dst = read_node(args[1]);
	int weight = read_int(args[2]);
   uint32_t srco = (uint32_t)src;
   uint32_t dsto = (uint32_t)dst;
   uint32_t weighto = (uint32_t)weight;

   fp->write((char *)&srco, sizeof(uint32_t));
   fp->write((char *)&dsto, sizeof(uint32_t));
   fp->write((char *)&weighto, sizeof(uint32_t));
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
	read_file(file);

   fp->close();
   delete fp;

	return EXIT_SUCCESS;
}


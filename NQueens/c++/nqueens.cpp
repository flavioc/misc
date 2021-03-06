
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <stdint.h>

#include "../../mem.hpp"

using std::cout;
using std::cerr;
using std::endl;

typedef struct {
   int x, y;
} coord;

static std::vector<std::vector<int> > *solutions;

static bool
can_place(const std::vector<int> cols, int row, int new_col, const size_t size)
{
   // try to find queen in the same column
   for(size_t i(0); i < cols.size(); ++i) {
      if(new_col == cols[i])
         return false;
   }

   // right diagonal
   for(int row1(row - 1), col1(new_col + 1), j(cols.size()-1); row1 >= 0 && col1 < size; --row1, ++col1, --j) {
      assert(j >= 0);
      if(cols[j] == col1)
         return false;
   }

   // left diagonal
   for(int row1(row - 1), col1(new_col - 1), j(cols.size()-1); row1 >= 0 && col1 >= 0; --row1, --col1, --j) {
      assert(j >= 0);
      if(cols[j] == col1)
         return false;
   }

   return true;
}

static size_t
do_queens(std::vector<int>& cols, int row, size_t size)
{
   if(row == size) {
      readMemory();
      std::vector<int> v;
      for(size_t i(0); i < cols.size(); ++i) {
         v.push_back(i);
         v.push_back(cols[i]);
      }
      solutions->push_back(v);
#if 0
      cout << "Solution: ";
      for(size_t i(0); i < size; ++i) {
         cout << cols[i] << " ";
      }
      cout << endl;
#endif
      return 1;
   }

   size_t total = 0;
   for(size_t col(0); col < size; ++col) {
      if(can_place(cols, row, col, size)) {
         std::vector<int> new_state(cols);
         new_state.push_back(col);
         total += do_queens(new_state, row + 1, size);
      }
   }
   return total;
}

static size_t
compute_queens(size_t size)
{
   std::vector<int> columns_used;

   return do_queens(columns_used, 0, size);
}

int
main(int argc, char **argv)
{
   initMemory();
   if(argc != 2) {
      cerr << "usage: nqueens <size>\n";
      return EXIT_FAILURE;
   }

   solutions = new std::vector< std::vector<int> >();
   const size_t size(atoi(argv[1]));

#ifndef MEASURE_MEM
   cout << "Using size " << size << endl;
#endif

   compute_queens(size);
#ifndef MEASURE_MEM
   cout << "Found " << solutions->size() << " solutions" << endl;
#endif
   printMemory();

   return EXIT_SUCCESS;
}

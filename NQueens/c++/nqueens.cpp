
#include <vector>
#include <iostream>
#include <cstdlib>
#include <assert.h>

using std::cout;
using std::cerr;
using std::endl;

typedef struct {
   int x, y;
} coord;

class vector {
   private:

      size_t _cap;
      size_t _size;
      int *elems;

   public:

      void add(const int x) {
         if(_size == _cap) {
            int *oldelems(elems);

            _cap++;
            elems = (int*)malloc(_cap * sizeof(int));
            memcpy(elems, oldelems, sizeof(int)*_size);

            free(oldelems);
         }

         elems[_size++] = x;
      }

      size_t size() const { return _size; }

      int operator[](const size_t i) const {
         return elems[i];
      }

      int& operator[](const size_t i) {
         int& x(elems[i]);
         return x;
      }

      vector() {
         _cap = 1;
         _size = 0;
         elems = (int*)malloc(sizeof(int)*_cap);
      }

      vector(const vector& copy) {
         _cap = copy._cap;
         _size = copy._size;
         elems = (int*)malloc(sizeof(int) * _cap);
         memcpy(elems, copy.elems, sizeof(int)*_cap);
      }

      ~vector() {
         free(elems);
      }
};

static std::vector< vector > solutions;

static bool
can_place(const vector cols, int row, int new_col, const size_t size)
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
do_queens(vector cols, int row, size_t size)
{
   if(row == size) {
      vector v;
      for(size_t i(0); i < cols.size(); ++i) {
         v.add(i);
         v.add(cols[i]);
      }
      solutions.push_back(v);
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
         vector new_state;
         for(size_t i(0); i < cols.size(); ++i)
            new_state.add(cols[i]);
         new_state.add(col);
         total += do_queens(new_state, row + 1, size);
      }
   }
   return total;
}

static size_t
compute_queens(size_t size)
{
   vector columns_used;

   return do_queens(columns_used, 0, size);
}

int
main(int argc, char **argv)
{
   if(argc != 2) {
      cerr << "usage: nqueens <size>\n";
      return EXIT_FAILURE;
   }

   const size_t size(atoi(argv[1]));

   cout << "Using size " << size << endl;

   compute_queens(size);
   cout << "Found " << solutions.size() << " solutions" << endl;

   return EXIT_SUCCESS;
}

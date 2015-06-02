
#include <vector>
#include <list>
#include <iostream>

#include "../../mem.hpp"

using namespace std;

static int total(0);
static bool DEBUG(false);
static list< list<int>> *valid;

typedef struct {
   int score;
   int play;
} ret_value;

static int
otherplayer(const int player)
{
   if(player == 1) return 2;
   return 1;
}

static bool
allfilled(const vector<int> board, int player, int *points)
{
   int count(0);
   for(int x : board) {
      if(x == 0)
         return false;
      if(x == player)
         count++;
   }
   *points = count;
   return true;
}

static bool
checkwin(const vector<int> board, int player)
{
   for(auto& v : *valid) {
      bool found(true);
      for(int i : v) {
         if(board[i] != player) {
            found = false;
            break;
         }
      }
      if(found)
         return true;
   }
   return false;
}

static ret_value
minimax(const vector<int> board, int player, int rootplayer, int tab = 0)
{
#ifdef MEASURE_MEM
   if(player == rootplayer && tab % 2 == 0)
      readMemory();
#endif
   if(DEBUG) {
      for(size_t i(0); i < tab; ++i)
         cout << "\t";
      for(int x : board) {
         cout << x << " ";
      }
      cout << endl;
   }
   ++total;
   if(player == rootplayer) {
      if(checkwin(board, player)) {
         ret_value x;
         x.score = 200;
         x.play = 0;
         return x;
      }
      else if(checkwin(board, otherplayer(player))) {
         ret_value x;
         x.score = -100;
         x.play = 0;
         return x;
      } else {
         int points;
         if(allfilled(board, rootplayer, &points)) {
            ret_value x;
            x.score = points;
            x.play = 0;
            return x;
         }
         int bestPlay = -1;
         int bestScore = -999999;
         for(size_t i(0); i < board.size(); ++i) {
            if(board[i] != 0)
               continue;
            vector<int> new_board(board);
            new_board[i] = player;
            ret_value ret(minimax(new_board, otherplayer(player), rootplayer, tab + 1));
            if(DEBUG) {
               for(size_t i(0); i < tab + 1; ++i)
                  cout << "\t";
               cout << ret.score << endl;
            }
            if(bestPlay == -1) {
               bestPlay = i;
               bestScore = ret.score;
            } else {
               if(bestScore < ret.score) {
                  bestScore = ret.score;
                  bestPlay = i;
               }
            }
         }
         ret_value ret;
         ret.play = bestPlay;
         ret.score = bestScore;
         return ret;
      }
   } else {
      if(checkwin(board, player)) {
         ret_value x;
         x.score = -100;
         x.play = 0;
         return x;
      }
      else if(checkwin(board, otherplayer(player))) {
         ret_value x;
         x.score = 200;
         x.play = 0;
         return x;
      } else {
         int points;
         if(allfilled(board, rootplayer, &points)) {
            ret_value x;
            x.score = points;
            x.play = 0;
            return x;
         }
         int bestPlay = -1;
         int bestScore = 999999;
         for(size_t i(0); i < board.size(); ++i) {
            if(board[i] != 0)
               continue;
            vector<int> new_board(board);
            new_board[i] = player;
            ret_value ret(minimax(new_board, otherplayer(player), rootplayer, tab + 1));
            if(DEBUG) {
               for(size_t i(0); i < tab + 1; ++i)
                  cout << "\t";
               cout << ret.score << endl;
            }
            if(bestPlay == -1) {
               bestPlay = i;
               bestScore = ret.score;
            } else {
               if(bestScore > ret.score) {
                  bestScore = ret.score;
                  bestPlay = i;
               }
            }
         }
         ret_value ret;
         ret.play = bestPlay;
         ret.score = bestScore;
         return ret;
      }
   }
}

int
main(int argc, char **argv)
{
   initMemory();
   valid = new std::list< std::list<int> >({{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 4, 8}, {2, 4, 6}});
   vector<int> board = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   //vector<int> board = {0, 1, 0, 0, 0, 0, 0, 2, 1};
//   vector<int> board = {2, 1, 2, 0, 1, 0, 0, 0, 2};
   ret_value r(minimax(board, 1, 1));
#ifndef MEASURE_MEM
   cout << r.score << " " << r.play << endl;
   cout << "total " << total << endl;
#endif
   printMemory();
   return 0;
}

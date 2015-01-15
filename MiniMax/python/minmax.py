#!/usr/bin/python

DEBUG = False

def checkwin(board, player):
   for state in [[0, 3, 6], [1, 4, 7], [2, 5, 8], [0, 1, 2], [3, 4, 5], [6, 7, 8], [0, 4, 8], [2, 4, 6]]:
      found = True
      for x in state:
         if board[x] != player:
            found = False
            break
      if found: return True

def otherplayer(player):
   if player == 1: return 2
   else: return 1

def allfilled(board, rootplayer):
   count = 0
   for x in board:
      if x == 0:
         return (False, 0)
      elif x == rootplayer:
         count = count + 1
   return (True, count)

total = 0
def minimax(board, player, rootplayer, tab):
   if DEBUG:
      print "\t" * tab, board
   global total
   total = total + 1
   if player == rootplayer:
      # maximize
      if checkwin(board, player):
         return (200, 0)
      elif checkwin(board, otherplayer(player)):
         return (-100, 0)
      else:
         (filled, points) = allfilled(board, rootplayer)
         if filled: return (points, 0)
         bestPlay = -1
         bestScore = -999999
         copy = board[:]
         for i, val in enumerate(copy):
            if board[i] == 0:
               board[i] = player
               (score, p) = minimax(board, otherplayer(player), rootplayer, tab + 1)
               if DEBUG: print "\t" * (tab + 1), score
               board[i] = 0
               if bestPlay == -1:
                  bestPlay = i
                  bestScore = score
               else:
                  if bestScore < score:
                     bestScore = score
                     bestPlay = i
         return (bestScore, bestPlay)   
   else:
      if checkwin(board, player):
         return (-100, 0)
      elif checkwin(board, otherplayer(player)):
         return (200, 0)
      else:
         (filled, points) = allfilled(board, rootplayer)
         if filled: return (points, 0)
         bestPlay = -1
         bestScore = 999999
         copy = board[:]
         for i, val in enumerate(copy):
            if board[i] == 0:
               board[i] = player
               (score, p) = minimax(board, otherplayer(player), rootplayer, tab + 1)
               if DEBUG: print "\t" * (tab + 1), score
               board[i] = 0
               if bestPlay == -1:
                  bestPlay = i
                  bestScore = score
               else:
                  if bestScore > score:
                     bestScore = score
                     bestPlay = i
         return (bestScore, bestPlay)

#play([0, 1, 0, 0, 0, 0, 0, 2, 1], NextPlayer 1, LastPlay 0, RootPlayer 1).

#init_board = [0, 1, 0, 0, 0, 0, 0, 2, 1]
init_board = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
#init_board = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
print minimax(init_board, 1, 1, 0)
print total

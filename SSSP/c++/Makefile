

CXX = g++
CC = g++
OS = $(shell uname -s)

INCLUDE_DIRS = -I.
LIBRARY_DIRS =

PROFILING = #-pg
OPTIMIZATIONS = -O3
ARCH = -march=x86-64
DEBUG = -DNDEBUG #-g
WARNINGS = -Wall -Wextra #-Werror

CFLAGS = $(ARCH) $(PROFILING) $(OPTIMIZATIONS) $(WARNINGS) $(DEBUG) $(INCLUDE_DIRS) -std=c++11
LIBRARIES = -lm

CXXFLAGS = $(CFLAGS)
LDFLAGS = $(PROFILING) $(LIBRARY_DIRS) $(LIBRARIES)
COMPILE = $(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS)

OBJS = read.o \
		 dijkstra.o	\
		 compile_graph.o \
		 read_txt_graph.o \
		 to_ligra.o \
		 meld_to_txt.o

all: compile_graph dijkstra read_txt_graph to_ligra meld_to_txt

dijkstra: read.o dijkstra.o
	$(CXX) $(CFLAGS) read.o dijkstra.o -o dijkstra

compile_graph: read.o compile_graph.o
	$(CXX) $(CFLAGS) read.o compile_graph.o -o compile_graph

read_txt_graph: read.o read_txt_graph.o
	$(CXX) $(CFLAGS) read.o read_txt_graph.o -o read_txt_graph

meld_to_txt: read.o meld_to_txt.o
	$(CXX) $(CFLAGS) read.o meld_to_txt.o -o meld_to_txt

to_ligra: read.o to_ligra.o
	$(CXX) $(CFLAGS) read.o to_ligra.o -o to_ligra

clean:
	rm -f *.o dijkstra compile_graph read_txt_graph to_ligra

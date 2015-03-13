
CXXFLAGS = -std=c++11 -O3
CC = g++

all: compile_heat ht

ht: ht.o

ht.o: ht.cpp Makefile

compile_heat: compile_heat.o read.o

read.o: read.cpp read.hpp

compile_heat.o: compile_heat.cpp

clean:
	rm -f compile_heat ht *.o

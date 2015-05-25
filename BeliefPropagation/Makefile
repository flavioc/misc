CC = g++
CXXFLAGS = -O3 $(INCLUDES)
CFLAGS = $(CXXFLAGS)

all: main

main: main.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

clean:
	rm -f main main.o

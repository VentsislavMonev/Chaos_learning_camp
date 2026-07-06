CXX      := g++
CXXFLAGS := -std=c++17 -O3 -DNDEBUG -march=native -pthread
TARGETS  := main

.PHONY: all clean

all: $(TARGETS)

main: benchmark.cpp AVL.hpp Circular_dequeue.cpp Circular_dequeue.hpp Tiered_vector.cpp Tiered_vector.hpp
	$(CXX) $(CXXFLAGS) -o $@ benchmark.cpp Tiered_vector.cpp Circular_dequeue.cpp

clean:
	rm -f $(TARGETS)
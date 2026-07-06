CXX      := g++
CXXFLAGS := -std=c++17 -O3 -DNDEBUG -march=native -pthread
TARGETS  := main

.PHONY: all clean

all: $(TARGETS)

main: benchmark.cpp AVL.hpp Circular_dequeue.cpp Circular_dequeue.hpp Tiered_vector.cpp Tiered_vector.hpp
	$(CXX) $(CXXFLAGS) -o $@ benchmark.cpp Tiered_vector.cpp Circular_dequeue.cpp

avl: test_avl.cpp AVL.hpp 
	$(CXX) $(CXXFLAGS) -o $@ test_avl.cpp

tiered_vector: test_tiered.cpp Circular_dequeue.cpp Circular_dequeue.hpp Tiered_vector.cpp Tiered_vector.hpp
	$(CXX) $(CXXFLAGS) -o $@ test_tiered.cpp Circular_dequeue.cpp Tiered_vector.cpp

circular_dequeue: test_circular.cpp Circular_dequeue.cpp Circular_dequeue.hpp 
	$(CXX) $(CXXFLAGS) -o $@ test_circular.cpp Circular_dequeue.cpp

clean:
	rm -f $(TARGETS)
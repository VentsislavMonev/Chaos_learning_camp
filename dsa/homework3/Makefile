CXX      := g++
CXXFLAGS := -std=c++20 -O3 -march=native -Wall -Wextra -I.
LDFLAGS  :=

LIBS := -lpthread

.PHONY: all clean

all: heat_shield gen_input

heat_shield: heat_shield.cpp thread_pool.hpp parralel_for_and_reduce.h
	$(CXX) $(CXXFLAGS) -o $@ heat_shield.cpp $(LIBS)

gen_input: gen_input.cpp
	$(CXX) $(CXXFLAGS) -o $@ gen_input.cpp $(LIBS)

clean:
	rm -f heat_shield gen_input checkpoint_*.bin test_input.bin

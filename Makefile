CXX=g++
CFLAGS=-Wall -g -O2 -std=gnu++17 -lpthread

.PHONY: tests clean

SRC_FILES=$(wildcard *.cpp)
HEAD_FILES=$(wildcard *.hpp)

# Compile all
all: tsp

# Run test with valgrind to detect memory leaks
test: tsp
	valgrind ./tsp < test1.in

# Run
run: tsp
	./tsp < test1.in

# Object files
tsp: $(SRC_FILES) $(HEAD_FILES)
	$(CXX) -o $@ $< $(CFLAGS)

# Cleaning
clean:
	rm tsp

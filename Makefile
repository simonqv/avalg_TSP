CXX=g++
CFLAGS=-Wall -g -O2 -std=gnu++17

.PHONY: test clean val

SRC_FILES=$(wildcard *.cpp)
HEAD_FILES=$(wildcard *.hpp)

# Compile all
all: tsp

# Run test with valgrind to detect memory leaks
val: tsp
	valgrind ./tsp < test1.in

test: tsp
	time ./tsp < test2.in 1> /dev/null

# Run
run: tsp
	./tsp < test1.in

# Object files
tsp: $(SRC_FILES) $(HEAD_FILES)
	$(CXX) -o $@ $< $(CFLAGS)

# Cleaning
clean:
	rm tsp

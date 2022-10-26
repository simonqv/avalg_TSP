CXX=g++
CFLAGS=-Wall -g -O2 -std=gnu++17 -static -lrt -Wl,--whole-archive -lpthread -Wl,--no-whole-archive

.PHONY: tests clean

SRC_FILES=$(wildcard *.cpp)
HEAD_FILES=$(wildcard *.hpp)

# Compile all
all: tsp

# Run test with valgrind to detect memory leaks
test: tsp
	valgrind ./tsp

# Run
run: tsp
	./tsp

# Object files
tsp: $(SRC_FILES) $(HEAD_FILES)
	$(CXX) -o $@ $< $(CFLAGS)

# Cleaning
clean:
	rm tsp

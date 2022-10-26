CXX=g++
CFLAGS=-Wall -g -std=c++17
LFLAGS=-lm -lpthread

.PHONY: tests clean

# Compile all
all: tsp

# Run tests
tests: tsp
	valgrind ./tsp

# Run
run: tsp
	./tsp

# Executables
tsp: tsp.o
	$(CXX) -o $@ $^ $(CFLAGS) $(LFLAGS)

# Object files
%.o: %.cpp %.h
	$(CXX) -c -o $@ $< $(CFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(CFLAGS)

# Cleaning
clean:
	find . -name '*.o' | xargs -I % rm %
	find . -name 'tsp' | xargs -I % rm %

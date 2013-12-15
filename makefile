#Michael Coyle makefile
COMPILER = gcc
CCFLAGS = -pedantic -Wall -std=c99
all: cache-sim.c cache-sim.h
	$(COMPILER) $(CCFLAGS) -o -lm cache-sim.c
debug: cache-sim.c cache-sim.h
	$(COMPILER) $(CCFLAGS) -ggdb -o -lm cache-sim.c
	
clean:
	rm -f cache-sim
	rm -f *.o

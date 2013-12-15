#Michael Coyle makefile
COMPILER = gcc
CCFLAGS = -pedantic -Wall -std=c99
all: cache-sim.c cache-sim.h
	$(COMPILER) $(CCFLAGS) -o cache-sim.c -lm
debug: cache-sim.c cache-sim.h
	$(COMPILER) $(CCFLAGS) -ggdb -o -lm cache-sim.c -lm
	
clean:
	rm -f cache-sim
	rm -f *.o

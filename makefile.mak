#Michael Coyle makefile
COMPILER = gcc
CCFLAGS = -pedantic -Wall -std=c99 -lm
all: cache-sim

cache-sim: cache-sim.o
	$(COMPILER) $(CCFLAGS) -o cache-sim cache-sim.o
cache-sim.o: cache-sim.c
	$(COMPILER) $(CCFLAGS) -c cache-sim.c
debug: cache-sim.c cache-sim.h
	$(COMPILER) $(CCFLAGS) -ggdb -o cache-sim.c
	
clean:
	rm -f cache-sim
	rm -f *.o

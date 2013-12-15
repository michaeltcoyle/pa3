#Michael Coyle makefile
COMPILER = gcc
CFLAGS = -pedantic -Wall -std=c99
all: cache-sim

cache-sim: cache-sim.o
	$(COMPILER) $(CFLAGS) -o -lm cache-sim cache-sim.o
cache-sim.o: cache-sim.c
	$(COMPILER) $(CFLAGS) -c -lm cache-sim.c
debug: cache-sim.c cache-sim.h
	$(COMPILER) $(CFLAGS) -ggdb -o -lm cache-sim.c
	
clean:
	rm -f cache-sim
	rm -f *.o

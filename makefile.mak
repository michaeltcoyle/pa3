#Michael Coyle makefile
COMPILER = gcc
CFLAGS = -pedantic -Wall -std=c99 -lm
all: cache-sim

cache-sim: cache-sim.o
	$(COMPILER) $(CFLAGS) -o cache-sim cache-sim.o
cache-sim.o: cache-sim.c
	$(COMPILER) $(CFLAGS) -c cache-sim.c
debug: cache-sim.c cache-sim.h
	$(COMPILER) $(CFLAGS) -ggdb -o cache-sim.c
	
clean:
	rm -f cache-sim
	rm -f *.o

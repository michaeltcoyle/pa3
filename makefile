#Michael Coyle makefile
COMPILER = gcc
CCFLAGS = -pedantic -Wall -std=c99
all: cache-sim

cache-sim: cache-sim.o 
	$(COMPILER) $(CCFLAGS) -lm -o cache-sim cache-sim.o
cache-sim.o: cache-sim.c
	$(COMPILER) $(CCFLAGS) -c -lm cache-sim cache-sim.c
debug: cache-sim cache-sim.o
	$(COMPILER) $(CCFLAGS) -ggdb -lm -o cache-sim cache-sim.c
	
clean:
	rm -f cache-sim
	rm -f *.o

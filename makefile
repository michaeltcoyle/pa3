#Michael Coyle makefile
COMPILER = gcc
CCFLAGS = -pedantic -Wall -std=c99
all: cache-sim
	$(COMPILER) $(CCFLAGS) -lm cache-sim cache-sim.o
cache-sim: cache-sim.o 
	$(COMPILER) $(CCFLAGS) -o -lm cache-sim cache-sim.o
cache-sim.o: cache-sim.c
	$(COMPILER) $(CCFLAGS) -c -lm cache-sim cache-sim.c
debug: cache-sim cache-sim.o
	$(COMPILER) $(CCFLAGS) -ggdb -lm cache-sim cache-sim.c
	
clean:
	rm -f cache-sim
	rm -f *.o

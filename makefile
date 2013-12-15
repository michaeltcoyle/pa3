#Michael Coyle makefile
COMPILER = gcc
CCFLAGS = -pedantic -Wall -std=c99 -o
all: cache-sim.o
	$(COMPILER) $(CCFLAGS) -lm cache-sim cache-sim.o
debug: cache-sim.o
	$(COMPILER) $(CCFLAGS) -ggdb -lm cache-sim cache-sim.c
	
clean:
	rm -f cache-sim
	rm -f *.o

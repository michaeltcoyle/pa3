#Michael Coyle makefile
COMPILER = gcc
CCFLAGS = -pedantic -Wall -std=c99
all: cache-sim.c
	$(COMPILER) cache-sim.c $(CCFLAGS) -lm -o cache-sim
cache-sim: cache-sim.o 
	$(COMPILER) cache-sim $(CCFLAGS) -lm -o cache-sim
cache-sim.o: cache-sim.c
	$(COMPILER) cache-sim.o $(CCFLAGS) -c -lm -o cache-sim cache-sim.c
debug: cache-sim.c
	$(COMPILER) cache-sim.c $(CCFLAGS) -DDEBUG -lm -o cache-sim
	
clean:
	rm -f cache-sim
	rm -f *.o

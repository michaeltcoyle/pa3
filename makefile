#Michael Coyle makefile
COMPILER = gcc
CCFLAGS = -pedantic -Wall -std=c99
all: cache-sim.c
	$(COMPILER) cache-sim.c $(CCFLAGS) -lm -o cache-sim
cache-sim: cache-sim.o 
	$(COMPILER) cache-sim $(CCFLAGS) -lm -o cache-sim
cache-sim.o: cache-sim.c
	$(COMPILER) cache-sim.o $(CCFLAGS) -c -lm -o cache-sim cache-sim.c
debug: cache-sim cache-sim.o cache-sim.c
	$(COMPILER) cache-sim.o $(CCFLAGS) -ggdb -lm -o cache-sim cache-sim.c

run:
	./cache-sim -l1size 1024 -l1assoc direct -l2size 2048 -l2assoc assoc:3 -l3size 4096 -l3assoc assoc:2 16 fifo lstrace.txt
	
clean:
	rm -f cache-sim
	rm -f *.o

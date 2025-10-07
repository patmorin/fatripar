CPP=g++


# Use these for fastest binary
DEBUGS=
OPTS=-O3

# Use these for slow debuggable binary
DEBUGS=-g -DDEBUG
OPTS=-Og

CPPFLAGS=-c -Wall $(OPTS) $(DEBUGS)

OBJECTS=triangulation.o bfs.o cotree.o lca.o rmq.o tripod.o bipod.o
HEADERS=$(OBJECTS:.o=.h)

all: tripod_demo bipod_demo

tripod_demo: $(OBJECTS) tripod_demo.o
	$(CPP) $(OBJECTS) $(DEBUGS) tripod_demo.o -o $@

bipod_demo: $(OBJECTS) bipod_demo.o
	$(CPP) $(OBJECTS) $(DEBUGS) bipod_demo.o -o $@

bfs.o : bfs.cc bfs.h triangulation.h
	$(CPP) -C $(CPPFLAGS) $< -o $@

main.o : tripod_demo.cc $(HEADERS)
	$(CPP) -C $(CPPFLAGS) $< -o $@

%.o: %.cc %.h
	$(CPP) -C $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o main

reset: clean main

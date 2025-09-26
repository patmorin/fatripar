

CPP=g++
DEBUGS=-g -DDEBUG
#DEBUGS=
#OPTS=-O4
CPPFLAGS=-c -Wall $(OPTS) $(DEBUGS)

OBJECTS=triangulation.o bfs.o cotree.o lca.o rmq.o tripod.o
HEADERS=$(OBJECTS:.o=.h)

all: main

main: $(OBJECTS) main.o
	$(CPP) $(OBJECTS) $(DEBUGS) main.o -o $@

bfs.o : bfs.cc bfs.h triangulation.h
	$(CPP) -C $(CPPFLAGS) $< -o $@

main.o : main.cc $(HEADERS)
	$(CPP) -C $(CPPFLAGS) $< -o $@

%.o: %.cc %.h
	$(CPP) -C $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o main

reset: clean main

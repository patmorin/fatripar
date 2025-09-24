

CPP=g++
DEBUGS=-g -DDEBUG
CPPFLAGS=-c -O4 -Wall $(DEBUGS)

OBJECTS=triangulation.o bfs.o lca.o rmq.o
HEADERS=$(OBJECTS:.o=.h)
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



CPP=g++
CPPFLAGS=-c -g -Wall -DDEBUG

OBJECTS=triangulation.o bfs.o

main: $(OBJECTS) main.o
	$(CPP) $(OBJECTS) main.o -o $@

main.o : main.cc
	$(CPP) -C $(CPPFLAGS) $< -o $@

%.o: %.cc %.h
	$(CPP) -C $(CPPFLAGS) $< -o $@

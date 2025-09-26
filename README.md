# fatripa: Fast Tripod Partition
Linear-time tripod partitions of triangulations

C++ code for the linear-time algorithm for partitioning a triangulation into tripods described in this paper:

https://arxiv.org/abs/2202.08870

In product structure language, this code produces an embedding of an input triangulation G into the strong product H x P x K_3, where H is a graph of treewidth at most 3, P is a path, and K_3 is a 3-cycle.

## Building

In a typical Linux development environment you should be able to compile the code by typing `make`.  This will create a demonstration program called `main`.

## Running

Sample input files and a program for making large inputs are provided in the `inputs/` subdirectory. The `main` program takes the name of one of these files as an argument and creates a tripod decomposition. The code is pretty fast. Partitioning a million-vertex triangulation takes around 1.5 seconds on a very modest desktop.

## Input file format

We treat an *n*-vertex triangulation as having vertices labelled 0,...,*n*-1 and having 2*n*-4 faces labelled 0,..,2*n*-5.

The input file format consists of a zero'th line that contains the number *n* of vertices in the triangulation followed 2*n*-4 lines. For *i*>= 1, the *i*th line of the file contains six integers that describe the (*i*-1)th face.  The first three of these are the vertices of the face, in counterclockwise order. The last three of these are the neighbouring faces, also in counterclockwise order and aligned so that (for example) the zero'th neighbouring face of face *i* shares the zero'th and first vertices of face *i*.  If you're confused, here's what a five vertex triangulation looks like:

    5
    4 0 3 4 1 2
    0 1 3 5 2 0
    3 1 4 1 3 0
    1 2 4 5 4 2
    4 2 0 3 5 0
    1 0 2 1 4 3

## Using the code

If you want to use the code in your own project, the class you're probably interested in is the `tripod_partition` class.  It produces a partition X, where H = G/X is the graph that appears in the strong product above.
# Input files and generator

This directory contains sample triangulations that in the format parseable by the `triangulation` class. It also contains a program, `makeinput.cpp` that can generate these files. To compile this program and make a 1 million vertex input file, you can run the following commands:

    g++ -O makeinput.cpp -o makeinput
    ./makeinput 1000000 > t1m.txt

This program uses (and this directory contains a copy of) the `delaunator-cpp` code by Volodymyr Bilonenko. The original repository that contains this code is at https://github.com/delfrrr/delaunator-cpp/
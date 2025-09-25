#include<fstream>
#include<vector>
#include<chrono>

#include "triangulation.h"
#include "bfs.h"
#include "lca.h"
#include "tripod.h"

int main(int argc, char **argv) {
  // triangulation g(3);

  std::cout << "argc = " << argc << std::endl;
  char const *filename = "triangulation.txt";
  if (argc == 2) {
    filename = argv[1];
  }
  std::cout << "Reading input file...";
  std::cout.flush();
  auto start = std::chrono::high_resolution_clock::now();
  std::ifstream inFile(filename);
  triangulation g(inFile);
  auto stop = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
  std::cout << "done (" << 1e-9*elapsed << "s)" << std::endl;

  std::cout << g.nVertices() << " vertices and " << g.nFaces()
            << " faces." << std::endl;

  std::cout << "Verifying...";
  std::cout.flush();
  start = std::chrono::high_resolution_clock::now();
  g.verify();
  stop = std::chrono::high_resolution_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
  std::cout << "done (" << 1e-9*elapsed << "s)" << std::endl;

  std::cout << "Starting tripod partition.............................." << std::endl;
  start = std::chrono::high_resolution_clock::now();
  tripod_partition tp(g);
  stop = std::chrono::high_resolution_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
  std::cout << "...............................................done ("
    << 1e-9*elapsed << "s)" << std::endl;
}

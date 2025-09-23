#include<fstream>
#include<vector>
#include<chrono>

#include "triangulation.h"
#include "bfs.h"
#include "lca.h"

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

  // std::cout << g << std::endl;

  std::cout << "Computing BFS forest...";
  std::cout.flush();
  start = std::chrono::high_resolution_clock::now();
  ;
  std::vector<half_edge> t(g.nVertices(), half_edge (-2, -2));
  std::vector<int[3]> bt(g.nFaces());
  int f0 = 0;
  half_edge e0(f0, 0);
  bfs_tree_cotree(g, e0, t, bt);
  stop = std::chrono::high_resolution_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
  std::cout << "done (" << 1e-9*elapsed << "s)" << std::endl;

  std::cout << "Computing LCA structure...";
  std::cout.flush();
  start = std::chrono::high_resolution_clock::now();
  ;
  stop = std::chrono::high_resolution_clock::now();
  lca_structure lca(bt, f0);
  elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
  std::cout << "done (" << 1e-9*elapsed << "s)" << std::endl;




}

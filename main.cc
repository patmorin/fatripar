#include<fstream>
#include<vector>
#include<chrono>

#include "triangulation.h"
#include "bfs.h"

int main(int argc, char **argv) {
  // triangulation g(3);


  std::cout << "Reading input file...";
  std::cout.flush();
  auto start = std::chrono::high_resolution_clock::now();
  std::ifstream inFile("triangulation.txt");
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
  triangulation::half_edge e0(g, -1, -1);
  std::vector<triangulation::half_edge> t(g.nVertices(), e0);
  triple_bfs(g, 0, t);
  stop = std::chrono::high_resolution_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
  std::cout << "done (" << 1e-9*elapsed << "s)" << std::endl;
}

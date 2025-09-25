#include<chrono>
#include<iostream>
#include<vector>
#include<assert.h>
#include "tripod.h"
#include "bfs.h"
#include "lca.h"

tripod_partition::tripod_partition(const triangulation& _g) 
    : g(_g),
      tripods(),
      vertex_colours(g.nVertices(), -1) {
  std::cout << "Computing BFS forest...";
  std::cout.flush();
  auto start = std::chrono::high_resolution_clock::now();
  std::vector<half_edge> t(g.nVertices(), half_edge (-2, -2));
  std::vector<int[3]> bt(g.nFaces());
  int f0 = 0;
  half_edge e0(f0, 0);
  bfs_tree_cotree(g, e0, t, bt);
  auto stop = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
  std::cout << "done (" << 1e-9*elapsed << "s)" << std::endl;

  std::cout << "Computing LCA structure...";
  std::cout.flush();
  start = std::chrono::high_resolution_clock::now();
  lca_structure lca(bt, f0);
  stop = std::chrono::high_resolution_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
  std::cout << "done (" << 1e-9*elapsed << "s)" << std::endl;

  tripod t0;
  t0.tau = f0;
  for (auto i = 0; i < 3; i++) {
    t0.legs[i].push_back(g[f0].vertices[i]);
  }
  monochromatic_instance(e0.reverse(g));
}

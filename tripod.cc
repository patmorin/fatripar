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
      vertex_colours(g.nVertices(), -1),
      face_colours(g.nFaces(), -1),
      t(g.nVertices(), half_edge (-2, -2)),
      bt(g.nFaces()) {
  std::cout << "Computing BFS forest...";
  std::cout.flush();
  auto start = std::chrono::high_resolution_clock::now();
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
    vertex_colours[g[f0].vertices[i]] = 0;
    t0.legs[i].push_back(g[f0].vertices[i]);
  }
  tripods.push_back(t0);
  face_colours[f0] = 0;
  monochromatic_instance(e0.reverse(g));
}

void tripod_partition::monochromatic_instance(const half_edge e0 ) {
  assert(face_colours[e0.left_face(g)] == -1);

  int c = tripods.size();
  tripod y;
  y.tau = e0.left_face(g);
  face_colours[y.tau] = c;
  int u = e0.opposite_vertex(g);
  int lu = (e0.i+2) % 3;
  assert(g[e0.left_face(g)].vertices[lu] == u);
  while (vertex_colours[u] == -1) {
    y.legs[lu].push_back(u);
    vertex_colours[u] = c;
    u = t[u].target(g);
  }
  // tripod is complete, add it to the list
  tripods.push_back(y);

  if (y.legs[lu].empty()) {
    // tripod is degenerate, check for non-empty subproblems
    // check left subproblem
    half_edge e1 = e0.next_edge_vertex(g);
    if (!tree_edge(e1) && face_colours[e1.left_face(g)] == -1) {
      monochromatic_instance(e1);
    }
    // check right subproblem
    half_edge e2 = e0.next_edge_face(g).reverse(g);
    if (!tree_edge(e2) && face_colours[e2.left_face(g)] == -1) {
      monochromatic_instance(e2);
    }
  } else {
    // check left subproblem
    half_edge e1 = e0.next_edge_vertex(g);
    half_edge e2 = t[y.legs[lu].back()];
    if (e1 != e2.reverse(g)) {
      bichromatic_instance(e1, e2);
    }
    // check right subproblem
    e1 = e0.next_edge_face(g).reverse(g);
    e2 = t[y.legs[lu].back()].reverse(g);
    if (e1 != e2.reverse(g)) {
      bichromatic_instance(e1, e2);
    }
  }
}

void tripod_partition::bichromatic_instance(const half_edge e0, const half_edge e1) {
}  


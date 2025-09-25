#include<chrono>
#include<iostream>
#include<vector>
#include<assert.h>
#include "tripod.h"
#include "bfs.h"
#include "lca.h"

tripod_partition_algorithm::tripod_partition_algorithm(const triangulation& _g, std::vector<tripod> _tripods) 
    : g(_g),
      tripods(_tripods),
      vertex_colours(g.nVertices(), -1),
      face_colours(g.nFaces(), -1),
      t(g.nVertices(), half_edge (-2, -2)),
      bt(g.nFaces()),
      lca(NULL), // dummy, will be initialized later
      subproblems() {
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
  lca = new lca_structure(bt, f0);
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

  std::vector<half_edge> s0(1, e0.reverse(g));
  subproblems.push_back(std::vector<half_edge>(1, e0.reverse(g)));
  while (!subproblems.empty()) {
    auto s = subproblems.back();
    subproblems.pop_back();
    if (s.size() == 1) {
      std::cout << "mono" << std::endl;
      monochromatic_instance(s[0]);
    } else if (s.size() == 2) {
      std::cout << "bi" << std::endl;
      bichromatic_instance(s[0], s[1]);
    } else if (s.size() == 3) {
      std::cout << "tri" << std::endl;
      trichromatic_instance(s[0], s[1], s[2]);
    } else {
      assert(false);
    }
  }
  delete lca;
}

void tripod_partition_algorithm::monochromatic_instance(const half_edge e0 ) {
  assert(face_colours[e0.left_face(g)] == -1);

  tripod y;
  y.tau = e0.left_face(g);
  // y has two empty legs e.source() and e0.target, and a third leg from e0.opposite()
  int c = tripods.size();
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
      subproblems.push_back(std::vector<half_edge>(1, e1));
    }
    // check right subproblem
    half_edge e2 = e0.next_edge_face(g).reverse(g);
    if (!tree_edge(e2) && face_colours[e2.left_face(g)] == -1) {
      subproblems.push_back(std::vector<half_edge>(1,e2));
    }
  } else {
    // tripod is non-degenerate, subproblems are bichromatic
    // check left subproblem
    half_edge e1 = e0.next_edge_vertex(g);
    half_edge e2 = t[y.legs[lu].back()];
    if (e1 != e2.reverse(g)) {
      subproblems.push_back(std::vector<half_edge> {e1, e2});
    }
    // check right subproblem
    e1 = e0.next_edge_face(g).reverse(g);
    e2 = t[y.legs[lu].back()].reverse(g);
    if (e1 != e2.reverse(g)) {
      subproblems.push_back(std::vector<half_edge> {e1, e2});
    }
  }
}

void tripod_partition_algorithm::bichromatic_instance(const half_edge e0, const half_edge e1) {
  assert(face_colours[e0.left_face(g)] == -1);
  assert(face_colours[e1.left_face(g)] == -1);
  assert(e0.reverse(g) != e1);
  assert(vertex_colours[e0.source(g)] == vertex_colours[e1.target(g)]);
  assert(vertex_colours[e1.source(g)] == vertex_colours[e0.target(g)]);

  tripod y;
  y.tau = e0.left_face(g);
  // y has two empty legs e.source() and e0.target, and a third leg from e0.opposite()
  int c = tripods.size();
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
    // tripod is degenerate, non-empty subproblems are monochromatic
    // check left subproblem
    half_edge e2 = e0.next_edge_vertex(g);
    if (!tree_edge(e2) && face_colours[e2.left_face(g)] == -1) {
      // non-empty, is it monochromatic or bichromatic?
      if (vertex_colours[e2.target(g)] == vertex_colours[e2.source(g)]) {
        subproblems.push_back(std::vector<half_edge> {e2}); // monochromatic
      } else {
        subproblems.push_back(std::vector<half_edge> {e1, e2}); // bichromatic
      }
    }
    // check right subproblem
    e2 = e0.next_edge_face(g).reverse(g);
    if (!tree_edge(e2) && face_colours[e2.left_face(g)] == -1) {
      // non-empty, is it monochromatic or bichromatic?
      if (vertex_colours[e2.target(g)] == vertex_colours[e2.source(g)]) {
        subproblems.push_back(std::vector<half_edge> {e2}); // monochromatic
      } else {
        subproblems.push_back(std::vector<half_edge> {e1, e2}); // bichromatic
      }
    }
  } else {
    // tripod is non-degenerate, non-empty subproblems are bi and trichromatic
    // check left subproblem
    half_edge e2 = e0.next_edge_vertex(g);
    half_edge e3 = t[y.legs[lu].back()];
    if (e2 != e3.reverse(g)) {
      if (vertex_colours[e3.target(g)] == vertex_colours[e2.source(g)]) {
        subproblems.push_back(std::vector<half_edge> {e2, e3}); // bichromatic
      } else {
        subproblems.push_back(std::vector<half_edge> {e1, e2, e3}); // trichromatic
      }
    }
    // check right subproblem
    e2 = e0.next_edge_face(g).reverse(g);
    e3 = t[y.legs[lu].back()].reverse(g);
    if (e2 != e3.reverse(g)) {
      if (vertex_colours[e3.source(g)] == vertex_colours[e2.target(g)]) {
        subproblems.push_back(std::vector<half_edge> {e2, e3}); // bichromatic
      } else {
        subproblems.push_back(std::vector<half_edge> {e1, e3, e2}); // trichromatic
      }
    }
  }
}

int tripod_partition_algorithm::find_sperner_triangle(const half_edge e0, const half_edge e1, const half_edge e2)   {
  // find the triangle with vertices of all three colours
  auto f0 = e0.left_face(g);
  auto f1 = e1.left_face(g);
  auto p = lca->query(f0, f1);
  auto f2 = e2.left_face(g);
  auto r = lca->query(p, f2);

  if (bt[r][1] == -1 || bt[r][2] == -1) {
    // r has only one child, so it is one of the input faces
    // and the other two faces are in the same subtree
    if (r == f0) {
      return lca->query(f1, f2); 
    } else if (r == f1) {
      return lca->query(f0, f2); 
    } else /* r == f2 */ {
      return lca->query(f0, f1); 
    }
  }
  // r has two children, two of the input faces are in one subtree
  if (p != r) {
    // f0 and f1 are in the same subtree of r
    return p;
  } 
  // f0 and f1 are in different subtrees of r, we need to know which one contains f2
  auto q = lca->query(f0, f2);
  if (q != r) {
    // f0 and f2 are in the same subtree of r
    return q;
  }
  return lca->query(f1, f2);
}

void tripod_partition_algorithm::trichromatic_instance(const half_edge e0, const half_edge e1, const half_edge e2) {
  assert(face_colours[e0.left_face(g)] == -1);
  assert(face_colours[e1.left_face(g)] == -1);
  assert(face_colours[e2.left_face(g)] == -1);
  assert(vertex_colours[e0.source(g)] == vertex_colours[e2.target(g)]);
  assert(vertex_colours[e1.source(g)] == vertex_colours[e0.target(g)]);
  assert(vertex_colours[e2.source(g)] == vertex_colours[e1.target(g)]);


  auto f0 = e0.left_face(g);
  auto f1 = e1.left_face(g);
  auto f2 = e2.left_face(g);

  tripod y;
  if (f0 == f1) {
    y.tau = f0;
  } else if (f1 == f2) {
    y.tau = f1;
  } else if (f2 == f0) {
    y.tau = f2;
  } else {
    y.tau = find_sperner_triangle(e0, e1, e2);
  }
  int c = tripods.size();
  face_colours[y.tau] = c;

  for (auto lu = 0; lu < 3; lu++) {
    int u = g[y.tau].vertices[lu];
    while (vertex_colours[u] == -1) {
      y.legs[lu].push_back(u);
      vertex_colours[u] = c;
      u = t[u].target(g);
    }
  }
  for (auto i = 0; i < 3; i++) {
    assert(vertex_colours[foot(y, i)] != vertex_colours[foot(y, (i+1)%3)]);
  }
  // tripod is complete, add it to the list
  tripods.push_back(y);

  int r = 0;
  while (r < 3 && vertex_colours[e0.source(g)] != vertex_colours[foot(y, r)]) {
    r++;
  }
  assert(r < 3);

  // subproblem with e0, leg r and leg (r+1)%3
  half_edge a0;
  if (y.legs[r].empty()) {
    a0 = half_edge(y.tau, r).reverse(g);
  } else {
    a0 = t[y.legs[r].back()];
  }
  half_edge a1;
  if (y.legs[(r+1)%3].empty()) {
    a1 = half_edge(y.tau, r).reverse(g);
  } else {
    a1 = t[y.legs[(r+1)%3].back()].reverse(g);
  }
  if (a0 == e0) {
    // empty subproblem, do nothing
  } else if (a0 == a1) {
    subproblems.push_back(std::vector<half_edge> {e0, a0}); // bichromatic
  } else {
    subproblems.push_back(std::vector<half_edge> {a0, e0, a1}); // trichromatic
  }
}

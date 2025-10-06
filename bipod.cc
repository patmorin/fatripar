#include<chrono>
#include<iostream>
#include<vector>
#include<assert.h>
#include "bfs.h"
#include "cotree.h"
#include "lca.h"
#include "bipod.h"

bipod_partition_algorithm::bipod_partition_algorithm(const triangulation& _g, const std::vector<half_edge>& _t,
                                                       int f0, std::vector<bipod>& _bipods)
    : g(_g),
      bipods(_bipods),
      vertex_colours(g.nVertices(), -1),
      edge_status(2*g.nFaces(), false),
      t(_t),
      bt(g.nFaces()),
      lca(NULL), // dummy, will be initialized later
      subproblems() {
  cotree(g, t, f0, bt);
  lca = new lca_structure(bt, f0);
  partition(f0);
  delete lca;

#ifdef DEBUG
  for (int v = 0; v < (int)g.nVertices(); v++) {
    assert(vertex_colours[v] >= 0);
  }
  for (int f = 0; f < (int)g.nFaces(); f++) {
    assert(face_colours[f] >= 0);
  }
#endif // DEBUG
}

void bipod_partition_algorithm::partition(int f0) {
  bipod b0;
  b0.tau = half_edge(f0, 1);
  b0.legs[0].push_back(g[b0.tau.f].vertices[1]);
  b0.legs[0].push_back(g[b0.tau.f].vertices[0]);
  b0.legs[1].push_back(g[b0.tau.f].vertices[2]);
  for (auto i = 0; i < 3; i++) {
    vertex_colours[g[b0.tau.f].vertices[i]] = 0;
    make_solid(half_edge(b0.tau.f, i));
  }
  bipods.push_back(b0);
  // face_colours[f0] = 0;


  subproblems.push_back(std::vector<half_edge> {half_edge(f0, 0).reverse(g)});
  while (!subproblems.empty()) {
    auto s = subproblems.back();
    subproblems.pop_back();
    if (s.size() == 1) {
      monochromatic_instance(s[0]);
    } else if (s.size() == 2) {
      bichromatic_instance(s[0], s[1]);
    } else if (s.size() == 3) {
      trichromatic_instance(s);
    } else {
      assert(false);
    }
  }
}

void bipod_partition_algorithm::subcritical_instance(const vector<half_edge> e ) {
  auto d = e.size();
  for (auto i = 0; i < d; i++) {
    assert(vertex_colours[e[i].source(g)] == vertex_colours[e[i].target(g)]);
    assert(solid_edge(e[i]));
    if (d > 1) {
      // check that portal edges are bichromatic
      assert(vertex_colours[e[i].target(g)]
            != vertex_colours[e[(i+1)%d)].source(g)]);
    }
  }

  bipod y;
  y.tau = e0.next_edge_vertex(g);
  grow_leg(y, bipods.size(), 1);

  if (y.legs[1].empty()) {
    // check if left subproblem is non-empty
    if (!solid_edge(y.tau)) {
      make_solid(y.tau);
      auto j = 0;
      while (j < e.size() && vertex_colours[foot(y)]
                             != vertex_colours[e[j].source(g)]) {
        j++;
      }
      assert(j < e.size());
      std::vector<half_edge>

      for (auto j = 0; j < e.size() && ; j++) {

      }
      subproblems.push_back(std::vector<half_edge>(1, y.tau));
    }
    // check if right subproblem is non-empty
    auto e2 = e0.next_edge_face(g).reverse(g);
    if (!solid_edge(e2)) {
      make_solid(e2);
      subproblems.push_back(std::vector<half_edge> {e2});
    }
  } else {
    // y is non-empty, subproblems are bichromatic
    make_solid(y.tau);
    bipods.push_back(y);
    // check left subproblem
    auto e2 = t[y.legs[1].back()];
    if (y.tau != e2.reverse(g)) {
      subproblems.push_back(std::vector<half_edge> {y.tau, e2});
    }
    // check right subproblem
    auto e1 = e0.next_edge_face(g).reverse(g);
    make_solid(e1);
    e2 = t[y.legs[1].back()].reverse(g);
    if (e1 != e2.reverse(g)) {
      subproblems.push_back(std::vector<half_edge> {e1, e2});
    }
  }
}

void bipod_partition_algorithm::monochromatic_instance(const half_edge e0 ) {
  // assert(face_colours[e0.left_face(g)] == -1);

  bipod y;
  y.tau = e0.next_edge_vertex(g);
  grow_leg(y, bipods.size(), 1);

  if (y.legs[1].empty()) {
    // y is empty, subproblems are monochromatic
    // check if left subproblem is non-empty
    if (!solid_edge(y.tau)) {
      make_solid(y.tau);
      subproblems.push_back(std::vector<half_edge>(1, y.tau));
    }
    // check if right subproblem is non-empty
    auto e2 = e0.next_edge_face(g).reverse(g);
    if (!solid_edge(e2)) {
      make_solid(e2);
      subproblems.push_back(std::vector<half_edge> {e2});
    }
  } else {
    // y is non-empty, subproblems are bichromatic
    make_solid(y.tau);
    bipods.push_back(y);
    // check left subproblem
    auto e2 = t[y.legs[1].back()];
    if (y.tau != e2.reverse(g)) {
      subproblems.push_back(std::vector<half_edge> {y.tau, e2});
    }
    // check right subproblem
    auto e1 = e0.next_edge_face(g).reverse(g);
    make_solid(e1);
    e2 = t[y.legs[1].back()].reverse(g);
    if (e1 != e2.reverse(g)) {
      subproblems.push_back(std::vector<half_edge> {e1, e2});
    }
  }
}

void bipod_partition_algorithm::bichromatic_instance(const half_edge e0, const half_edge e1) {
  // assert(face_colours[e0.left_face(g)] == -1);
  // assert(face_colours[e1.left_face(g)] == -1);
  assert(e0.reverse(g) != e1);
  assert(vertex_colours[e0.source(g)] == vertex_colours[e1.target(g)]);
  assert(vertex_colours[e1.source(g)] == vertex_colours[e0.target(g)]);

  // y has two empty legs e.source() and e0.target, and a
  // third leg from e0.opposite()
  bipod y;
  y.tau = e0.next_edge_vertex(g);
  grow_leg(y, bipods.size(), 1);

  if (y.legs[1].empty()) {
    // y is empty, subproblems are mono and bichromatic

    // face_colours[y.tau] = g.nFaces();
    // check if left subproblem is non-empty
    // half_edge e2 = e0.next_edge_vertex(g);
    if (!solid_edge(y.tau)) {
      // non-empty, is it monochromatic or bichromatic?
      if (vertex_colours[y.tau.target(g)] == vertex_colours[y.tau.source(g)]) {
        // monochromatic subproblem
        subproblems.push_back(std::vector<half_edge> {y.tau});
      } else {
        // bichromatic subproblem
        subproblems.push_back(std::vector<half_edge> {y.tau, e1});
      }
    }
    // check if right subproblem is non-empty
    auto e2 = e0.next_edge_face(g).reverse(g);
    if (!solid_edge(e2)) {
      // non-empty, is it monochromatic or bichromatic?
      if (vertex_colours[e2.target(g)] == vertex_colours[e2.source(g)]) {
        // monochromatic subproblem
        subproblems.push_back(std::vector<half_edge> {e2});
      } else {
        // bichromatic subproblem
        subproblems.push_back(std::vector<half_edge> {e1, e2});
      }
    }
  } else {
    // y is non-empty, subproblems are bi and trichromatic
    // face_colours[y.tau] = bipods.size();
    bipods.push_back(y);
    make_solid(y.tau);
    // check if left subproblem is non-empty
    // half_edge e2 = e0.next_edge_vertex(g);
    half_edge e3 = t[y.legs[1].back()];
    if (y.tau != e3.reverse(g)) {
      if (vertex_colours[e3.target(g)] == vertex_colours[y.tau.source(g)]) {
        // bichromatic subproblem
        subproblems.push_back(std::vector<half_edge> {y.tau, e3});
      } else {
        // trichromatic subproblem
        subproblems.push_back(std::vector<half_edge> {e1, y.tau, e3});
      }
    }
    // check if right subproblem is non-empty
    auto e2 = e0.next_edge_face(g).reverse(g);
    e3 = t[y.legs[1].back()].reverse(g);
    if (e2 != e3.reverse(g)) {
      if (vertex_colours[e3.source(g)] == vertex_colours[e2.target(g)]) {
        // bichromatic subproblem
        subproblems.push_back(std::vector<half_edge> {e2, e3});
      } else {
        // trichromatic subproblem
        subproblems.push_back(std::vector<half_edge> {e1, e3, e2});
      }
    }
  }
}

// int bipod_partition_algorithm::find_sperner_triangle(int f0, int f1, int f2) {
//   // find the triangle with vertices of all three colours
//   auto p = lca->query(f0, f1);
//   auto r = lca->query(p, f2);
//
//   if (bt[r][1] == -1 || bt[r][2] == -1) {
//     // r has only one child, so it is one of the input faces
//     // and the other two faces are in the same subtree
//     if (r == f0) {
//       return lca->query(f1, f2);
//     } else if (r == f1) {
//       return lca->query(f0, f2);
//     } else /* r == f2 */ {
//       return lca->query(f0, f1);
//     }
//   }
//   // r has two children, two of the input faces are in one subtree
//   if (p != r) {
//     // f0 and f1 are in the same subtree of r
//     return p;
//   }
//   // f0 and f1 are in different subtrees of r, we need to know which one contains f2
//   auto q = lca->query(f0, f2);
//   if (q != r) {
//     // f0 and f2 are in the same subtree of r
//     return q;
//   }
//   return lca->query(f1, f2);
// }


void bipod_partition_algorithm::trichromatic_instance(const std::vector<half_edge>& e) {
  int i;
  for (i = 0; i < 3; i++) {
    assert(vertex_colours[e[i].target(g)] == vertex_colours[e[(i+1)%3].source(g)]);
  }

  int f[3] = { e[0].left_face(g), e[1].left_face(g), e[2].left_face(g) };

  bipod y;
  for (i = 0; i < 3 && f[i] != f[(i+1)%3]; i++) {}
  if (i < 3) {
    // Sperner triangle is f[i] when f[i] == f[i+1]
    y.tau = f[i];
  } else {
    // f[0], f[1], and f[2] are distinct, use LCA structure
    y.tau = find_sperner_triangle(f[0], f[1], f[2]);
  }

  // Construct the bipod
  grow_legs(y, bipods.size());
  for (i = 0; i < 3; i++) {
    assert(vertex_colours[foot(y, i)] != vertex_colours[foot(y, (i+1)%3)]);
  }
  // bipod is complete, add it to the list
  if (y.empty()) {
    face_colours[y.tau] = g.nFaces();
  } else {
    face_colours[y.tau] = bipods.size();
    bipods.push_back(y);
  }

  // orient y with respect to e[0],e[1],e[2] so that foot(y,r+i) is "between"
  // e[i-1] and e[i] on the boundary of the subproblem
  int r = 0;
  while (r < 3 && vertex_colours[e[0].source(g)] != vertex_colours[foot(y, r)]) {
    r++;
  }
  assert(r < 3);
  for (i = 0; i < 3; i++) {
    assert(vertex_colours[foot(y,(r+i)%3)] == vertex_colours[e[i].source(g)]);
  }

  for (i = 0; i < 3; i++) {
    // subproblem with portals at leg r+i, e[i], and leg (r+i+1)
    half_edge a0;
    if (y.legs[(r+i)%3].empty()) {
      a0 = half_edge(y.tau, (r+i)%3).reverse(g);
    } else {
      a0 = t[y.legs[(r+i)%3].back()];
    }
    half_edge a1;
    if (y.legs[(r+i+1)%3].empty()) {
      a1 = half_edge(y.tau, (r+i)%3).reverse(g);
    } else {
      a1 = t[y.legs[(r+i+1)%3].back()].reverse(g);
    }
    if (a0 == e[i].reverse(g)) {
      // empty subproblem, do nothing
    } else if (a0 == a1) {
      // bichromatic subproblem
      subproblems.push_back(std::vector<half_edge> {e[i], a0});
    } else {
      // trichromatic subproblem
      subproblems.push_back(std::vector<half_edge> {a0, e[i], a1});
    }
  }
}

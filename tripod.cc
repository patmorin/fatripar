#include<chrono>
#include<iostream>
#include<vector>
#include<assert.h>
#include "bfs.h"
#include "cotree.h"
#include "lca.h"
#include "tripod.h"

tripod_partition_algorithm::tripod_partition_algorithm(const triangulation& _g, const std::vector<half_edge>& _t,
                                                       int f0, std::vector<tripod>& _tripods)
    : g(_g),
      tripods(_tripods),
      vertex_colours(g.nVertices(), -1),
      face_colours(g.nFaces(), -1),
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


void tripod_partition_algorithm::partition(int f0) {
  tripod t0;
  t0.tau = f0;
  for (auto i = 0; i < 3; i++) {
    vertex_colours[g[f0].vertices[i]] = 0;
    t0.legs[i].push_back(g[f0].vertices[i]);
  }
  tripods.push_back(t0);
  face_colours[f0] = 0;

  subproblems.push_back(subproblem {half_edge(f0, 0).reverse(g)});
  while (!subproblems.empty()) {
    auto s = subproblems.back();
    subproblems.pop_back();
    if (s.size() < 3) {
      subcritical_instance(s);
    } else if (s.size() == 3) {
      trichromatic_instance(s);
    } else {
      assert(false);
    }
  }
}


int tripod_partition_algorithm::find_sperner_triangle(int f0, int f1, int f2) {
  // find the triangle with vertices of all three colours
  auto p = lca->query(f0, f1);
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

void tripod_partition_algorithm::subcritical_instance(const subproblem& s) {
  int chromacity = s.size();   // d is the "degree" (number of colours) in subproblem
  for (auto i = 0; i < chromacity; i++) {
    assert(face_colours[s[i].left_face(g)] == -1);
    assert(vertex_colours[s[i].target(g)] == vertex_colours[s[(i+1)%chromacity].source(g)]);
  }
  // create a new tripod y with two empty legs with feet on e[0].source() and
  // e[0].target, and a third leg from e0.opposite()
  tripod y;
  y.tau = s[0].left_face(g);
  int lu = (s[0].i+2) % 3;
  grow_leg(y, tripods.size(), lu);
  int j = 0;
  while (j < chromacity && vertex_colours[foot(y, lu)] != vertex_colours[s[j].target(g)]) {
    j++;
  }
  assert(j < chromacity);
  // third foot of y is in e[j].target(),...,e.[j+1].source()

  if (y.legs[lu].empty()) {
    // y is empty, colour y.tau, but don't add y to our list of tripods
    face_colours[y.tau] = g.nFaces();
    auto ea = s[0].next_edge_vertex(g);
    if (!tree_edge(ea) && face_colours[ea.left_face(g)] == -1) {
      // left subproblem is non-empty and has d-j tripods on its boundary
      subproblem sa(chromacity-j);
      sa[0] = ea;
      for (auto k = j+1; k < chromacity; k++) {
        sa[k-j] = s[k];
      }
      subproblems.push_back(sa);
    }
    auto eb = s[0].next_edge_face(g).reverse(g);
    if (!tree_edge(eb) && face_colours[eb.left_face(g)] == -1) {
      // right subproblem is non-empty and has j+1 tripods on its boundary
      subproblem sb(j+1);
      sb[0] = eb;
      for (auto k = 1; k <= j; k++) {
        sb[k] = s[k];
      }
      subproblems.push_back(sb);
    }
  } else {
    // y is non-empty, colour y.tau and add y to our list of tripods
    face_colours[y.tau] = tripods.size();
    tripods.push_back(y);
    // create left subproblem
    auto ea0 = s[0].next_edge_vertex(g);
    auto ea1 = t[y.legs[lu].back()];
    if (ea0 != ea1.reverse(g)) {
      subproblem sa(chromacity-j+1);
      sa[0] = ea0;
      sa[1] = ea1;
      for (auto k = j+1; k < chromacity; k++) {
        sa[k-j+1] = s[k];
      }
      subproblems.push_back(sa);
    }
    // create right subproblem
    auto eb0 = ea1.reverse(g);
    auto eb1 = s[0].next_edge_face(g).reverse(g);
    if (eb0 != eb1.reverse(g)) {
      subproblem sb(j+2);
      sb[0] = eb0;
      sb[1] = eb1;
      for (auto k = 1; k <= j; k++) {
        sb[k+1] = s[k];
      }
      subproblems.push_back(sb);
    }
  }
}

void tripod_partition_algorithm::trichromatic_instance(const subproblem& s) {
  int i;
  for (i = 0; i < 3; i++) {
    assert(face_colours[s[i].left_face(g)] == -1);
    assert(vertex_colours[s[i].target(g)] == vertex_colours[s[(i+1)%3].source(g)]);
  }

  int f[3] = { s[0].left_face(g), s[1].left_face(g), s[2].left_face(g) };

  tripod y;
  for (i = 0; i < 3 && f[i] != f[(i+1)%3]; i++) {}
  if (i < 3) {
    // Sperner triangle is f[i] when f[i] == f[i+1]
    y.tau = f[i];
  } else {
    // f[0], f[1], and f[2] are distinct, use LCA structure
    y.tau = find_sperner_triangle(f[0], f[1], f[2]);
  }

  // Construct the tripod
  grow_legs(y, tripods.size());
  for (i = 0; i < 3; i++) {
    assert(vertex_colours[foot(y, i)] != vertex_colours[foot(y, (i+1)%3)]);
  }

  if (y.empty()) {
    // y is empty, colour y.tau, but don't add y to our partition
    face_colours[y.tau] = g.nFaces();
  } else {
    // y is non-empty, could y.tau but don't add y to our partition
    face_colours[y.tau] = tripods.size();
    tripods.push_back(y);
  }

  // orient y with respect to e[0],e[1],e[2] so that foot(y,r+i) is "between"
  // s[i-1] and s[i] on the boundary of the subproblem
  int r = 0;
  while (r < 3 && vertex_colours[s[0].source(g)] != vertex_colours[foot(y, r)]) {
    r++;
  }
  assert(r < 3);
  for (i = 0; i < 3; i++) {
    assert(vertex_colours[foot(y,(r+i)%3)] == vertex_colours[s[i].source(g)]);
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
    if (a0 == s[i].reverse(g)) {
      // empty subproblem, do nothing
    } else if (a0 == a1) {
      // bichromatic subproblem
      subproblems.push_back(subproblem {s[i], a0});
    } else {
      // trichromatic subproblem
      subproblems.push_back(subproblem {a0, s[i], a1});
    }
  }
}

//
// Created by morin on 10/9/25.
//

#include<chrono>
#include<iostream>
#include<vector>
#include<assert.h>
#include "bfs.h"
#include "cotree.h"
#include "lca.h"
#include "monopod.h"

monopod_partition_algorithm::monopod_partition_algorithm(const triangulation& _g, const std::vector<half_edge>& _t,
                                                       int f0, std::vector<monopod>& _monopods)
    : g(_g),
      monopods(_monopods),
      vertex_colours(g.nVertices(), -1),
      edge_status(3*g.nFaces(), false),
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
    for (int i = 0; i < 3; i++) {
      assert(solid_edge(half_edge(f, i)));
    }
  }
  size_t nv = 0;
  for (auto y : monopods) {
    nv += y.size();
  }
  assert(nv == g.nVertices());
#endif // DEBUG
}

void monopod_partition_algorithm::partition(int f0) {
  // TODO: tripod and bipod partitions should also start with code like this
  assert(bt[f0][0] < 0);  // f0 must be the root of the cotree
  for (auto i = 0; i < 3; i++) {
    make_solid(half_edge(f0, i));
    monopod m;
    m.legs[0].push_back(g[f0].vertices[i]);
    vertex_colours[g[f0].vertices[i]] = monopods.size();
    monopods.push_back(m);
  }
  subproblems.push_back({half_edge(f0, 2).reverse(g),
                        half_edge(f0, 1).reverse(g),
                        half_edge(f0, 0).reverse(g)});

  while (!subproblems.empty()) {
    auto s = subproblems.back();

    for (size_t i = 0; i < s.size(); i++) {
      printf(i+1 == s.size() ? "%d-%d\n" : "%d-%d-", vertex_colours[s[i].source(g)],
        vertex_colours[s[i].target(g)]);
    }
    
    subproblems.pop_back();
    if (s.size() < 5) {
      subcritical_instance(s);
    } else if (s.size() == 5) {
      pentachromatic_instance(s);
    } else {
      assert(false);
    }
  }
}

void monopod_partition_algorithm::subcritical_instance(const subproblem& s) {
  int chromacity = s.size();   // number of colours/tripods on the boundary
  for (auto i = 0; i < chromacity; i++) {
    assert(solid_edge(s[i]));
    assert(vertex_colours[s[i].target(g)]
      == vertex_colours[s[(i+1)%chromacity].source(g)]);
  }
  // create a new monopod y with a foot on e[0].source() that grows from
  // e[0].opposite()
  monopod y;
  auto tau = s[0].next_edge_vertex(g);
  grow_leg(y, monopods.size(), tau.target(g));
  auto foot = y.empty() ? tau.target(g) : t[y.legs[0].back()].target(g);

  // find location of foot on the boundary
  // after this, the foot of y is in e[j].target(),...,e.[j+1].source()
  int j = 0;
  while (j < chromacity &&
         vertex_colours[foot] != vertex_colours[s[j].target(g)]) {
    j++;
  }
  assert(j < chromacity);
  
  if (y.legs[0].empty()) {
    // y is empty, make y.tau solid, but don't add y to our list of monopods
    auto el = tau;
    if (!solid_edge(el)) {
      make_solid(el);
      // left subproblem is non-empty, with chromacity-j tripods on its boundary
      subproblem sl(chromacity-j);
      sl[0] = el;
      for (auto k = j+1; k < chromacity; k++) {
        sl[k-j] = s[k];
      }
      subproblems.push_back(sl);
    }
    auto er = s[0].next_edge_face(g).reverse(g);
    if (!solid_edge(er)) {
      make_solid(er);
      // right subproblem is non-empty, with j+1 tripods on its boundary
      subproblem sr(j+1);
      sr[0] = er;
      for (auto k = 1; k <= j; k++) {
        sr[k] = s[k];
      }
      subproblems.push_back(sr);
    }
  } else {
    // y is non-empty, make y.tau solid and add y to our list of tripods
    make_solid(tau);
    monopods.push_back(y);
    // check left subproblem
    auto el0 = tau;;
    auto el1 = t[y.legs[0].back()];
    if (el0 != el1.reverse(g)) {
      // non-empty subproblem with chromacity-j+1 tripods on its boundary
      subproblem sl(chromacity-j+1);
      sl[0] = el0;
      sl[1] = el1;
      for (auto k = j+1; k < chromacity; k++) {
        sl[k-j+1] = s[k];
      }
      subproblems.push_back(sl);
    }
    // check right subproblem
    auto er0 = el1.reverse(g);
    auto er1 = s[0].next_edge_face(g).reverse(g);
    make_solid(er1);
    if (er0 != er1.reverse(g)) {
      // non-empty subproblem with chromacity-j+1 tripods on its boundary
      subproblem sr(j+2);
      sr[0] = er0;
      sr[1] = er1;
      for (auto k = 1; k <= j; k++) {
        sr[k+1] = s[k];
      }
      subproblems.push_back(sr);
    }
  }
}


void monopod_partition_algorithm::pentachromatic_instance(const subproblem& s) {
  for (auto i = 0; i < 4; i++) {
    assert(solid_edge(s[i]));
    assert(vertex_colours[s[i].target(g)]
      == vertex_colours[s[(i+1)%5].source(g)]);
  }

  // check if two of our portal edges bound the same triangle
  int i = 0;
  while (i < 5 && s[i].left_face(g) != s[(i+1)%5].left_face(g)) {
    i++;
  }
  if (i < 5) {
    // s[i] and s[i+1] bound the same triangle, use the third edge of this
    // triangle to make a trichromatic subproblem
    half_edge e0 = s[i].next_edge_vertex(g);
    make_solid(e0);
    subproblems.push_back({e0, s[(i+2)%5], s[(i+3)%5], s[(i+4)%5]});
    return;
  }
  // Find Sperner edge using LCA queries
  // TODO: No code here...
  return;
}



// const half_edge monopod_partition_algorithm::find_sperner_edge(
//   const subproblem &s) {
//
//   // find branching triangle for faces 0, 1, 2
//   int f1[3] = {s[0].left_face(g), s[1].left_face(g), s[2].left_face(g)};
//   int a1[3];
//   for (auto i = 0; i < 3; i++) {
//     a1[i] = lca->query(f1[i], f1[(i+1)%3]);
//   }
//   auto i1 = 0;
//   while (i1 < 3 && a1[i1] != a1[(i1+1)%3]) {
//     i1++;
//   }
//   assert(i1 < 3);
//   i1 = (i1 + 2) % 3;
//
//   // bt - a has f1[0], f1[1], and f1[2] in different components
//   auto a = a1[i1];
//   // auto p = bt[a][0];  // parent of a in the cotree bt
//
//   // bt - ap has a component that contains {f1[i1], f1[i1+1]], a} and a
//   // component that contains {f1[i1+2], p}
//
//   // determine which component of bt - ap contains s[3].left_face()
//   int f2[3] = {f1[i1], f1[(i1+1)%3], s[3].left_face(g)};
//   int a2[3] = {a1[i1], -1, -1};
//   for (auto i = 1; i < 3; i++) {
//     a2[i] = lca->query(f2[i],f2[(i+1)%3]);
//   }
//   auto i2 = 0;
//   while (i2 < 3 && a2[i2] != a2[(i2+1)%3]) {
//     i2++;
//   }
//   assert(i2 < 3);
//   i2 = (i2+2) % 3;
//
//   if (a1[i1] == a2[i2]) {
//     // s[3].left_face() is in same component as { f1[i1+2, p }.
//     return parent_half_edge(a);
//   } else {
//     // s[3].left_face() is in same component as {f1[i1], f1[i1+1], a}
//     return parent_half_edge(a2[i2]);
//   }
// }
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


  subproblems.push_back(subproblem {half_edge(f0, 0).reverse(g)});
  while (!subproblems.empty()) {
    auto s = subproblems.back();
    std::cout << "subproblem size " << s.size() << std::endl;
    subproblems.pop_back();
    if (s.size() < 4) {
      subcritical_instance(s);
    } else if (s.size() == 4) {
      quadrichromatic_instance(s);
    } else {
      assert(false);
    }
  }
}

void bipod_partition_algorithm::subcritical_instance(const subproblem& s) {
  int chromacity = s.size();   // number of colours/tripods on the boundary
  for (auto i = 0; i < chromacity; i++) {
    assert(solid_edge(s[i]));
    assert(vertex_colours[s[i].target(g)]
      == vertex_colours[s[(i+1)%chromacity].source(g)]);
  }
  // create a new tripod y with two empty legs with feet on e[0].source() and
  // e[0].target, and a third leg starting at e0.opposite()
  bipod y;
  y.tau = s[0].next_edge_vertex(g);
  grow_leg(y, bipods.size(), 1);

  // find location of third foot on the boundary
  // after this, the third foot of y is in e[j].target(),...,e.[j+1].source()
  int j = 0;
  while (j < chromacity &&
         vertex_colours[foot(y, 1)] != vertex_colours[s[j].target(g)]) {
    j++;
  }
  assert(j < chromacity);
  
  if (y.legs[1].empty()) {
    // y is empty, make y.tau solid, but don't add y to our list of bipods
    auto el = y.tau;
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
    make_solid(y.tau);
    bipods.push_back(y);
    // check left subproblem
    auto el0 = y.tau;;
    auto el1 = t[y.legs[1].back()];
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


void bipod_partition_algorithm::quadrichromatic_instance(const subproblem& s) {
  for (auto i = 0; i < 4; i++) {
    assert(solid_edge(s[i]));
    assert(vertex_colours[s[i].target(g)]
      == vertex_colours[s[(i+1)%4].source(g)]);
  }
  
  // TODO: even the code below contains a bug
  int i = 0;
  while (i < 4 && s[i].left_face(g) != s[(i+1)%4].left_face(g)) {
    i++;
  }
  if (i < 4) {
    // use an empty bipod to get a trichromatic problem
    subproblem s0(3);
    s0[0] = s[i].next_edge_vertex(g);
    make_solid(s0[0]);
    for (int j = 0; j < 2; j++) {
      s0[j+1] = s[(i+j+2)%4];
    }
    subproblems.push_back(s0);
    return;
  } 
  // Find Sperner edge using LCA queries
}
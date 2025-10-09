//
// Created by morin on 10/9/25.
//

#ifndef FATRIPAR_MONOPOD_H
#define FATRIPAR_MONOPOD_H


#include "triangulation.h"
#include "lca.h"

struct monopod {
  std::vector<int> legs[1]; // for consistency with bipods and tripods
  
  const size_t size() const {
    return legs[0].size();
  }
  
  const bool empty() const {
      return legs[0].empty();
  }
};

// An instance of the monopod_partition_algorithm.
// This object is designed to be created and then forgotten.
// It's only purpose is to fill in the monopods array
// provided in its constructor.
class monopod_partition_algorithm {
protected:
  const triangulation& g;
  std::vector<monopod> &monopods;
  std::vector<int> vertex_colours;
  std::vector<bool> edge_status;
  const std::vector<half_edge>& t;
  std::vector<int[3]> bt;
  lca_structure *lca;

  class subproblem final {
  private:
    size_t chromacity;
    half_edge portals[5];

  public:
    subproblem(size_t _n) : chromacity(_n) { }
    subproblem(std::initializer_list<half_edge> _e) : chromacity(_e.size()) {
      size_t i = 0;
      for (auto x : _e) {
        portals[i++] = x;
      }
    }
    half_edge& operator[](size_t i) { return portals[i]; }
    const half_edge& operator[](size_t i) const { return portals[i]; }
    const size_t size() const { return chromacity; }
  };

  std::vector<subproblem> subproblems;

  bool solid_edge(const half_edge& e) const {
    auto e2= e.reverse(g);
    return edge_status[3*e.f + e.i] || edge_status[3*e2.f + e2.i];
  }

  bool tree_root(int v) const { return t[v].f < 0; }

  void make_solid(const half_edge& e) {
    edge_status[3*e.f + e.i] = true;
    auto e2= e.reverse(g);
    edge_status[3*e2.f + e2.i] = true;
  }

  // int foot(const monopod& y, int lu) const {
  //   assert(lu == 0);
  //   if (y.legs[lu].empty()) {
  //     return g[y.tau.f].vertices[(y.tau.i+lu)%3];
  //   } else {
  //     return t[y.legs[lu].back()].target(g);
  //   }
  // }

  void grow_leg(monopod& y, int c, int u0) {
    int u = u0;
    while (vertex_colours[u] == -1) {
      y.legs[0].push_back(u);
      vertex_colours[u] = c;
      make_solid(t[u]);
      u = t[u].target(g);
    }
  }

  // const half_edge find_sperner_edge(const subproblem &s);

  // void grow_legs(monopod& y, int c) {
  //   for (auto lu = 0; lu < 1; lu++) {
  //     grow_leg(y, c, lu);
  //   }
  // }

  // this code is used because nodes in the cotree bt are aligned so that the parent
  // of a node is at index 0, but faces (triangles) in g are aligned
  // arbitrarily, so we need to locate the parent of a in g[a].neighbours
  const half_edge parent_half_edge(int a)  const {
    // Sperner edge is dual to edge from a1[i1] to its parent
    auto p = bt[a][0];  // parent of a1[i1] in cotree
    int r = 0;
    while (r < 3 && g[a].neighbours[r] != p) {
      r++;
    }
    assert(r < 3);
    return half_edge(a, r);
  }

  void subcritical_instance(const subproblem& s);
  void pentachromatic_instance(const subproblem& s);


public:
  monopod_partition_algorithm(const triangulation& _g, const std::vector<half_edge>& _t, int f0, std::vector<monopod>& _monopods);

  void partition(int f0);
};

// A monopod partition P of the vertices of a triangulaton g, so that tw(g/P) <= 3.
class monopod_partition {
public:
  const triangulation& g;
  std::vector<monopod> monopods;

  // Advanced constructor. Requires that f0 be incident to the root of t
  monopod_partition(const triangulation& _g, std::vector<half_edge>& t, int f0) : g(_g), monopods() {
    monopod_partition_algorithm(g, t, f0, monopods);
  }

  // Standard partition that gives H*P*K_3 where tw(H) <= 3
  monopod_partition(const triangulation& _g) : g(_g), monopods() {
    std::vector<half_edge> t(g.nVertices(), half_edge(-2,-2));
    bfs_tree(g, half_edge(0, 0), t);
    monopod_partition_algorithm(g, t, 0, monopods);
  }
};

#endif //FATRIPAR_MONOPOD_H
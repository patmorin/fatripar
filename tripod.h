
#ifndef __TRIPOD_H
#define __TRIPOD_H

#include "triangulation.h"
#include "lca.h"

struct tripod {
  int tau;
  std::vector<int> legs[3];

  const bool empty() {
    return legs[0].empty() && legs[1].empty() && legs[2].empty();
  }
};

// An instance of the tripod_partition_algorithm.
// This object is designed to be created and then forgotten.
// It's only purpose is to fill in the tripods array
// provided in its constructor.
class tripod_partition_algorithm {
protected:
  const triangulation& g;
  std::vector<tripod> &tripods;
  std::vector<int> vertex_colours;
  std::vector<int> face_colours;
  const std::vector<half_edge>& t;
  std::vector<int[3]> bt;
  lca_structure *lca;

  struct subproblem {
    size_t chromacity;
    half_edge e[3];

    subproblem(size_t _n) : chromacity(_n) { }
    subproblem(std::initializer_list<half_edge> _e) : chromacity(_e.size()) {
      size_t i = 0;
      for (auto x : _e) {
        e[i++] = x;
      }
    }
    half_edge& operator[](size_t i) { return e[i]; }
    const half_edge& operator[](size_t i) const { return e[i]; }
    const size_t size() const { return chromacity; }
  };

  std::vector<subproblem> subproblems;

  bool tree_edge(const half_edge e) const {
    return t[e.source(g)] == e || t[e.target(g)] == e.reverse(g);
  }

  int foot(const tripod& y, int lu) const {
    if (y.legs[lu].empty()) {
      return g[y.tau].vertices[lu];
    } else {
      return t[y.legs[lu].back()].target(g);
    }
  }

  void grow_leg(tripod& y, int c, int lu) {
    int u = g[y.tau].vertices[lu];
    while (vertex_colours[u] == -1) {
      y.legs[lu].push_back(u);
      vertex_colours[u] = c;
      u = t[u].target(g);
    }
  }

  void grow_legs(tripod& y, int c) {
    for (auto lu = 0; lu < 3; lu++) {
      grow_leg(y, c, lu);
    }
  }

  int find_sperner_triangle(int f1, int f2, int f3);

  void subcritical_instance(const subproblem& s);
  void trichromatic_instance(const subproblem& s);

  public:
  tripod_partition_algorithm(const triangulation& _g,
                             const std::vector<half_edge>& _t,
                             int f0,
                             std::vector<tripod>& _tripods);

  void partition(int f0);
};

// A tripod partition P of the vertices of a triangulaton g,
// so that tw(g/P) <= 3.
class tripod_partition {
public:
  const triangulation& g;
  std::vector<tripod> tripods;

  // Advanced constructor. Requires that f0 be incident to the root of t
  tripod_partition(const triangulation& _g, std::vector<half_edge>& t, int f0) : g(_g), tripods() {
    tripod_partition_algorithm(g, t, f0, tripods);
  }

  // Standard partition that gives H*P*K_3 where tw(H) <= 3
  tripod_partition(const triangulation& _g) : g(_g), tripods() {
    std::vector<half_edge> t(g.nVertices(), half_edge(-2,-2));
    bfs_tree(g, half_edge(0, 0), t);
    tripod_partition_algorithm(g, t, 0, tripods);
  }
};
#endif // __TRIPOD_H

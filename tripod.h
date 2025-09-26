
#ifndef __TRIPOD_H
#define __TRIPOD_H

#include "triangulation.h"
#include "lca.h"

struct tripod {
  int tau;
  std::vector<int> legs[3];
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
  // half_edge e0;

  std::vector<std::vector<half_edge>> subproblems;

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

  int find_sperner_triangle(int f1, int f2, int f3);

  void monochromatic_instance(const half_edge e0 );
  void bichromatic_instance(const half_edge e0, const half_edge e1);
  void trichromatic_instance(const std::vector<half_edge>& e);

  public:
  tripod_partition_algorithm(const triangulation& _g, const std::vector<half_edge>& _t, int f0, std::vector<tripod>& _tripods);

  void partition(int f0);
};

// A tripod partition P of the vertices of a triangulaton g, so that tw(g/P) <= 3.
class tripod_partition {
protected:
  const triangulation& g;
  std::vector<tripod> tripods;

public:
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


#ifndef __TRIPOD_H
#define __TRIPOD_H

#include "triangulation.h"
#include "lca.h"

struct tripod {
  int tau;
  std::vector<int> legs[3];
};


class tripod_partition_algorithm {
protected:
  const triangulation& g;
  std::vector<tripod> tripods;
  std::vector<int> vertex_colours;
  std::vector<int> face_colours;
  std::vector<half_edge> t;
  std::vector<int[3]> bt;
  lca_structure *lca;

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

  int find_sperner_triangle(const half_edge e0, const half_edge e1, const half_edge e2);

  void monochromatic_instance(const half_edge e0 );
  void bichromatic_instance(const half_edge e0, const half_edge e1);
  void trichromatic_instance(const std::vector<half_edge>& e);

  public:
  tripod_partition_algorithm(const triangulation& _g, std::vector<tripod> _tripods);
};

class tripod_partition {
protected:
  const triangulation& g;
  std::vector<tripod> tripods;

public:
  tripod_partition(const triangulation& _g) : g(_g), tripods() {
    tripod_partition_algorithm(g, tripods);
  }
};
#endif // __TRIPOD_H

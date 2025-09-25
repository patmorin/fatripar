
#ifndef __TRIPOD_H
#define __TRIPOD_H

#include "triangulation.h"

struct tripod {
  int tau;
  std::vector<int> legs[3];
};



class tripod_partition {
protected:
  const triangulation& g;
  std::vector<tripod> tripods;
  std::vector<int> vertex_colours;
  std::vector<int> face_colours;
  std::vector<half_edge> t;
  std::vector<int[3]> bt;
  bool tree_edge(const half_edge e) const {
    return t[e.source(g)] == e || t[e.target(g)] == e.reverse(g);
  }

  public:
  tripod_partition(const triangulation& _g);
  void monochromatic_instance(const half_edge e0);
  void bichromatic_instance(const half_edge e0, const half_edge e1);
};

#endif // __TRIPOD_H

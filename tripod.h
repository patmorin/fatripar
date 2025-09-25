
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

  public:
  tripod_partition(const triangulation& _g);
  void monochromatic_instance(const half_edge e);

};

#endif // __TRIPOD_H

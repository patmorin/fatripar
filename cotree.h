//
// Created by morin on 9/25/25.
//

#ifndef LHP_II_COTREE_H
#define LHP_II_COTREE_H
#include "triangulation.h"

void cotree(const triangulation& g, const std::vector<half_edge>& t, const half_edge& e0,
  std::vector<int[3]>& bt);

#endif //LHP_II_COTREE_H
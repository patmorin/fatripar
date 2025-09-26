//
// Created by morin on 9/25/25.
//

#ifndef LHP_II_COTREE_H
#define LHP_II_COTREE_H
#include "triangulation.h"

// Compute the cotree of the spanning-tree t, rooted at f0
// Format: for each vertex v of g, t[v].source()=v and t[v].target()=parent(v)
//         except if v is the root, in which case t[v].t is negative.
void cotree(const triangulation& g, const std::vector<half_edge>& t, int f0,
  std::vector<int[3]>& bt);

#endif //LHP_II_COTREE_H
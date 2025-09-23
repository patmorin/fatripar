#include<iostream>
#include<cassert>
#include "lca.h"


lca_structure::lca_structure(const std::vector<int[3]> &bt, int r) : node2tour(bt.size(), -1)
{
  // size_t n = bt.size();

  std::vector<int> tour;
  int prev = -1;
  int depth = 0;
  int u = r;
  int next;
  while (u != -1) {
    // std::cout << u << std::endl;
    tour.push_back(u);
    depths.push_back(depth);
    if (prev == bt[u][0]) {  // just came from parent
      if (bt[u][1] >= 0) {
        depth++;
        next = bt[u][1];  // go to left child
      } else if (bt[u][2] >= 0) {
        depth++;
        next = bt[u][2];  // go to right child
      } else {
        depth--;
        next = bt[u][0];  // go to parent
      }
    } else if (prev == bt[u][1]) { // just came from left child
      if (bt[u][2] >= 0) {
        depth++;
        next = bt[u][2];   // go to left child
      } else {
        depth--;
        next = bt[u][0];   // go to parent
      }
    } else{ // just came from right child
      depth--;
      next = bt[u][0]; // go to parent
    }
    prev = u;
    u = next;
  }
  std::cout << tour.size() << " " << depths.size() << std::endl;
  std::cout << node2tour.size() << std::endl;

  // Map each node to its first occurrence in the tour
  for (size_t i = 0; i < tour.size(); i++) {
    if (node2tour[tour[i]] == -1) {
      node2tour[tour[i]] = i;
    }
  }
  for (size_t i = 0; i < node2tour.size(); i++) {
    assert(node2tour[i] >= 0);
  }

  rmq_structure rmq(depths);
  // const std::vector<int>& _data
}

int lca_structure::query(int v, int w) const
{
  return 0;
}

#include<iostream>
#include<cassert>
#include "lca.h"

lca_structure::lca_structure(const std::vector<int[3]> &bt, int r) :
      tour(2*bt.size()-1, -1),
      depths(2*bt.size()-1, -1),
      node2tour(bt.size(), -1)
{
  // size_t n = bt.size();

  int prev = -1;
  int depth = 0;
  int u = r;
  int next;
  int i = 0;
  while (u != -1) {
    // std::cout << u << std::endl;
    tour[i] = u;
    depths[i++] = depth;
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
  // std::cout << tour.size() << " " << depths.size() << std::endl;
  // std::cout << node2tour.size() << std::endl;

  // Map each node to its first occurrence in the tour
  for (size_t i = 0; i < tour.size(); i++) {
    if (node2tour[tour[i]] == -1) {
      node2tour[tour[i]] = i;
    }
  }
  for (size_t i = 0; i < node2tour.size(); i++) {
    assert(node2tour[i] >= 0);
  }

  // std::cout << std::endl;
  // std::cout << std::endl;
  rmq = new rmq_opt(depths);


  // if (bt.size() >= 1000000) {
  //   for (int z = 1; z < 100; z++) {
  //     int x = z*10000;
  //     int y = x+7;
  //     std::cout << std::endl;
  //     int mini = x;
  //     for (int i = x; i <= y; i++) {
  //       if (depths[i] < depths[mini]) {
  //         mini = i;
  //       }
  //       std::cout << depths[i] << ",";
  //     }
  //     int m = rmq->query(x,y);
  //     std::cout << std::endl
  //               << "mini = " << mini << ", "
  //               << "m = " << m << ", "
  //               << "depths[mini] = " << depths[mini] << ", "
  //               << "depths[m] = " << depths[m]
  //               << std::endl << std::endl;
  //     assert(m == mini);
  //   }
  // }
  // const std::vector<int>& _data
}

int lca_structure::query(int v, int w) const
{
  int x = node2tour[v];
  int y = node2tour[w];
  int u = rmq->query(x, y);
  return tour[u];
}

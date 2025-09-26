#include<iostream>
#include<cassert>
#include "lca.h"

lca_structure::lca_structure(const std::vector<int[3]> &_bt, int r) :
      bt(_bt),
      tour(2*bt.size()-1, -1),
      depths(2*bt.size()-1, -1),
      node2tour(bt.size(), -1)
{
  // The following implemnents a constant memory Euler tour of bt
  int prev = -1;
  int depth = 0;
  int u = r;
  int next;
  int i = 0;
  while (u != -1) {
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
  // Map each node to its first occurrence in the tour
  for (size_t i = 0; i < tour.size(); i++) {
    if (node2tour[tour[i]] == -1) {
      node2tour[tour[i]] = i;
    }
  }
  // sanity check, make sure every node appears in the tour
  for (size_t i = 0; i < node2tour.size(); i++) {
    assert(node2tour[i] >= 0);
  }
  rmq = new rmq_structure(depths);
}

int lca_structure::query(int v, int w) const
{
  int x = node2tour[v];
  int y = node2tour[w];
  int u;
  if (x <= y) {
    u = rmq->query(x, y);
  } else {
    u = rmq->query(y, x);
  }
  // assert(tour[u] == query_trivial(v, w));
  return tour[u];
}

int lca_structure::query_trivial(int v, int w) const {
  // compute the depth of v by walking to the root
  int dv = 0;
  int vp = v;
  while (bt[vp][0] != -1) {
    dv++;
    vp = bt[vp][0];
  }
  // compute the depth of w by walking to the root
  int dw = 0;
  int wp = w;
  while (bt[wp][0] != -1) {
    dw++;
    wp = bt[wp][0];
  }
  // ensure that dv >= dw
  if (dv < dw) {
    std::swap(v, w);
    std::swap(dv, dw);
  }
  // walk from v up to depth(w)
  while (dv > dw) {
    v = bt[v][0];
    dv--;
  }
  // walk up in parallel until v and w meet
  while (v != w) {
    v = bt[v][0];
    w = bt[w][0];
  }
  return v;
}
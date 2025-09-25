#include<queue>
#include<vector>
#include<iostream>

#include "triangulation.h"
#include "bfs.h"


void bfs_tree(const triangulation &g, const half_edge e0, std::vector<half_edge> &t) {
  assert(t.size() == g.nVertices());

  // first compute the BFS tree rooted at e0.source()
  size_t v0 = e0.source(g);
  t[v0] = half_edge(-1,-1);
  std::queue<half_edge> q;
  q.push(e0);

  while(!q.empty()) {
    half_edge e(q.front());
    q.pop();
    // process the neighbours of e.source()
    for (half_edge e2 = e.next_edge_vertex(g); e2.target(g) != e.target(g);
      e2 = e2.next_edge_vertex(g)) {
      auto w = e2.target(g);
      if (t[w].t == -2) {
        t[w] = e2.reverse(g);
        q.push(t[w]);
      }
    }
  }
}

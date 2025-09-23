#include<queue>
#include<vector>
#include<iostream>

#include "triangulation.h"
#include "bfs.h"

void triple_bfs(const triangulation &g, int f0,
                std::vector<triangulation::half_edge> &t) {
  std::queue<triangulation::half_edge> q;
  // -2 denotes a not-yet-discovered vertex
  std::vector<int> parents(g.nVertices(), -2);
  triangulation::half_edge e(g, 0, 0);

  // the three vertices of f0 have parent -1
  for (auto j = 0; j < 3; j++) {
    parents[g[f0].vertices[j]] = -1;
    q.push(triangulation::half_edge(g, f0, j));
  }
  while(!q.empty()) {
    triangulation::half_edge e(q.front());
    q.pop();
    // process the neighbours of e.source()
    int v = e.source();
    for (triangulation::half_edge e2 = e.next_edge(); e2.target() != e.target(); e2 = e2.next_edge()) {
      auto w = e2.target();
      if (parents[w] == -2) {
        t[w] = e2.reverse();
        q.push(t[w]);
        parents[w] = v;
      }
    }
  }
  // std::cout << parent << std::endl;
}

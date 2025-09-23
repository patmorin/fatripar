#include<queue>
#include<vector>
#include<iostream>

#include "triangulation.h"
#include "bfs.h"

void bfs(const triangulation &g, const half_edge e0,
                std::vector<half_edge> &t,
                std::vector<int[3]> &bt) {

  assert(t.size() == g.nVertices());
  assert(bt.size() == g.nFaces());

  // first compute the BFS tree
  // e0.source(g) is the root vertex
  int v0 = e0.source(g);
  int f0 = e0.left_face(g);
  t[v0] = half_edge(-1,-1);
  std::queue<half_edge> q;
  q.push(e0);

  while(!q.empty()) {
    half_edge e(q.front());
    q.pop();
    // process the neighbours of e.source()
    for (half_edge e2 = e.next_edge(g); e2.target(g) != e.target(g);
         e2 = e2.next_edge(g)) {
      auto w = e2.target(g);
      if (t[w].t == -2) {
        t[w] = e2.reverse(g);
        q.push(t[w]);
      }
    }
  }

  // now compute the cotree
  // copy the triangulation adjacencies
  for (size_t f = 0; f < g.nFaces(); f++) {
    for (auto j = 0; j < 3; j++) {
      bt[f][j] = g[f].neighbours[j];
    }
  }
  // delete missing edges
  for (size_t v = 0; v < g.nVertices(); v++) {
    bt[t[v].t][t[v].i] = -1;
  }
  std::queue<int> q2;
  q2.push(f0);
  for (auto j = 0; j < 3; j++)
    std::cout << bt[f0][j] << ",";
  std::cout << std::endl;
  // bt[f0][0] = -1;
}

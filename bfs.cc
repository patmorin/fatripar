#include<queue>
#include<vector>
#include<iostream>

#include "triangulation.h"
#include "bfs.h"


// Rotate an array of length 3
inline void rot3(int *a, int shift) {
  if (shift == 1) {
    auto a0 = a[0];
    a[0] = a[2];
    a[2] = a[1];
    a[1] = a0;
  } else if (shift == 2) {
    auto a0 = a[0];
    a[0] = a[1];
    a[1] = a[2];
    a[2] = a0;
  } else{
    assert(shift == 0);
  }
}

// Rotate an array of length 3 so that a[0] == val
inline void align3(int *a, int val) {
  auto i = 0;
  while (a[i] != val) {
    i++;
    assert(i < 3);
  }
  rot3(a, (3-i) % 3);
}

void bfs_tree_cotree(const triangulation &g, const half_edge e0,
                std::vector<half_edge> &t,
                std::vector<int[3]> &bt) {

  assert(t.size() == g.nVertices());
  assert(bt.size() == g.nFaces());

  // first compute the BFS tree rooted at e0.source()
  size_t v0 = e0.source(g);
  size_t f0 = e0.left_face(g);
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

  // for (size_t v = 0; v < g.nVertices(); v++) {
  //   if (v != v0) {
  //     std::cout << t[v].source(g) << "->" << t[v].target(g) << ",";
  //   }
  // }
  // std::cout << std::endl;
  //
  // std::cout << "ok" << std::endl;

  // compute the cotree rooted at e0.left_face()
  // copy the triangulation adjacencies
  for (size_t f = 0; f < g.nFaces(); f++) {
    for (auto j = 0; j < 3; j++) {
      bt[f][j] = g[f].neighbours[j];
    }
  }
  // delete missing edges
  for (size_t v = 0; v < g.nVertices(); v++) {
    if (v != v0) {
      // std::cout << v << " " << t[v].t << " " << t[v].i << std::endl;
      assert(t[v].t >= 0 && (size_t)t[v].t < g.nFaces());
      assert(t[v].i >= 0 && (size_t)t[v].i < g.nVertices());
      assert(bt[t[v].t][t[v].i] == g[t[v].t].neighbours[t[v].i]);
      bt[t[v].t][t[v].i] = -1;
      half_edge r = t[v].reverse(g);
      bt[r.t][r.i] = -1;
    }
  }

  // for (size_t f = 0; f < g.nFaces(); f++) {
  //   std::cout << f << " (";
  //   for (auto j = 0; j < 3; j++) {
  //     std::cout << bt[f][j] << ((j == 2) ? ")" : ",");
  //   }
  //   std::cout << " (";
  //   for (auto j = 0; j < 3; j++) {
  //     std::cout << g[f].neighbours[j] << ((j == 2) ? ")" : ",");
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl;


  // rotate arrays so that bt[f][0] is parent, bt[f][1] is left child
  // and bt[f][2] is right child
  rot3(bt[f0], 1);
  int prev = -1;
  int u = f0;
  int next;
  while (u != -1) {
    if (prev == bt[u][0]) {  // just came from parent
      if (bt[u][1] >= 0) {
        next = bt[u][1];  // go to left child
        align3(bt[next], u);
      } else if (bt[u][2] >= 0) {
        next = bt[u][2];  // go to right child
        align3(bt[next], u);
      } else {
        next = bt[u][0];  // go to parent
      }
    } else if (prev == bt[u][1]) { // just came from left child
      if (bt[u][2] >= 0) {
        next = bt[u][2];   // go to left child
        align3(bt[next], u);
      } else {
        next = bt[u][0];   // go to parent
      }
    } else{ // just came from right child
      next = bt[u][0]; // go to parent
    }
    prev = u;
    u = next;
  }

  // for (size_t f = 0; f < g.nFaces(); f++) {
  //   std::cout << f << " (";
  //   for (auto j = 0; j < 3; j++) {
  //     std::cout << bt[f][j] << ((j == 2) ? ")" : ",");
  //   }
  //   std::cout << " (";
  //   for (auto j = 0; j < 3; j++) {
  //     std::cout << g[f].neighbours[j] << ((j == 2) ? ")" : ",");
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl;

}

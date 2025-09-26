#ifndef __LCA_H
#define __LCA_H

#include<vector>
#include "rmq.h"

// An O(n) space O(1) query time lowest common ancestor structure for binary trees
class lca_structure {
protected:
  const std::vector<int[3]> &bt; // the input tree
  std::vector<int> tour;         // Euler tour of bt
  std::vector<int> depths;       // The depth of each node in tour
  std::vector<int> node2tour;    // The index of the first occurence of each node in tour
  rmq_structure *rmq;            // Range minimum structure on depths array

  int query_trivial(int v, int w) const; // a trivial implementation (for debugging)

public:
  lca_structure(const std::vector<int[3]> &bt, int r);
  ~lca_structure() { delete rmq; }
  int query(int v, int w) const;
};

#endif //__LCA_H

#ifndef __LCA_H
#define __LCA_H

#include<vector>
#include "rmq.h"

class lca_structure {
protected:
  const std::vector<int[3]> &bt;
  std::vector<int> tour;
  std::vector<int> depths;
  std::vector<int> node2tour;
  rmq_opt *rmq;
  int query_trivial(int v, int w) const;

public:
  lca_structure(const std::vector<int[3]> &bt, int r);
  ~lca_structure() { delete rmq; }
  int query(int v, int w) const;
};

#endif //__LCA_H

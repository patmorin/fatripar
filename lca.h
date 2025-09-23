#ifndef __LCA_H
#define __LCA_H

#include<vector>
#include "rmq.h"

class lca_structure {
protected:
  std::vector<int> depths;
  std::vector<int> node2tour;
  rmq_structure rmq;

public:
  lca_structure(const std::vector<int[3]> &bt, int r);
  int query(int v, int w) const;
};

#endif //__LCA_H

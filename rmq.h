#ifndef __RMQ_H
#define __RMQ_H

#include<iostream>
#include<vector>

class rmq_structure {
private:
  int d;
  int *m;

  int& lookup(int i, int k) { return m[k*data.size()+i]; }
  int find(int i, int k) const { return m[k*data.size()+i]; }
protected:
  std::vector<int> const &data;

public:

  rmq_structure(const std::vector<int>& _data);

  int query(int i, int j) const;
};

#endif //__RMQ_H

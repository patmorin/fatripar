#ifndef __RMQ_H
#define __RMQ_H

#include<iostream>
#include<vector>

/**
 * O(n log n) space O(1) query time RMQ data structure
 */
class rmq_big {
private:
  std::vector<int> const &data;
  int n;
  int logn;
  std::vector<int> m;

  int& lookup(int i, int k) { return m[k*data.size()+i]; }
  int find(int i, int k) const { return m[k*data.size()+i]; }
protected:

public:

  rmq_big(const std::vector<int>& _data);

  int query(int i, int j) const;
};


/**
 * O(n) space, O(1) query time structure
 */
class rmq_opt {
protected:
  std::vector<int> const &data;
  int n;
  const int d;
  const int bs;
  std::vector<int> blockmins;
  std::vector<int> blocks;
  std::vector<int> blocktabs;
  std::vector<int> suffixmins;
  std::vector<int> prefixmins;
  std::vector<int> tables;

  rmq_big *rmqb;

public:
  rmq_opt(const std::vector<int>& _data);
  ~rmq_opt() { delete rmqb; };
  int query(int x, int y) const;
};

#endif //__RMQ_H

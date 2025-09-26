//
// Implementation of O(n) space O(1) query time range minimum data structure
//
#ifndef __RMQ_H
#define __RMQ_H

#include<iostream>
#include<vector>

// O(n log n) space O(1) query time RMQ data structure
class rmq_big {
private:
  std::vector<int> const &data;
  int n;                          // data.size()
  int logn;                       // floor(log2(n))
  std::vector<int> m;             // 2d table, m[x,k] stores index of min{x,x+1,...,x+2^k-1}

  int& lookup(int i, int k) { return m[k*data.size()+i]; }
  int find(int i, int k) const { return m[k*data.size()+i]; }

public:
  rmq_big(const std::vector<int>& _data);
  int query(int i, int j) const; // return index of minimum value in data[x,x+1,...,y]
};



// O(n) space, O(1) query time data structure (Bender and Farach-Colton (2000))
class rmq_structure {
protected:
  std::vector<int> const &data;
  int n;                        // data.size()
  const int bs;                 // block size (~(1/2)log n)
  std::vector<int> blockminis;  // the index (in data) of the minimum value in each block
  std::vector<int> blockmins;   // the minimum value in each block
  std::vector<int> blocktabs;   // blocktabs[i] gives offset into tables for queries in block i
  std::vector<int> suffixmins;  // the index (in data) of min{data[x,x+1,...,bs*(1+x/bs)-1]}
  std::vector<int> prefixmins;  // the index (in data) of min{data[bs*(x/bs),bs*(x/bs+1),...,x}
  std::vector<int> tables;      // 3d array, to answer queries in single blocks
  rmq_big *rmqb;                // O(n log n) space structure for finding min in ranges of blocks

public:
  rmq_structure(const std::vector<int>& _data);
  ~rmq_structure() { delete rmqb; };
  int query(int x, int y) const; // return index of minimum value in data[x,x+1,...,y]
};

#endif //__RMQ_H

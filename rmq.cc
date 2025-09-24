#include "rmq.h"

#include<cassert>

// class rmq_fast {
// protected:
//   std::vector<int> const &data;
//
// };

class indirector {
public:
  std::vector<int> &data;
  std::vector<int> &indices;

  indirector(std::vector<int> &_data, std::vector<int> &_indices) : data(_data), indices(_indices) {}

  size_t size() { return indices.size(); }
  int& operator[](int i) { return data[indices[i]]; }
};

class rmq_blocked {
protected:
  std::vector<int> const &data;
  const int d;
  const int bs;
  std::vector<int> blockmins;
  std::vector<int> suffixmins;
  std::vector<int> prefixmins;


public:
  rmq_blocked(const std::vector<int>& _data);
};

rmq_blocked::rmq_blocked(const std::vector<int>& _data) :
    data(_data),
    d((8*sizeof(data.size()) - __builtin_clz(data.size()) - 1)/2),
    bs(d),
    blockmins(data.size()/bs+1, -1),
    suffixmins(data.size(), -1),
    prefixmins(data.size(), -1)
{
  int n = data.size();
  assert(n > 0);

  // compute the minimum withi each block and suffixmins for each block
  int mini = 0;
  for (auto i = 1; i < n; i++) {
    if (i % bs == 0) {
      blockmins[i/bs - 1] = mini;
      mini = i;
    }
    if (data[i] < data[mini]) mini = i;
    prefixmins[i] = mini;
  }

  // compute the suffix minima for each block
  mini = n-1;
  for (int i = n-1; i >= 0; i--) {
    if ((i+1) % bs == 0) mini = i;
    if (data[i] < data[mini]) mini = i;
    suffixmins[i] = mini;
  }
}



// TODO: This is the O(n log n) space, O(1) query time data structure.
rmq_structure::rmq_structure(const std::vector<int>& _data) : data(_data)
{
  int n = data.size();
  assert(n > 0);
  d = 8*sizeof(n) - __builtin_clz(n) - 1;
  assert((1<<d) <= n);
  assert(2*(1<<d) >= n);

  m = new int[d*n];

  for (auto i = 0; i < n; i++) {
    lookup(i, 0) = i;
  }

  for (auto k = 1; k < d; k++) {
    int bs = 1 << k;
    for (auto i = 0; i < n-bs; i++) {
      int a = lookup(i, k-1);
      int b = lookup(i+bs/2,k-1);
      if (data[a] <= data[b]) {
        lookup(i,k) = a;
      } else {
        lookup(i,k) = b;
      }
    }
  }
  // std::cout << "n = " << n << " d=" << d << std::endl;
}

int rmq_structure::query(int x, int y) const {
  assert(x <= y);
  if (x == y) return x;
  int r = y - x + 1;
  int k = 8*sizeof(r) - __builtin_clz(r) - 1;
  int bs = (1<<k);
  assert(bs <= r);
  assert(2*bs >= r);

  // std::cout << std::endl;
  // for (int kk = 0; kk <= k; kk++) {
  //   std::cout << kk << ":";
  //   for (int i = x; i <= y-(1<<kk)+1; i++) {
  //     std::cout << data[lookup(i, kk)] << ",";
  //   }
  //   std::cout << std::endl;
  // }
  // std::cout << std::endl << std::endl;
  // std::cout << "x = " << x << ","
  //           << "y = " << y << ","
  //           << "r = " << r << ","
  //           << "k = " << k << ","
  //           << "bs = " << bs
  //           << std::endl;
  int a = find(x, k);
  int b = find(y+1-bs, k);
  if (data[a] <= data[b]) {
    return a;
  }
  return b;
}

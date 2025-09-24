#include "rmq.h"

#include<cassert>
#include<bitset>

// class rmq_fast {
// protected:
//   std::vector<int> const &data;
//
// };

// TODO: This idea could be used to save a small amount of space
// class indirector {
// public:
//   std::vector<int> &data;
//   std::vector<int> &indices;
//
//   indirector(std::vector<int> &_data, std::vector<int> &_indices) : data(_data), indices(_indices) {}
//
//   size_t size() { return indices.size(); }
//   int& operator[](int i) { return data[indices[i]]; }
// };



rmq_big::rmq_big(const std::vector<int>& _data) :
    data(_data),
    n(data.size()),
    d(8*sizeof(n) - __builtin_clz(n) - 1),
    m(n*d)
{
  assert(n > 0);
  assert((1<<d) <= n);
  assert(2*(1<<d) >= n);

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

int rmq_big::query(int x, int y) const {
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

rmq_blocked::rmq_blocked(const std::vector<int>& _data) :
    data(_data),
    n(data.size()),
    d((8*sizeof(n) - __builtin_clz(n) - 1)/2),
    bs(d),
    blockmins(data.size()/bs+1, -1),
    blocks(blockmins.size()),
    blocktabs(blocks.size()),
    suffixmins(data.size(), -1),
    prefixmins(data.size(), -1),
    tables(bs*bs*((1<<(bs-1))), -1)
{
  assert(n > 0);
  // compute the minimum within each block and suffixmins for each block
  int mini = 0;
  for (auto i = 1; i < n; i++) {
    if (i % bs == 0) {
      blockmins[i/bs - 1] = mini;
      mini = i;
    }
    if (data[i] < data[mini]) mini = i;
    prefixmins[i] = mini;
  }
  blockmins[n/bs] = mini;

  for (size_t i = 0; i < blocks.size(); i++) {
    if (blockmins[i] < 0) {
      std::cout << "i = " << i << std::endl;
    }
    assert(blockmins[i] >= 0);
    blocks[i] = data[blockmins[i]];
  }
  rmqb = new rmq_big(blocks);

  // compute the suffix minima for each block
  mini = n-1;
  for (int i = n-1; i >= 0; i--) {
    if ((i+1) % bs == 0) mini = i;
    if (data[i] <= data[mini]) mini = i;
    suffixmins[i] = mini;
  }

  // compute lookup tables for each block
  for (int i = 0; i < (int)blocks.size(); i++) {
    std::bitset<8*sizeof(n)> sig;
    for (int j = 0; j < bs-1; j++) {
      if (i*bs + j + 1 >= n || data[i*bs+j] < data[i*bs+j+1]) {
        sig.set(j);
      }
    }
    size_t isig = sig.to_ulong();
    size_t base = isig*bs*bs;  // TODO: bs+1 choose 2 is enough
    blocktabs[i] = base;
    if (tables[base] < 0) {
      // we need to fill in this table.
      for (int x = 0; x < bs; x++) {
        int v = 0;
        int vmin = v;
        int xmin = x;
        tables[base + x*bs + x] = xmin;
        for (int y = x+1; y < bs; y++) {
          v += sig[y-1] ? 1 : -1;
          if (v < vmin) {
            vmin = v;
            xmin = y;
          }
          tables[base + x*bs + y] = xmin;
        }
      }
    }
  }
}

int rmq_blocked::query(int x, int y) const
{
  int bx = x/bs;
  int by = y/bs;
  if (bx == by) {
    // Use lookup table
    int xm = x%bs;
    int ym = y%bs;
    int tmini = tables[blocktabs[bx] + xm*bs + ym];
    tmini += bx*bs;
#ifdef DEBUG
    int mini = x;
    for (int i = x+1; i <= y; i++) {
      if (data[i] < data[mini]) mini = i;
    }
    assert(mini == tmini);
#endif //DEBUG
    return tmini;
  }
  int xx = bx+1;
  int yy = by-1;
  int rangemin = x;
  if (xx <= yy) {
    // x and y have at least one block between them, use rmqb
    int rm = rmqb->query(xx, yy);
    rangemin = blockmins[rm];
  }
  int mina = suffixmins[x];
  int minb = prefixmins[y];
  if (data[rangemin] < data[mina]) {
    if (data[minb] < data[rangemin]) {
      return minb;
    } else {
      return rangemin;
    }
  } else if (data[minb] < data[mina]) {
    return minb;
  } else {
    return mina;
  }
}

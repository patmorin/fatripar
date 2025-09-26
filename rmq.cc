
#include "rmq.h"

#include<cassert>
#include<bitset>


// TODO: This idea could be used to save a small amount of space

/*
 * Return the integer i such that 2^i <= n < 2^{i+1}
 */
static inline int ilog(int n) {
  return 8*sizeof(n) - __builtin_clz(n) - 1;
}

rmq_big::rmq_big(const std::vector<int>& _data) :
    data(_data),
    n(data.size()),
    logn(ilog(n)),
    m(n*(1+logn))
{
  assert(n > 0);
  assert((1<<logn) <= n);
  assert(2*(1<<logn) >= n);

  // dynamic programming fills table row by row
  for (auto i = 0; i < n; i++) {
    lookup(i, 0) = i;
  }
  for (auto k = 1; k < (1+logn); k++) {
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
}

int rmq_big::query(int x, int y) const {
  assert(x <= y);
  if (x == y) return x;
  int r = y - x + 1;
  int k = ilog(r);
  int bs = (1<<k);
  assert(bs <= r);
  assert(2*bs >= r);
  int a = find(x, k);
  int b = find(y+1-bs, k);
  if (data[a] <= data[b]) {
    return a;
  }
  return b;
}

rmq_structure::rmq_structure(const std::vector<int>& _data) :
    data(_data),
    n(data.size()),
    bs(std::max(1, ilog(n)/2)),
    blockminis((data.size()+bs-1)/bs, -1),
    blockmins(blockminis.size()),
    blocktabs(blockmins.size()),
    suffixmins(data.size(), -1),
    prefixmins(data.size(), -1),
    tables(bs*bs*((1<<(bs-1))), -1) {
  assert(n > 0);
  // compute the minimum within each block and suffix minima for each block
  int mini = 0;
  for (auto i = 1; i < n; i++) {
    if (i % bs == 0) {
      blockminis[i/bs - 1] = mini;
      mini = i;
    }
    if (data[i] < data[mini]) mini = i;
    prefixmins[i] = mini;
  }
  blockminis[blockminis.size()-1] = mini;

  // create array of size O(n/log n) for suboptimal structure
  for (size_t i = 0; i < blockmins.size(); i++) {
    assert(blockminis[i] >= 0);
    blockmins[i] = data[blockminis[i]];
  }
  rmqb = new rmq_big(blockmins);

  // compute the suffix minima for each block
  mini = n-1;
  for (int i = n-1; i >= 0; i--) {
    if ((i+1) % bs == 0) mini = i;
    if (data[i] <= data[mini]) mini = i;
    suffixmins[i] = mini;
  }

  // compute lookup tables for each block
  for (int i = 0; i < (int)blockmins.size(); i++) {
    // compute the signature of this block
    std::bitset<8*sizeof(n)> sig;
    for (int j = 0; j < bs-1; j++) {
      // extra cruft here is to pad final partial block with 1s
      assert(i*bs + j + 1 >= n || std::abs(data[i*bs+j]-data[i*bs+j+1])==1);
      if (i*bs + j + 1 >= n || data[i*bs+j] < data[i*bs+j+1]) {
        sig.set(j);
      }
    }
    // check if a table for this signature has already been created
    size_t isig = sig.to_ulong();
    size_t base = isig*bs*bs;  // TODO: (bs+1 choose 2) would be enough
    blocktabs[i] = base;
    if (tables[base] < 0) {
      // we need to fill in this table.
      for (int x = 0; x < bs; x++) {
        int val = 0;
        int min = val;
        int mini = x;
        tables[base + x*bs + x] = mini;
        for (int y = x+1; y < bs; y++) {
          val += sig[y-1] ? 1 : -1;
          if (val < min) {
            min = val;
            mini = y;
          }
          tables[base + x*bs + y] = mini;
        }
      }
    }
  }
}

int rmq_structure::query(int x, int y) const
{
  assert(x <= y);
  int bx = x/bs;  // block that contains x
  int by = y/bs;  // block that contains y
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
    assert(x <= tmini && tmini <= y);
    return tmini;
  }
  // x and y are in different blocks, we need min{blockmins[bx+1,...,by-1]}
  int xx = bx+1;
  int yy = by-1;
  int rangemini = x;
  if (xx <= yy) {
    int rm = rmqb->query(xx, yy);
    assert(xx <= rm && rm <= yy);
    rangemini = blockminis[rm];
  }
  assert(x <= rangemini && rangemini <= y);
  int minxi = suffixmins[x];
  assert(x <= minxi && minxi <= y);
  int minyi = prefixmins[y];
  assert(x <= minyi && minyi <= y);
  // the following logic ensures we return first occurrence of minimum
  if (data[rangemini] < data[minxi]) {
    if (data[minyi] < data[rangemini]) {
      return minyi;
    } else {
      return rangemini;
    }
  } else if (data[minyi] < data[minxi]) {
    return minyi;
  } else {
    return minxi;
  }
}

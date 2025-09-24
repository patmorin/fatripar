#ifndef __TRIANGULATION_H
#define __TRIANGULATION_H

#include<iostream>
#include<cassert>
#include<vector>

struct triangle {
  int vertices[3];
  int neighbours[3];
};

std::ostream& operator<<(std::ostream& os, const triangle& f);


class triangulation {
protected:
  size_t n;  // number of vertices, number faces is 2n-4
  std::vector<triangle> faces;

public:
  triangulation(std::istream& is);

  bool operator==(const triangulation& other) const {
    return this == &other;
  }

  void verify() const;

  size_t nVertices() const {
    return n;
  }

  size_t nFaces() const {
    return 2*n - 4;
  }

  const triangle& operator[](int i) const {
    return faces[i];
  }
};

struct half_edge {
// private:
  int t;  // triangle to the left of this half-edge
  int i;  //

// public:
  half_edge(int _t, int _i) : t(_t), i(_i) { }

  int source(const triangulation& g) const {
    return g[t].vertices[i];
  }

  int target(const triangulation& g) const {
    return g[t].vertices[(i+1)%3];
  }

  int left_face(const triangulation& g) const {
    return t;
  }

  int right_face(const triangulation& g) const {
    return g[t].neighbours[i];
  }

  half_edge reverse(const triangulation& g) const {
    int nt = right_face(g);
    int ns = target(g);
    int ni = 0;
    while (i < 3 && g[nt].vertices[ni] != ns) {
      ni++;
    }
    assert(ni < 3);
    assert(g[nt].vertices[(ni+1)%3] == source(g));
    assert(g[nt].neighbours[ni] == left_face(g));
    return half_edge(nt, ni);
  }

  half_edge next_edge(const triangulation& g) {
    return half_edge(t, (i+2)%3).reverse(g);
  }

  // half_edge& operator=(const half_edge& other) {
  //   assert(g == other.g);
  //   t = other.t;
  //   i = other.i;
  //   return *this;
  // }
};





extern std::ostream& operator<<(std::ostream& os, const triangulation &g);


extern std::ostream& operator<<(std::ostream& os, const half_edge& e);

#endif //__TRIANGULATION_H

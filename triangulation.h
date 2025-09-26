#ifndef __TRIANGULATION_H
#define __TRIANGULATION_H

#include<iostream>
#include<cassert>
#include<vector>

// A face within a triangulation
// This represents the face with vertics vertices[0,1,2]
// in counterclockwise order. neighbours[i] is the triangle
// that shares the edge vertices[i],vertices[(i+1)%3] with this triangle
struct triangle {
  int vertices[3];
  int neighbours[3];
};

extern std::ostream& operator<<(std::ostream& os, const triangle& f);

// A simple triangulation class
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

// This structure represents a directed edge in a triangulation
// An instance of this is always associated with a triangulation, g,
// but we don't store a reference to g explicitly, since this would
// be really wasteful. As a result, the methods for this structure
// always take g as an argument
struct half_edge {
// private:
  int f;  // triangle to the left of this half-edge
  int i;  // f[i] is the source vertex of this half-edge

// public:
  half_edge() : f(-1), i(-1) { }

  half_edge(int _t, int _i) : f(_t), i(_i) { }

  bool is_valid() const { return f >= 0 && i >= 0; }

  int source(const triangulation& g) const { return g[f].vertices[i]; }

  int target(const triangulation& g) const { return g[f].vertices[(i+1)%3]; }

  int left_face(const triangulation& g) const { return f; }

  int right_face(const triangulation& g) const { return g[f].neighbours[i];  }

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

  half_edge next_edge_vertex(const triangulation& g) const {
    return half_edge(f, (i+2)%3).reverse(g);
  }

  half_edge next_edge_face(const triangulation& g) const {
    return half_edge(f, (i+1)%3);
  }

  int opposite_vertex(const triangulation& g) const {
    return g[f].vertices[(i+2)%3];
  }

  bool operator==(const half_edge& other) const {
    return f == other.f && i == other.i;
  }

  bool operator!=(const half_edge& other) const {
    return !(*this == other);
  }
};

extern std::ostream& operator<<(std::ostream& os, const triangulation &g);
extern std::ostream& operator<<(std::ostream& os, const half_edge& e);

#endif //__TRIANGULATION_H

#include "triangulation.h"

// Read a triangulation from an input stream.
// Format: The zeroth line contains the number, n of vertices
//         For 1 <= i <= n, the i'th line contains the three vertices
//         vertices[0,1,2] and adjacencies neighbours[0,1,2] of
//         triangle i-1.
triangulation::triangulation(std::istream& is)
{
  is >> this->n;

  int nf = 2*n - 4;
  faces.resize(nf);
  for (auto i = 0; i < nf; i++) {
    for (auto j = 0; j < 3; j++) {
      is >> this->faces[i].vertices[j];
    }
    for (auto j = 0; j < 3; j++) {
      is >> this->faces[i].neighbours[j];
    }
  }
}


// simple verification that makes sure triangle adjacencies are symmetric and line up across reverse edges.
// Does not check connectivity of dual.
void triangulation::verify() const {
  for (size_t i = 0; i < nFaces(); i++) {
    for (auto j = 0; j < 3; j++) {
      half_edge e(i, j);
      half_edge r(e.reverse(*this));
    }
  }
}

std::ostream& operator<<(std::ostream& os, const triangle& f) {
  os << "(";
  for (auto j = 0; j < 3; j++) {
    os << f.vertices[j] << ((j==2) ? ")" : ",");
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const triangulation &g) {
  os << g.nVertices() << std::endl;
  for (size_t i = 0; i < g.nFaces(); i++) {
    for (auto j = 0; j < 3; j++) {
      os << g[i].vertices[j] << " ";
    }
    for (auto j = 0; j < 3; j++) {
      os << g[i].neighbours[j] << ((j==2) ? "" : " ");
    }
    os << std::endl;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const half_edge& e) {
  return os << "half_edge()";
}

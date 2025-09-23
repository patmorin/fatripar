#include "triangulation.h"

triangulation::triangulation(std::istream& is)
{
  int n;
  is >> n;

  auto f = 2*n - 4;

  this->n = n;
  this->faces = new triangle[f];
  for (auto i = 0; i < f; i++) {
    // std::cout << i << "v ";
    for (auto j = 0; j < 3; j++) {
      is >> this->faces[i].vertices[j];
      // std::cout << this->faces[i].vertices[j]
      //           << ((j==2) ? "\n" : ",");
    }
    // std::cout << i << "t ";
    for (auto j = 0; j < 3; j++) {
      is >> this->faces[i].neighbours[j];
      // std::cout << this->faces[i].neighbours[j]
      //           << ((j==2) ? "\n" : ",");
    }
  }
}


void triangulation::verify() const {
  // simple verification that makes sure triangle adjacencies are symmetric and line up across reverse edges.
  // Does not check connectivity of dual.
  for (size_t i = 0; i < nFaces(); i++) {
    for (auto j = 0; j < 3; j++) {
      half_edge e(i, j);
      // std::cout << e.source() << ", " << e.target() << std::endl;
      half_edge r(e.reverse(*this));
      // std::cout << r.source() << ", " << r.target() << std::endl;
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

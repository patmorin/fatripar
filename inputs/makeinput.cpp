#include "delaunator.hpp"
#include <cstdio>
#include <iostream>
#include <random>
#include <chrono> // For seeding with a high-resolution clock
#include <unordered_map>
#include <cassert>

struct PairHash {
  template <class T1, class T2>
  std::size_t operator () (const std::pair<T1, T2>& p) const {
    auto h1 = std::hash<T1>{}(p.first);
    auto h2 = std::hash<T2>{}(p.second);

    // A common way to combine hashes: XOR and bit shift
    // This helps to mix the bits of the two hashes
    return h1 ^ (h2 << 1);
  }
};

int main(int argc, char **argv) {
  size_t n = 200;
  if (argc == 2) {
    n = std::stoi(argv[1]);
  }

  /* x0, y0, x1, y1, ... */
  std::vector<double> coords = {1, 0, 0, 0, 0, 1};

  std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_real_distribution<double> distribution(.1, .45);

  while(coords.size() < 2*n) {
    coords.push_back(distribution(generator));
    coords.push_back(distribution(generator));
  }

  std::vector<size_t[3]> faces(2*n-4);
  {
    //triangulation happens here
    delaunator::Delaunator d(coords);
    assert(d.triangles.size()/3 == 2*n-5);
    for (size_t i = 0; i < d.triangles.size()/3; i++) {
      for (auto j = 0; j < 3; j++) {
        faces[i][j] = d.triangles[3*i+j];
      }
    }
  }

  // map half edges to adjacent triangles
  std::unordered_map<std::pair<size_t,size_t>,size_t,PairHash> m;
  for (size_t i = 0; i < faces.size()-1; i++) {
    for (auto j = 0; j < 3; j++) {
      m[{faces[i][j],faces[i][(j+1)%3]}] = i;
    }
  }

  // create dual graph
  size_t last = faces.size()-1;
  std::vector<size_t[3]> adjacencies(2*n-4);
  for (size_t i = 0; i < faces.size()-1; i++) {
    for (auto j = 0; j < 3; j++) {
      std::pair<size_t,size_t> revedge {faces[i][(j+1)%3],faces[i][j]};
      if (m.find(revedge) == m.end()) {
        m[revedge] = last;
      }
      adjacencies[i][j] = m[revedge];
    }
  }

  // at this point, the dual graph is missing the outer face
  int missing[3] = {-1, -1, -1};
  for (size_t i = 0; i < faces.size()-1; i++) {
    for (auto j = 0; j < 3; j++) {
      std::pair<size_t,size_t> halfedge {faces[i][j],faces[i][(j+1)%3]};
      std::pair<size_t,size_t> revedge {faces[i][(j+1)%3],faces[i][j]};
      if (m[revedge] == last) {
        if (missing[0] == -1) {
          missing[0] = revedge.first;
          missing[1] = revedge.second;
          adjacencies[last][0] = m[halfedge];
        } else if ((int)revedge.first == missing[1]) {
          missing[2] = revedge.second;
          adjacencies[last][1] = m[halfedge];
        } else {
          missing[2] = revedge.first;
          adjacencies[last][2] = m[halfedge];
        }
      }
    }
  }
  // faces.resize(faces.size()+1);
  for (auto j = 0; j < 3; j++) {
    faces[last][j] = missing[j];
  }

  std::cout << n << std::endl;
  for (size_t i = 0; i < faces.size(); i++) {
    for (auto j = 0; j < 3; j++) {
      std::cout << faces[i][j] << " ";
    }
    for (auto j = 0; j < 3; j++) {
      std::cout << adjacencies[i][j] << ((j == 2) ? "" : " ");
    }
    std::cout << std::endl;
  }
  return 0;
}

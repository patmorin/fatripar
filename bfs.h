#ifndef __BFS_H
#define __BFS_H

#include<vector>
#include "triangulation.h"


void bfs(const triangulation &g, const half_edge e0,
                std::vector<half_edge> &t,
                std::vector<int[3]> &bt) ;

#endif // __BFS_H

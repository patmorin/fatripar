#ifndef __BFS_H
#define __BFS_H

#include<vector>

void triple_bfs(const triangulation &g, int f0,
                std::vector<triangulation::half_edge> &t);

#endif // __BFS_H

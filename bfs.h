#ifndef __BFS_H
#define __BFS_H

#include<vector>
#include "triangulation.h"

/* compute BFS tree t rooted at e0.source() and cotree bt rooted
   at e0.left_face()
*/
// TODO: Split this into two functions, one for the tree and one for the cotree
void bfs_tree(const triangulation &g, const half_edge e0,
                std::vector<half_edge> &t) ;

#endif // __BFS_H

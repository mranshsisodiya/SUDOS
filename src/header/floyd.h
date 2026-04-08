#ifndef FLOYD_H
#define FLOYD_H

#include "graph.h"

/*
 * Floyd-Warshall all-pairs shortest paths.
 *
 * dist[i][j]  -> shortest distance from i to j  (INF if unreachable)
 * next[i][j]  -> first hop from i toward j      (-1 if unreachable)
 *
 * Complexity: O(V^3)
 */
void floyd_warshall(const Graph *g,
                    int dist[MAX_NODES][MAX_NODES],
                    int next[MAX_NODES][MAX_NODES]);

/* Reconstruct path from i to j using next[][] */
void floyd_print_path(const Graph *g,
                      int next[MAX_NODES][MAX_NODES],
                      int i, int j);

/* Print the full distance matrix */
void floyd_print_matrix(const Graph *g,
                         int dist[MAX_NODES][MAX_NODES]);

#endif /* FLOYD_H */

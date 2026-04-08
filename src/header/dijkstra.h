#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"

/*
 * Dijkstra's single-source shortest path.
 *
 * dist[i]  -> shortest distance from 'src' to node i  (INF if unreachable)
 * prev[i]  -> predecessor of i on shortest path       (-1 if none)
 *
 * Complexity: O(V^2)  — suitable for dense academic graphs
 */
void dijkstra(const Graph *g, int src,
              int dist[MAX_NODES], int prev[MAX_NODES]);

/* Print the shortest path from src to dest using prev[] */
void dijkstra_print_path(const Graph *g, const int prev[], int src, int dest);

/* Print the full distance table from a single source */
void dijkstra_print_table(const Graph *g, int src,
                           const int dist[], const int prev[]);

#endif /* DIJKSTRA_H */

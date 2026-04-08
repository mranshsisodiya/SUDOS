#ifndef TSP_GREEDY_H
#define TSP_GREEDY_H

#include "graph.h"

#define MAX_DELIVERY_NODES 20

/*
 * Nearest-Neighbour Greedy TSP heuristic.
 *
 * Operates on a precomputed all-pairs distance matrix (from Floyd/Dijkstra).
 * Finds a Hamiltonian path visiting all delivery nodes starting from 'start'.
 *
 * nodes[]    - array of delivery node indices (length num_nodes)
 * num_nodes  - number of delivery stops (including start)
 * start      - index into nodes[] for the depot/starting point
 * tour[]     - OUTPUT: visit order as indices into nodes[] (length num_nodes+1,
 *              last element == first for closed tour)
 * returns    - total tour cost  (-1 on error)
 *
 * Complexity: O(N^2)
 */
int tsp_greedy(int dist_matrix[MAX_NODES][MAX_NODES],
               const int nodes[], int num_nodes,
               int start_node,
               int tour[], int *tour_len);

void tsp_greedy_print(const Graph *g,
                      const int tour[], int tour_len,
                      int total_cost);

#endif /* TSP_GREEDY_H */

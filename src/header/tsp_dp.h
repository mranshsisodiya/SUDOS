#ifndef TSP_DP_H
#define TSP_DP_H

#include "graph.h"

/*
 * Held-Karp Bitmask Dynamic Programming for exact TSP.
 *
 * dp[mask][i] = minimum cost to visit exactly the nodes encoded in 'mask',
 *               ending at node i.
 *
 * nodes[]    - array of delivery node indices (length num_nodes)
 * num_nodes  - number of delivery stops  (max MAX_DP_NODES due to 2^N states)
 * start_node - depot node (must be in nodes[])
 * tour[]     - OUTPUT: optimal visit order (graph node indices)
 * tour_len   - OUTPUT: length of tour array
 * returns    - optimal tour cost  (-1 on error / unreachable)
 *
 * Complexity: O(2^N * N^2) — practical for N ≤ 20
 */

#define MAX_DP_NODES 20

int tsp_dp(int dist_matrix[MAX_NODES][MAX_NODES],
           const int nodes[], int num_nodes,
           int start_node,
           int tour[], int *tour_len);

void tsp_dp_print(const Graph *g,
                  const int tour[], int tour_len,
                  int total_cost);

#endif /* TSP_DP_H */

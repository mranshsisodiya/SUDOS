#include "tsp_dp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
int tsp_dp(int dist_matrix[MAX_NODES][MAX_NODES],
           const int nodes[], int num_nodes,
           int start_node,
           int tour[], int *tour_len) {

    if (num_nodes <= 0 || num_nodes > MAX_DP_NODES) return -1;

    /* Find start index inside nodes[] */
    int start_idx = 0;
    for (int i = 0; i < num_nodes; i++)
        if (nodes[i] == start_node) { start_idx = i; break; }

    int N    = num_nodes;
    int FULL = (1 << N) - 1;

    /* Allocate dp and parent tables dynamically (2^N * N) */
    int (*dp)[MAX_DP_NODES]     = calloc(1 << N, sizeof(*dp));
    int (*parent)[MAX_DP_NODES] = calloc(1 << N, sizeof(*parent));
    if (!dp || !parent) { free(dp); free(parent); return -1; }

    /* Initialise */
    for (int mask = 0; mask <= FULL; mask++)
        for (int i = 0; i < N; i++) {
            dp[mask][i]     = INF;
            parent[mask][i] = -1;
        }

    /* Base case: start at start_idx, visited only start */
    dp[1 << start_idx][start_idx] = 0;

    /* Fill DP */
    for (int mask = 0; mask <= FULL; mask++) {
        for (int u = 0; u < N; u++) {
            if (!(mask & (1 << u))) continue;         /* u not in mask */
            if (dp[mask][u] == INF)  continue;

            for (int v = 0; v < N; v++) {
                if (mask & (1 << v)) continue;        /* v already visited */
                int d = dist_matrix[nodes[u]][nodes[v]];
                if (d == INF) continue;

                int new_mask = mask | (1 << v);
                int new_cost = dp[mask][u] + d;
                if (new_cost < dp[new_mask][v]) {
                    dp[new_mask][v]     = new_cost;
                    parent[new_mask][v] = u;
                }
            }
        }
    }

    /* Find best last node to return to start */
    int best_cost = INF;
    int last      = -1;
    for (int u = 0; u < N; u++) {
        if (u == start_idx) continue;
        if (dp[FULL][u] == INF) continue;
        int ret = dist_matrix[nodes[u]][nodes[start_idx]];
        if (ret == INF) continue;
        int total = dp[FULL][u] + ret;
        if (total < best_cost) {
            best_cost = total;
            last      = u;
        }
    }

    /* Handle trivial single-node case */
    if (N == 1) {
        tour[0]   = nodes[start_idx];
        *tour_len = 1;
        free(dp); free(parent);
        return 0;
    }

    if (last == -1) { free(dp); free(parent); return -1; }

    /* Reconstruct path */
    int path[MAX_DP_NODES];
    int path_len = 0;
    int mask     = FULL;
    int cur      = last;
    while (cur != -1) {
        path[path_len++] = cur;
        int prev = parent[mask][cur];
        mask    ^= (1 << cur);
        cur      = prev;
    }

    /* Reverse path into tour */
    *tour_len = 0;
    for (int i = path_len - 1; i >= 0; i--)
        tour[(*tour_len)++] = nodes[path[i]];
    /* append return to start */
    tour[(*tour_len)++] = nodes[start_idx];

    free(dp);
    free(parent);
    return best_cost;
}

/* ------------------------------------------------------------------ */
void tsp_dp_print(const Graph *g,
                  const int tour[], int tour_len,
                  int total_cost) {
    printf("\n  [DP Optimal] Route: ");
    for (int i = 0; i < tour_len; i++) {
        printf("%s", g->node_names[tour[i]]);
        if (i < tour_len - 1) printf(" -> ");
    }
    printf("\n  [DP Optimal] Total Cost: %d\n", total_cost);
}

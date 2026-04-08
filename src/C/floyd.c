#include "floyd.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
void floyd_warshall(const Graph *g,
                    int dist[MAX_NODES][MAX_NODES],
                    int next[MAX_NODES][MAX_NODES]) {
    int n = g->num_nodes;

    /* Initialise from adjacency matrix */
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            dist[i][j] = (i == j) ? 0 : INF;
            next[i][j] = (i == j) ? i : -1;
        }

    for (int i = 0; i < n; i++)
        for (Edge *e = g->adj[i]; e; e = e->next)
            if (e->weight < dist[i][e->dest]) {
                dist[i][e->dest] = e->weight;
                next[i][e->dest] = e->dest;
            }

    /* Relax through intermediate nodes */
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                if (dist[i][k] == INF || dist[k][j] == INF) continue;
                long long nd = (long long)dist[i][k] + dist[k][j];
                if (nd < dist[i][j]) {
                    dist[i][j] = (int)nd;
                    next[i][j] = next[i][k];
                }
            }
}

/* ------------------------------------------------------------------ */
void floyd_print_path(const Graph *g,
                      int next[MAX_NODES][MAX_NODES],
                      int i, int j) {
    if (next[i][j] == -1) {
        printf("  No path from %s to %s\n",
               g->node_names[i], g->node_names[j]);
        return;
    }
    printf("  %s", g->node_names[i]);
    int cur = i;
    while (cur != j) {
        cur = next[cur][j];
        printf(" -> %s", g->node_names[cur]);
    }
    printf("\n");
}

/* ------------------------------------------------------------------ */
void floyd_print_matrix(const Graph *g,
                         int dist[MAX_NODES][MAX_NODES]) {
    int n = g->num_nodes;
    if (n > 20) {
        printf("\n--- Floyd-Warshall: %d x %d matrix computed (too large to display fully) ---\n", n, n);
        printf("  Sample distances from node 0 (%s):\n", g->node_names[0]);
        printf("  %-24s  %8s\n", "Destination", "Dist(km)");
        printf("  %-24s  %8s\n", "------------------------", "--------");
        for (int j = 1; j < n && j <= 10; j++) {
            if (dist[0][j] == INF)
                printf("  %-24s  %8s\n", g->node_names[j], "INF");
            else
                printf("  %-24s  %8d\n", g->node_names[j], dist[0][j]);
        }
        printf("  ... (use interactive query to check any pair)\n");
        return;
    }
    printf("\n--- Floyd-Warshall Distance Matrix ---\n");
    printf("  %12s", "");
    for (int j = 0; j < n; j++)
        printf("  %6s", g->node_names[j]);
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("  %12s", g->node_names[i]);
        for (int j = 0; j < n; j++) {
            if (dist[i][j] == INF)
                printf("  %6s", "INF");
            else
                printf("  %6d", dist[i][j]);
        }
        printf("\n");
    }
}

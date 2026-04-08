#include "dijkstra.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
void dijkstra(const Graph *g, int src,
              int dist[MAX_NODES], int prev[MAX_NODES]) {
    int n = g->num_nodes;
    int visited[MAX_NODES] = {0};

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[src] = 0;

    for (int iter = 0; iter < n; iter++) {
        /* pick unvisited node with minimum dist */
        int u = -1;
        for (int i = 0; i < n; i++)
            if (!visited[i] && (u == -1 || dist[i] < dist[u]))
                u = i;

        if (u == -1 || dist[u] == INF) break; /* remaining unreachable */
        visited[u] = 1;

        /* relax neighbours */
        for (Edge *e = g->adj[u]; e; e = e->next) {
            int v = e->dest;
            if (!visited[v] && dist[u] != INF &&
                dist[u] + e->weight < dist[v]) {
                dist[v] = dist[u] + e->weight;
                prev[v] = u;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* Recursive helper */
static void print_path_rec(const Graph *g, const int prev[], int node) {
    if (prev[node] == -1) {
        printf("%s", g->node_names[node]);
        return;
    }
    print_path_rec(g, prev, prev[node]);
    printf(" -> %s", g->node_names[node]);
}

void dijkstra_print_path(const Graph *g, const int prev[], int src, int dest) {
    if (prev[dest] == -1 && dest != src) {
        printf("  No path from %s to %s\n",
               g->node_names[src], g->node_names[dest]);
        return;
    }
    printf("  Path: ");
    print_path_rec(g, prev, dest);
    printf("\n");
}

/* ------------------------------------------------------------------ */
void dijkstra_print_table(const Graph *g, int src,
                           const int dist[], const int prev[]) {
    int n = g->num_nodes;
    printf("\n--- Dijkstra from [%s] ---\n", g->node_names[src]);
    printf("  %-24s  %8s  %s\n", "Destination", "Dist(km)", "Shortest Path");
    printf("  %-24s  %8s  %s\n",
           "------------------------", "--------", "--------------------");
    for (int i = 0; i < n; i++) {
        if (i == src) continue;
        if (dist[i] == INF)
            printf("  %-24s  %8s\n", g->node_names[i], "INF");
        else {
            printf("  %-24s  %8d  ", g->node_names[i], dist[i]);
            print_path_rec(g, prev, i);
            printf("\n");
        }
    }
}

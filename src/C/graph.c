#include "graph.h"

/* ------------------------------------------------------------------ */
Graph *graph_create(int num_nodes) {
    if (num_nodes <= 0 || num_nodes > MAX_NODES) {
        fprintf(stderr, "[graph] num_nodes must be 1..%d\n", MAX_NODES);
        return NULL;
    }
    Graph *g = (Graph *)calloc(1, sizeof(Graph));
    if (!g) { perror("calloc"); return NULL; }
    g->num_nodes = num_nodes;
    g->num_edges = 0;
    /* default names: "0", "1", ... */
    for (int i = 0; i < num_nodes; i++)
        snprintf(g->node_names[i], 32, "%d", i);
    return g;
}

/* ------------------------------------------------------------------ */
void graph_destroy(Graph *g) {
    if (!g) return;
    for (int i = 0; i < g->num_nodes; i++) {
        Edge *e = g->adj[i];
        while (e) {
            Edge *tmp = e;
            e = e->next;
            free(tmp);
        }
    }
    free(g);
}

/* ------------------------------------------------------------------ */
void graph_set_name(Graph *g, int node, const char *name) {
    if (!g || node < 0 || node >= g->num_nodes) return;
    strncpy(g->node_names[node], name, 31);
    g->node_names[node][31] = '\0';
}

/* ------------------------------------------------------------------ */
void graph_add_edge(Graph *g, int src, int dest, int weight) {
    if (!g || src < 0 || src >= g->num_nodes ||
        dest < 0 || dest >= g->num_nodes || weight < 0) return;

    Edge *e = (Edge *)malloc(sizeof(Edge));
    if (!e) { perror("malloc"); return; }
    e->dest   = dest;
    e->weight = weight;
    e->next   = g->adj[src];
    g->adj[src] = e;
    g->num_edges++;
}

/* ------------------------------------------------------------------ */
void graph_add_undirected_edge(Graph *g, int src, int dest, int weight) {
    graph_add_edge(g, src, dest, weight);
    graph_add_edge(g, dest, src, weight);
}

/* ------------------------------------------------------------------ */
void graph_print(const Graph *g) {
    if (!g) return;
    printf("\n=== City Graph (%d nodes, %d directed edges) ===\n",
           g->num_nodes, g->num_edges);
    for (int i = 0; i < g->num_nodes; i++) {
        printf("  [%s]", g->node_names[i]);
        for (Edge *e = g->adj[i]; e; e = e->next)
            printf(" -> %s(%d)", g->node_names[e->dest], e->weight);
        printf("\n");
    }
}

/* ------------------------------------------------------------------ */
void graph_to_matrix(const Graph *g, int mat[MAX_NODES][MAX_NODES]) {
    int n = g->num_nodes;
    /* initialise */
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[i][j] = (i == j) ? 0 : INF;

    for (int i = 0; i < n; i++)
        for (Edge *e = g->adj[i]; e; e = e->next)
            if (e->weight < mat[i][e->dest])
                mat[i][e->dest] = e->weight;
}

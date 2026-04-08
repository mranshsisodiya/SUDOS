#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_NODES    250
#define MAX_EDGES    2000
#define INF          INT_MAX

/* ---------- Edge (adjacency list node) ---------- */
typedef struct Edge {
    int   dest;          /* destination node index   */
    int   weight;        /* edge weight (distance)   */
    struct Edge *next;   /* next edge in list        */
} Edge;

/* ---------- Graph ---------- */
typedef struct {
    int    num_nodes;
    int    num_edges;
    char   node_names[MAX_NODES][32]; /* human-readable labels */
    Edge  *adj[MAX_NODES];            /* adjacency list heads  */
} Graph;

/* --- API --- */
Graph *graph_create(int num_nodes);
void   graph_destroy(Graph *g);
void   graph_set_name(Graph *g, int node, const char *name);
void   graph_add_edge(Graph *g, int src, int dest, int weight);
void   graph_add_undirected_edge(Graph *g, int src, int dest, int weight);
void   graph_print(const Graph *g);

/* Build a weight matrix from the graph (INF where no direct edge) */
void   graph_to_matrix(const Graph *g, int mat[MAX_NODES][MAX_NODES]);

#endif /* GRAPH_H */

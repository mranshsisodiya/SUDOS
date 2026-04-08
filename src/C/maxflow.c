#include "maxflow.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Internal flow network */

static int cap[MF_MAX_NODES][MF_MAX_NODES];  /* residual capacity */
static int flow_val[MF_MAX_NODES][MF_MAX_NODES]; /* actual flow */
static int parent_mf[MF_MAX_NODES];
static int visited_mf[MF_MAX_NODES];
static int queue_mf[MF_MAX_NODES];

/* BFS to find augmenting path — returns 1 if path found */
static int bfs(int s, int t, int total_nodes) {
    memset(visited_mf, 0, sizeof(visited_mf));
    memset(parent_mf,  -1, sizeof(parent_mf));
    int head = 0, tail = 0;
    queue_mf[tail++] = s;
    visited_mf[s]    = 1;

    while (head < tail) {
        int u = queue_mf[head++];
        for (int v = 0; v < total_nodes; v++) {
            if (!visited_mf[v] && cap[u][v] > 0) {
                parent_mf[v]  = u;
                visited_mf[v] = 1;
                if (v == t) return 1;
                queue_mf[tail++] = v;
            }
        }
    }
    return 0;
}

/* ------------------------------------------------------------------ */
int max_flow_assign(int num_agents, int num_clusters,
                    const int agent_capacity[],
                    const int cluster_demand[],
                    const int affinity[MAX_AGENTS][MAX_CLUSTERS],
                    int assignment[MAX_AGENTS][MAX_CLUSTERS]) {

    /* Node layout:
     *   0            : source
     *   1..A         : agents
     *   A+1..A+C     : clusters
     *   A+C+1        : sink
     */
    int A = num_agents, C = num_clusters;
    int S = 0, T = A + C + 1;
    int total = T + 1;

    memset(cap,       0, sizeof(cap));
    memset(flow_val,  0, sizeof(flow_val));
    memset(assignment, 0, sizeof(int) * MAX_AGENTS * MAX_CLUSTERS);

    /* Source -> agents */
    for (int a = 0; a < A; a++)
        cap[S][a + 1] = agent_capacity[a];

    /* agents -> clusters (affinity) */
    for (int a = 0; a < A; a++)
        for (int c = 0; c < C; c++)
            if (affinity[a][c])
                cap[a + 1][A + 1 + c] = agent_capacity[a]; /* bounded by agent cap */

    /* clusters -> sink */
    for (int c = 0; c < C; c++)
        cap[A + 1 + c][T] = cluster_demand[c];

    /* Edmonds-Karp: augment while path exists */
    int total_flow = 0;
    while (bfs(S, T, total)) {
        /* Find bottleneck along path */
        int path_flow = 1 << 30;
        for (int v = T; v != S; v = parent_mf[v]) {
            int u = parent_mf[v];
            if (cap[u][v] < path_flow) path_flow = cap[u][v];
        }
        /* Update residual capacities */
        for (int v = T; v != S; v = parent_mf[v]) {
            int u = parent_mf[v];
            cap[u][v]     -= path_flow;
            cap[v][u]     += path_flow;
            flow_val[u][v] += path_flow;
            flow_val[v][u] -= path_flow;
        }
        total_flow += path_flow;
    }

    /* Extract assignment: flow on agent->cluster edges */
    for (int a = 0; a < A; a++)
        for (int c = 0; c < C; c++)
            if (affinity[a][c] && flow_val[a + 1][A + 1 + c] > 0)
                assignment[a][c] = flow_val[a + 1][A + 1 + c];

    return total_flow;
}

/* ------------------------------------------------------------------ */
void maxflow_print(int num_agents, int num_clusters,
                   char agent_names[][32],
                   char cluster_names[][32],
                   int assignment[MAX_AGENTS][MAX_CLUSTERS],
                   int total_flow) {
    printf("\n--- Agent Assignment (Max-Flow / Ford-Fulkerson) ---\n");
    printf("  Total deliveries assigned: %d\n\n", total_flow);
    for (int a = 0; a < num_agents; a++) {
        printf("  Agent %-15s covers: ", agent_names[a]);
        int any = 0;
        for (int c = 0; c < num_clusters; c++)
            if (assignment[a][c]) {
                printf("%s(%d) ", cluster_names[c], assignment[a][c]);
                any = 1;
            }
        if (!any) printf("(none)");
        printf("\n");
    }
}

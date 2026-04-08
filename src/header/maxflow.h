#ifndef MAXFLOW_H
#define MAXFLOW_H

/*
 * Edmonds-Karp implementation of Ford-Fulkerson Max Flow.
 *
 * Used for agent-to-delivery-cluster assignment:
 *   source (S) -> agents     -> delivery clusters -> sink (T)
 *
 * Network layout (node indices):
 *   0          : source (S)
 *   1..A       : agent nodes
 *   A+1..A+C   : cluster nodes
 *   A+C+1      : sink (T)
 *
 * Parameters:
 *   num_agents        - number of delivery agents
 *   num_clusters      - number of delivery clusters
 *   agent_capacity[]  - max deliveries each agent can handle
 *   cluster_demand[]  - deliveries needed by each cluster
 *   affinity[][]      - affinity[a][c] = 1 if agent a can serve cluster c
 *   assignment[][]    - OUTPUT: assignment[a][c] = flow assigned
 *   returns           - total max flow (total deliveries assigned)
 *
 * Complexity: O(V * E^2)  (Edmonds-Karp BFS augmentation)
 */

#define MAX_AGENTS   10
#define MAX_CLUSTERS 10
#define MF_MAX_NODES (MAX_AGENTS + MAX_CLUSTERS + 2)

int max_flow_assign(int num_agents, int num_clusters,
                    const int agent_capacity[],
                    const int cluster_demand[],
                    const int affinity[MAX_AGENTS][MAX_CLUSTERS],
                    int assignment[MAX_AGENTS][MAX_CLUSTERS]);

void maxflow_print(int num_agents, int num_clusters,
                   char agent_names[][32],
                   char cluster_names[][32],
                   int assignment[MAX_AGENTS][MAX_CLUSTERS],
                   int total_flow);

#endif /* MAXFLOW_H */

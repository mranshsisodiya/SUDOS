# SUDOS — Smart Urban Delivery Optimization System
**Design and Analysis of Algorithms Project · VIT Vellore**

---

## Project Overview

SUDOS models a real-world urban delivery platform (like Amazon, Swiggy, or Blinkit) using
a city represented as a weighted graph and applies five distinct algorithmic paradigms to
compute optimal delivery assignments and routes.

---

## Algorithms Implemented

| Module          | Algorithm                    | Paradigm         | Complexity  |
|-----------------|------------------------------|------------------|-------------|
| `dijkstra.c`    | Dijkstra's Algorithm         | Greedy/Graph     | O(V²)       |
| `floyd.c`       | Floyd-Warshall               | Dynamic Programming | O(V³)    |
| `tsp_greedy.c`  | Nearest-Neighbour Heuristic  | Greedy TSP       | O(N²)       |
| `tsp_dp.c`      | Held-Karp Bitmask DP         | Dynamic Programming | O(2ᴺ·N²)|
| `maxflow.c`     | Edmonds-Karp (Ford-Fulkerson)| Network Flow     | O(V·E²)     |

---

## System Workflow

```
1. Build city graph (adjacency list, weighted undirected)
        ↓
2. Dijkstra — shortest paths from Depot (single source)
        ↓
3. Floyd-Warshall — all-pairs shortest distances (used by TSP)
        ↓
4. Max Flow — assign agents to delivery clusters (Ford-Fulkerson)
        ↓
5. For each agent's cluster:
     ├── Greedy NN TSP  → fast approximate route
     └── Held-Karp DP   → optimal route (N ≤ 20)
        ↓
6. Metrics comparison — time + cost across all algorithms
```

---

## City Map (10 Nodes, 20 Undirected Edges)

```
Depot ─4─ Zone-A ─5─ Zone-D ─1─ Zone-E
  │          │                     │  \
  3        2,6                   4,5,6  \
  │          │                     │    \
Zone-B ──── Zone-C      Zone-F ─3─ Zone-G
       \              /    \         |
        8            5      5       4
         \          /        \      |
          Zone-E──            Zone-I
              \
               Zone-H ─2─ Zone-I
```

---

## Build & Run

```bash
# Compile (requires GCC, C11)
make

# Run simulation
make run

# or directly
./sudos

# Clean build artifacts
make clean

# Memory check (requires valgrind)
make valgrind
```

---

## Module Descriptions

### `graph.h / graph.c`
- Adjacency list representation using linked-list edges
- Supports directed and undirected weighted edges
- `graph_to_matrix()` converts to weight matrix for TSP algorithms

### `dijkstra.h / dijkstra.c`
- O(V²) single-source shortest path using linear scan
- Returns `dist[]` and `prev[]` for path reconstruction
- Prints formatted distance table with full path traces

### `floyd.h / floyd.c`
- O(V³) all-pairs shortest path using 3-nested loops
- Returns `dist[][]` and `next[][]` for path reconstruction
- Output: complete N×N distance matrix

### `tsp_greedy.h / tsp_greedy.c`
- Nearest-Neighbour heuristic for TSP
- Starts at depot, always visits closest unvisited node
- Uses Floyd-Warshall precomputed distances
- Closed tour (returns to depot)

### `tsp_dp.h / tsp_dp.c`
- Held-Karp exact TSP via bitmask DP
- `dp[mask][i]` = min cost to visit nodes in `mask`, ending at `i`
- Path reconstruction via `parent[mask][i]` table
- Practical for N ≤ 20 nodes per agent cluster

### `maxflow.h / maxflow.c`
- Edmonds-Karp BFS-augmented Ford-Fulkerson
- Network: Source → Agents → Clusters → Sink
- Respects agent capacity and cluster demand constraints
- Returns flow assignment matrix `assignment[agent][cluster]`

### `metrics.h / metrics.c`
- `clock()`-based microsecond timing
- Formatted comparison table with cost and time per algorithm
- Computes greedy overhead vs optimal as a percentage

---

## Sample Output

```
PHASE 2A: Dijkstra (from Depot)
  Zone-A    dist=4   Depot → Zone-A
  Zone-B    dist=3   Depot → Zone-B
  Zone-G    dist=13  Depot → Zone-B → Zone-E → Zone-F → Zone-G

PHASE 3: Agent Assignment (Max Flow)
  Total deliveries assigned: 7
  Agent Ravi   → North(2), East(1)
  Agent Priya  → East(2), South(2)

PHASE 4: Routes
  [Greedy NN] Ravi/North:  Depot→Zone-B→Zone-A→Zone-C→Depot  Cost=18
  [DP Optimal] Ravi/North: Depot→Zone-C→Zone-B→Zone-A→Depot  Cost=17
  → DP saves 5.6% over Greedy

  [Greedy NN] Priya/East:  Depot→Zone-E→Zone-D→Zone-F→Zone-G→Depot Cost=28
  [DP Optimal] Priya/East: Depot→Zone-G→Zone-F→Zone-E→Zone-D→Depot Cost=28
  → Greedy matched Optimal
```

---

## Files

```
SUDOS/
├── main.c              ← simulation entry point
├── graph.h / graph.c   ← city graph (adjacency list)
├── dijkstra.h/.c       ← Dijkstra SSSP
├── floyd.h/.c          ← Floyd-Warshall APSP
├── tsp_greedy.h/.c     ← Nearest-Neighbour heuristic
├── tsp_dp.h/.c         ← Held-Karp bitmask DP
├── maxflow.h/.c        ← Edmonds-Karp max flow
├── metrics.h/.c        ← timing & comparison
├── Makefile
├── sudos_dashboard.html ← interactive visualization
└── README.md
```

---

## Real-World Connection

| SUDOS Component         | Industry Equivalent                    |
|-------------------------|----------------------------------------|
| City graph              | Road network graph (Google Maps)       |
| Dijkstra/Floyd          | Route precomputation cache             |
| Greedy NN TSP           | Real-time route dispatch (Swiggy)      |
| Held-Karp DP            | Batch-optimal routing (Amazon)         |
| Max Flow assignment     | Driver-zone allocation engine          |

---

*SUDOS implements the same algorithmic principles used in production logistics at academic scale, demonstrating strong understanding of graph algorithms, DP, greedy heuristics, and network flow.*

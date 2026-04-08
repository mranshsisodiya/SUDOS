# SUDOS -- Smart Urban Delivery Optimization System
**Design and Analysis of Algorithms Project | VIT Vellore**
**City: Delhi, Noida & NCR | 200 Real Locations | 5 Algorithms**

---

## Project Overview

SUDOS models a real-world urban delivery platform (like Amazon, Swiggy, or Blinkit) using
a weighted graph of 200 real locations across Delhi, Noida, and NCR. It applies five
distinct algorithmic paradigms to answer three core logistics questions:

1. What is the shortest road between any two locations?
2. Which delivery agent should handle which zone?
3. For each agent, what is the most efficient order to visit their stops?

The system is fully interactive -- a delivery agent can type in their stops and
instantly receive both a fast greedy route and a mathematically optimal route,
with a comparison showing how much distance the optimal route saves.

---

## What Changed Recently

### v2.0 Changes (current version)

| Change | Detail |
|--------|--------|
| City scale | Expanded from 10 dummy nodes to 200 real Delhi/Noida locations |
| City names | All zones replaced with real NCR locations (see zones below) |
| Road network | Expanded from 20 to 427 undirected roads (854 directed edges) |
| Agent input | Delivery agents now enter their own stops interactively |
| Terminal fix | All Unicode box-drawing characters replaced with plain ASCII |
| Menu system | Full 5-feature interactive menu replacing single auto-run demo |
| Memory fix | fw_dist/fw_next moved to static globals to prevent stack overflow |
| Floyd output | Matrix display skipped for large graphs; shows sample + query mode |

---

## City Zones (200 Locations)

```
Zone             Node Range    Example Locations
--------------   ----------    -------------------------------------------------
Central Delhi      0 - 19     Connaught Place, India Gate, ITO, Chandni Chowk
North Delhi       20 - 39     Rohini, Pitampura, Shalimar Bagh, Narela, Bawana
West Delhi        40 - 59     Janakpuri, Dwarka, Uttam Nagar, Palam, Najafgarh
South Delhi       60 - 79     Saket, Hauz Khas, Lajpat Nagar, Qutub Minar
East Delhi        80 - 99     Laxmi Nagar, Mayur Vihar, Dilshad Garden, Anand Vihar
South-East Delhi 100 - 109    Jasola, Sarita Vihar, Kalindi Kunj, Nizamuddin
Noida            110 - 149    Sector 1 through Sector 57
Greater Noida    150 - 179    Alpha, Beta, Gamma, Knowledge Park, Pari Chowk, Jewar
NCR Fringe       180 - 199    Ghaziabad, Faridabad, Gurugram, Bahadurgarh, Sonipat
```

---

## Road Network

- 427 undirected edges, 854 directed edges in adjacency list
- Weights represent approximate road distance in kilometres
- Covers internal zone roads + all major arterials:
  - Ring Road (inner and outer)
  - DND Flyway (East Delhi <-> Noida)
  - NH-24 (East Delhi <-> Ghaziabad)
  - NH-8 / NH-48 (South Delhi <-> Gurugram)
  - Noida-Greater Noida Expressway
  - Yamuna Expressway connections

---

## Algorithms Implemented

| Module           | Algorithm                     | Paradigm            | Complexity   |
|------------------|-------------------------------|---------------------|--------------|
| `dijkstra.c`     | Dijkstra's Algorithm          | Greedy / Graph      | O(V^2)       |
| `floyd.c`        | Floyd-Warshall                | Dynamic Programming | O(V^3)       |
| `tsp_greedy.c`   | Nearest-Neighbour Heuristic   | Greedy TSP          | O(N^2)       |
| `tsp_dp.c`       | Held-Karp Bitmask DP          | Dynamic Programming | O(2^N * N^2) |
| `maxflow.c`      | Edmonds-Karp (Ford-Fulkerson) | Network Flow        | O(V * E^2)   |

---

## Where the Data Is Stored

This is important for understanding the memory architecture of the project.

### City names and road distances
Stored as `static const` arrays at the top of `main.c`, compiled into the
program's read-only data segment. They are not read from any file or database.

```c
// main.c
static const char *CITY_NAMES[200] = { "Connaught_Place", "India_Gate", ... };
static const int   ROADS[][3]      = { {0,1,3}, {0,2,2}, ... };  // 427 entries
```

### Adjacency list (the live graph structure)
Built at runtime inside the `Graph` struct in heap memory.
When `main()` calls `graph_add_undirected_edge()` for each road,
it `malloc()`s an `Edge` node and prepends it to `adj[src]`.

```
ROADS[][] (read-only data)
    |
    v  graph_add_undirected_edge() x 427
    v
city->adj[0] -> Edge -> Edge -> NULL    (heap, linked list)
city->adj[1] -> Edge -> NULL
...
city->adj[199] -> Edge -> Edge -> Edge -> NULL
```

### Floyd-Warshall distance matrix
Two 250x250 int arrays (~500 KB total). Declared as static globals in `main.c`
to avoid stack overflow (the default stack is only 1-8 MB).

```c
// main.c  -- above main(), NOT inside it
static int fw_dist[MAX_NODES][MAX_NODES];
static int fw_next[MAX_NODES][MAX_NODES];
static int fw_ready = 0;   // computed once, then cached for the session
```

Computed lazily -- only when the agent first uses the route planner or demo.
Once computed it is reused for the entire session.

### Demo agent data
Stored as `static const` local arrays inside `feature_full_demo()` in `main.c`.

```c
static const char *AGENT_NAMES[3]  = {"Ramesh", "Sunita", "Arjun"};
static const int   AGENT_CAP[3]    = {4, 5, 4};
static const int   AFFINITY[10][10] = { {1,1,0}, {1,1,1}, {0,1,1} };
```

### Memory cleanup
`graph_destroy()` walks every linked list and calls `free()` on every Edge node,
then frees the Graph struct. No memory leaks. Verified with valgrind.

---

## System Workflow

```
Program starts
    |
    v
Build 200-node adjacency list from ROADS[] array  (heap malloc)
    |
    v
Show interactive menu
    |
    +--[1]--> Dijkstra: user picks src + dest -> shortest path + km + minutes
    |
    +--[2]--> Dijkstra: user picks hub -> table of all distances from that hub
    |
    +--[3]--> Delivery Agent Route Planner:
    |             User enters: name, depot, N stops (by name or index)
    |             Floyd-Warshall runs once (if not already cached)
    |             Greedy NN TSP     -> fast route
    |             Held-Karp DP TSP  -> optimal route
    |             Comparison: how much % does DP save over Greedy?
    |
    +--[4]--> Full Demo (3 agents, real clusters):
    |             Floyd-Warshall (if not cached)
    |             Max Flow -> assigns agents to clusters
    |             Greedy + DP routes for each agent
    |             Performance metrics table
    |
    +--[5]--> List all 200 locations by zone
    |
    +--[6]--> Exit -> graph_destroy() frees all heap memory
```

---

## Build & Run

```bash
# Requirements: GCC with C11 support, make
# Works on: Linux, macOS, WSL on Windows, MinGW/Git Bash

# Compile all modules
make

# Compile and run
make run

# Run directly
./sudos

# Clean compiled objects and binary
make clean

# Check for memory leaks (requires valgrind)
make valgrind
```

### Windows (no make available)
```bash
gcc -Wall -std=c11 -O2 -o sudos ^
    main.c graph.c dijkstra.c floyd.c ^
    tsp_greedy.c tsp_dp.c maxflow.c metrics.c

sudos.exe
```

---

## Interactive Menu -- How to Use

### [1] Shortest Path Between Two Locations
```
Enter name or index [0-199]: connaught
  -> [0] Connaught_Place
Enter name or index [0-199]: 69
  -> [69] Nehru_Place

Distance    : 18 km
Est. Time   : ~36 min  (avg 30 km/h city traffic)
Route       : Connaught_Place -> Green_Park -> Lajpat_Nagar
              -> Defence_Colony -> Greater_Kailash1 -> Kalkaji -> Nehru_Place
```

- Type a partial name (e.g. "noida sec1", "mayur", "cyber") -- it does substring search
- Or type the node index directly (e.g. "110" for Noida Sec1)
- If no exact match, it shows all locations that contain your search term

### [2] All Distances from a Hub
Runs Dijkstra from any chosen location and prints a table of distances
to every reachable node -- useful for a depot manager planning coverage area.

### [3] Delivery Agent Route Planner (main feature)
```
Your name (agent): Ramesh
Number of delivery stops (1 to 19): 4
DEPOT (your starting location): 0        <- Connaught Place
Stop 1 of 4: 82                          <- Mayur Vihar Ph1
Stop 2 of 4: 89                          <- Dilshad Garden
Stop 3 of 4: 116                         <- Noida Sec10
Stop 4 of 4: 122                         <- Noida Sec18

[Greedy NN]   Route: CP -> MayurVihar -> Dilshad -> Sec10 -> Sec18 -> CP
[Greedy NN]   Total Cost: 75 km

[DP Optimal]  Route: CP -> Sec18 -> Sec10 -> MayurVihar -> Dilshad -> CP
[DP Optimal]  Total Cost: 66 km

>> DP saves 12.0% over Greedy  (66 km vs 75 km)
```

### [4] Full SUDOS Demo (3 agents, predefined clusters)
Runs the complete pipeline -- Max Flow assignment followed by Greedy and
DP routing for each agent, ending with a performance comparison table.

### [5] List All 200 Locations
Prints all locations grouped by zone with their node index -- so agents
can look up the index of any stop before entering it in the route planner.

---

## Sample Performance Output

```
+---------------------------+--------------+---------------+
|  SUDOS - Algorithm Performance Comparison                 |
+---------------------------+--------------+---------------+
| Algorithm                 |   Time (us)  |   Route Cost  |
+---------------------------+--------------+---------------+
| Max Flow (Assignment)     |        0.021 |             7 |
| Greedy NN [Ramesh]        |        0.008 |            75 |
| DP Optimal [Ramesh]       |        0.312 |            66 |
| Greedy NN [Sunita]        |        0.011 |            28 |
| DP Optimal [Sunita]       |        0.284 |            28 |
+---------------------------+--------------+---------------+
  * Best route cost: 66 km
  >> Greedy overhead vs Optimal: 13.6%
```

All output uses plain ASCII only -- no Unicode box-drawing characters.
Works correctly on Windows CMD, PowerShell, Git Bash, Linux, macOS terminals.

---

## Module Descriptions

### `graph.h / graph.c`
- Adjacency list using a linked list of `Edge` structs per node
- `graph_add_undirected_edge()` adds both directions
- `graph_to_matrix()` exports weight matrix for TSP use
- `graph_destroy()` frees all heap-allocated edge memory
- `MAX_NODES` set to 250, supports up to 250 locations

### `dijkstra.h / dijkstra.c`
- O(V^2) SSSP using linear array scan (no priority queue)
- Returns `dist[]` (distances) and `prev[]` (path reconstruction)
- `dijkstra_print_path()` recursively reconstructs the full road path
- `dijkstra_print_table()` prints the full source-to-all table
- For 200 nodes: fast enough for interactive real-time use

### `floyd.h / floyd.c`
- O(V^3) all-pairs DP: 3 nested loops over k, i, j
- `next[][]` table enables path reconstruction for any pair
- For large graphs (n > 20), matrix display is suppressed --
  shows a 10-row sample and directs user to query mode instead
- Result is cached in static global `fw_dist` / `fw_next` in main.c

### `tsp_greedy.h / tsp_greedy.c`
- Nearest-Neighbour heuristic on a subset of nodes
- Takes `nodes[]` array + precomputed `fw_dist` matrix
- Builds closed tour: depot -> nearest unvisited -> ... -> depot
- O(N^2), runs in microseconds even for 19 stops

### `tsp_dp.h / tsp_dp.c`
- Held-Karp exact TSP using bitmask DP
- `dp[mask][i]` = min cost to visit nodes in `mask`, ending at node `i`
- Tables allocated on heap (not stack) to handle up to 2^20 states
- `parent[mask][i]` used to reconstruct the optimal path
- Practical upper limit: 20 stops per agent (2^20 = ~1M states)

### `maxflow.h / maxflow.c`
- Edmonds-Karp BFS-augmented Ford-Fulkerson
- Network: Source -> Agents -> Clusters -> Sink
- Capacity: agent capacity on S->agent edges, cluster demand on cluster->T edges
- `affinity[][]` matrix controls which agents can serve which clusters
- Returns `assignment[agent][cluster]` flow matrix

### `metrics.h / metrics.c`
- `clock()`-based microsecond timing using `CLOCKS_PER_SEC`
- Stores up to 16 algorithm entries per session
- Prints plain ASCII comparison table (no Unicode)
- Computes greedy overhead percentage vs optimal

---

## File Structure

```
SUDOS/
|-- main.c              <- interactive menu, 200 city data, simulation
|-- graph.h             <- Graph struct, Edge struct, MAX_NODES=250
|-- graph.c             <- adjacency list build + destroy
|-- dijkstra.h/.c       <- single-source shortest path
|-- floyd.h/.c          <- all-pairs shortest path
|-- tsp_greedy.h/.c     <- nearest-neighbour TSP heuristic
|-- tsp_dp.h/.c         <- Held-Karp bitmask DP (exact TSP)
|-- maxflow.h/.c        <- Edmonds-Karp Ford-Fulkerson
|-- metrics.h/.c        <- timing + ASCII comparison table
|-- Makefile            <- build rules, make / make run / make clean
|-- sudos_dashboard.html <- browser-based visualization (HTML/SVG/JS)
`-- README.md           <- this file
```

---

## Real-World Connection

| SUDOS Component              | Industry Equivalent                         |
|------------------------------|---------------------------------------------|
| 200-node Delhi/Noida graph   | Google Maps road graph (millions of nodes)  |
| Adjacency list (heap)        | Road network in PostGIS spatial database    |
| Dijkstra (interactive)       | Google Maps "get directions" query          |
| Floyd-Warshall (cached)      | Pre-computed distance matrix cache          |
| Greedy NN TSP                | Swiggy / Blinkit real-time dispatch         |
| Held-Karp DP TSP             | Amazon batch route optimizer                |
| Edmonds-Karp Max Flow        | Driver-to-zone allocation engine            |
| metrics.c timing table       | A/B testing framework for routing algos     |

---

## Known Limitations

- All data is in-memory only -- no file or database persistence between sessions
- Floyd-Warshall on 200 nodes takes ~2-3 seconds on first run (computed once)
- Held-Karp DP is limited to 20 stops per agent due to 2^N state explosion
- Road distances are approximate (realistic km estimates, not GPS-exact)
- No live traffic data -- weights are static distances, not real-time travel time

---

## Possible Extensions

- Load city graph from a CSV file at startup
- Add real-time traffic weight adjustment
- Support priority deliveries (weighted TSP variant)
- Add a web API layer so the route planner can be called over HTTP
- Replace Floyd-Warshall with Johnson's Algorithm for sparser graphs O(VE + V^2 log V)
- Use a min-heap priority queue in Dijkstra to get O((V+E) log V)

---

*SUDOS implements the same algorithmic principles used in production logistics
at academic scale, demonstrating practical understanding of graph algorithms,
dynamic programming, greedy heuristics, and network flow -- applied to a real
200-location map of Delhi and Noida.*

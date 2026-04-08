/*
 * ============================================================
 * SUDOS -- Smart Urban Delivery Optimization System
 * DAA Project  |  VIT Vellore
 * ============================================================
 * City: Delhi + Noida + NCR  (200 real locations)
 *
 * Algorithms:
 *   1. Dijkstra          -- Single-source shortest path  O(V^2)
 *   2. Floyd-Warshall    -- All-pairs shortest path      O(V^3)
 *   3. Greedy NN TSP     -- Fast approximate route       O(N^2)
 *   4. Held-Karp DP TSP  -- Exact optimal route         O(2^N*N^2)
 *   5. Edmonds-Karp Flow -- Agent-cluster assignment    O(V*E^2)
 *
 * Interactive modes:
 *   [1] Shortest path between any two locations (Dijkstra)
 *   [2] All distances from a chosen hub
 *   [3] Delivery agent: input your stops, get optimised route
 *   [4] Full SUDOS simulation demo (3 agents, real clusters)
 *   [5] List all 200 locations
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "graph.h"
#include "dijkstra.h"
#include "floyd.h"
#include "tsp_greedy.h"
#include "tsp_dp.h"
#include "maxflow.h"
#include "metrics.h"

/* ============================================================
   200 DELHI / NOIDA / NCR LOCATIONS
   Weights represent approximate road distance in km.
   ============================================================ */
#define N_CITY 200

static const char *CITY_NAMES[N_CITY] = {
    /* --- Central Delhi  (0-19) --- */
    "Connaught_Place",   "India_Gate",         "Rajpath",
    "Parliament_House",  "Jantar_Mantar",      "Patel_Chowk",
    "Mandi_House",       "Barakhamba_Road",    "Pragati_Maidan",
    "ITO",               "Delhi_Gate",         "Daryaganj",
    "Chandni_Chowk",     "Kashmere_Gate",      "ISBT_Delhi",
    "Civil_Lines",       "Rajghat",            "Lal_Qila",
    "Jama_Masjid",       "Paharganj",
    /* --- North Delhi  (20-39) --- */
    "Rohini_Sec1",       "Rohini_Sec2",        "Rohini_Sec3",
    "Pitampura",         "Shalimar_Bagh",      "Ashok_Vihar",
    "Model_Town",        "GTB_Nagar",          "Mukherjee_Nagar",
    "Azadpur",           "Jahangirpuri",       "Burari",
    "Narela",            "Bawana",             "Kirari",
    "Prashant_Vihar",    "Sainik_Vihar",       "Mangolpuri",
    "Sultanpuri",        "Peeragarhi",
    /* --- West Delhi  (40-59) --- */
    "Janakpuri",         "Uttam_Nagar",        "Dwarka_Sec1",
    "Dwarka_Sec6",       "Dwarka_Sec10",       "Dwarka_Sec21",
    "Najafgarh",         "Vikaspuri",          "Paschim_Vihar",
    "Punjabi_Bagh",      "Rajouri_Garden",     "Tagore_Garden",
    "Subhash_Nagar",     "Tilak_Nagar",        "Hari_Nagar",
    "Palam",             "Mahipalpur",         "Bijwasan",
    "Kapashera",         "Delhi_Cantt",
    /* --- South Delhi  (60-79) --- */
    "Saket",             "Malviya_Nagar",      "Hauz_Khas",
    "Green_Park",        "Lajpat_Nagar",       "Defence_Colony",
    "Greater_Kailash1",  "Greater_Kailash2",   "Kalkaji",
    "Nehru_Place",       "Okhla",              "Govindpuri",
    "Badarpur",          "Tughlakabad",        "Mehrauli",
    "Qutub_Minar",       "Vasant_Kunj",        "Vasant_Vihar",
    "RK_Puram",          "Munirka",
    /* --- East Delhi  (80-99) --- */
    "Laxmi_Nagar",       "Preet_Vihar",        "Mayur_Vihar_Ph1",
    "Mayur_Vihar_Ph2",   "Mayur_Vihar_Ph3",    "IP_Extension",
    "Patparganj",        "Shakarpur",          "Vivek_Vihar",
    "Dilshad_Garden",    "Gandhi_Nagar",       "Shahdara",
    "Seelampur",         "Welcome",            "Johripur",
    "Anand_Vihar",       "Karkardooma",        "Nirman_Vihar",
    "New_Ashok_Nagar",   "Kondli",
    /* --- South-East Delhi  (100-109) --- */
    "Jasola",            "Sarita_Vihar",       "Madanpur_Khadar",
    "Kalindi_Kunj",      "Jamia_Nagar",        "Batla_House",
    "Zakir_Nagar",       "Jangpura",           "Bhogal",
    "Nizamuddin",
    /* --- Noida  (110-149) --- */
    "Noida_Sec1",        "Noida_Sec2",         "Noida_Sec3",
    "Noida_Sec4",        "Noida_Sec5",         "Noida_Sec6",
    "Noida_Sec10",       "Noida_Sec11",        "Noida_Sec12",
    "Noida_Sec14",       "Noida_Sec15",        "Noida_Sec16",
    "Noida_Sec18",       "Noida_Sec19",        "Noida_Sec22",
    "Noida_Sec25",       "Noida_Sec27",        "Noida_Sec29",
    "Noida_Sec30",       "Noida_Sec31",        "Noida_Sec32",
    "Noida_Sec33",       "Noida_Sec34",        "Noida_Sec35",
    "Noida_Sec36",       "Noida_Sec37",        "Noida_Sec38",
    "Noida_Sec39",       "Noida_Sec40",        "Noida_Sec41",
    "Noida_Sec44",       "Noida_Sec45",        "Noida_Sec46",
    "Noida_Sec47",       "Noida_Sec50",        "Noida_Sec51",
    "Noida_Sec52",       "Noida_Sec55",        "Noida_Sec56",
    "Noida_Sec57",
    /* --- Greater Noida  (150-179) --- */
    "GNoida_West",       "GNoida_Alpha1",      "GNoida_Alpha2",
    "GNoida_Beta1",      "GNoida_Beta2",       "GNoida_Gamma1",
    "GNoida_Gamma2",     "GNoida_Delta1",      "GNoida_Delta2",
    "Knowledge_Park1",   "Knowledge_Park2",    "Knowledge_Park3",
    "Knowledge_Park4",   "Knowledge_Park5",    "Pari_Chowk",
    "Surajpur",          "Kasna",              "Dadri",
    "Dankaur",           "Rabupura",           "Jewar",
    "GN_Sector1",        "GN_Sector2",         "GN_Sector3",
    "GN_Sector4",        "GN_Sector5",         "GN_Sector10",
    "Ecotech1",          "Ecotech2",           "Ecotech3",
    /* --- NCR Fringe  (180-199) --- */
    "Vasundhara_GZB",    "Indirapuram_GZB",    "RajNagar_GZB",
    "Kaushambi_GZB",     "Vaishali_GZB",       "Faridabad_Sec15",
    "Faridabad_Sec16",   "Faridabad_Sec17",    "Ballabgarh",
    "Gurugram_Sec14",    "Gurugram_Sec29",     "Cyber_City_GGN",
    "DLF_Phase1_GGN",    "DLF_Phase2_GGN",    "Sohna_Road_GGN",
    "Bahadurgarh",       "Kundli",             "Sonipat",
    "Faridabad_NIT",     "Noida_Expressway"
};

/* ============================================================
   ROAD NETWORK  -- (src, dst, km)
   ============================================================ */
static const int ROADS[][3] = {
    /* Central Delhi internal */
    {0,1,3},{0,2,2},{0,4,1},{0,5,2},{0,6,2},{0,7,1},{0,19,2},
    {1,2,2},{1,8,3},{1,9,4},{1,16,3},{1,17,2},
    {2,3,1},{2,5,2},
    {3,4,1},{3,5,1},
    {5,6,1},{5,18,2},
    {6,7,1},{6,8,2},
    {7,8,2},{7,9,3},
    {8,9,2},{8,16,3},{8,17,3},
    {9,10,2},{9,11,2},{9,16,3},
    {10,11,1},{10,17,2},{10,18,2},
    {11,12,2},{11,17,1},{11,18,1},
    {12,13,2},{12,17,2},{12,18,1},
    {13,14,1},{13,15,2},
    {14,15,2},
    {15,16,3},
    {16,17,2},
    {17,18,1},
    {18,19,3},
    /* North Delhi internal */
    {20,21,2},{20,23,3},{20,35,4},
    {21,22,2},{21,24,3},
    {22,23,2},{22,34,4},
    {23,24,2},{23,35,3},
    {24,25,3},{24,36,3},
    {25,26,3},{25,28,4},
    {26,27,2},{26,29,4},
    {27,28,2},{27,29,3},
    {28,29,3},
    {29,30,4},{29,31,5},
    {30,31,4},{30,37,5},
    {31,32,8},{31,33,9},
    {32,33,6},
    {33,34,7},
    {34,37,4},{34,38,3},
    {35,36,3},
    {36,37,3},
    {37,38,3},
    {38,39,3},
    /* West Delhi internal */
    {40,41,4},{40,47,3},{40,50,5},
    {41,42,5},{41,47,4},
    {42,43,3},{42,55,6},
    {43,44,3},{43,45,5},
    {44,45,3},{44,55,5},
    {45,46,8},{45,55,4},{45,56,5},
    {46,55,9},{46,57,6},{46,58,5},
    {47,48,4},{47,53,4},
    {48,49,4},{48,39,3},
    {49,50,3},
    {50,51,2},
    {51,52,2},
    {52,53,2},
    {53,54,2},
    {54,59,4},
    {55,56,4},{55,57,5},
    {56,57,4},{56,58,3},
    {57,58,3},
    {58,59,5},
    {59,78,5},
    /* South Delhi internal */
    {60,61,2},{60,74,5},{60,76,4},
    {61,62,2},{61,74,4},
    {62,63,2},{62,79,3},
    {63,64,3},{63,78,4},
    {64,65,2},{64,107,4},
    {65,66,2},{65,108,3},
    {66,67,2},{66,68,3},
    {67,68,2},{67,100,5},
    {68,69,2},{68,71,3},
    {69,70,3},{69,104,5},
    {70,71,3},{70,100,4},{70,101,4},
    {71,72,4},
    {72,73,4},
    {73,74,5},
    {74,75,2},
    {75,76,4},
    {76,77,3},
    {77,78,3},
    {78,79,2},
    {79,62,3},
    /* East Delhi internal */
    {80,81,2},{80,87,3},{80,90,4},
    {81,82,2},
    {82,83,2},{82,86,3},
    {83,84,2},{83,99,4},
    {84,85,3},{84,98,3},
    {85,86,2},{85,95,4},
    {86,87,3},
    {87,88,3},{87,96,3},
    {88,89,3},
    {89,90,3},
    {90,91,3},
    {91,92,3},{91,93,3},
    {92,93,2},{92,94,3},
    {93,94,2},
    {94,95,3},
    {95,96,2},
    {96,97,2},
    {97,98,2},
    {98,99,2},
    /* South-East Delhi internal */
    {100,101,3},{100,104,5},
    {101,102,3},{101,103,4},
    {102,103,3},
    {103,104,4},
    {104,105,2},
    {105,106,2},{105,109,4},
    {106,107,2},
    {107,108,2},{107,109,2},
    {108,109,1},
    /* Noida internal */
    {110,111,2},{110,116,4},
    {111,112,2},{111,117,3},
    {112,113,2},{112,118,3},
    {113,114,2},{113,119,3},
    {114,115,2},{114,120,3},
    {115,116,4},{115,121,3},
    {116,117,2},{116,122,4},
    {117,118,2},{117,123,3},
    {118,119,3},{118,124,4},
    {119,120,3},{119,125,4},
    {120,121,3},{120,126,4},
    {121,122,4},{121,127,4},
    {122,123,3},{122,128,4},
    {123,124,4},{123,129,4},
    {124,125,4},{124,130,5},
    {125,126,4},{125,131,5},
    {126,127,4},{126,132,5},
    {127,128,4},{127,133,5},
    {128,129,4},{128,134,5},
    {129,130,4},{129,135,5},
    {130,131,4},{130,136,5},
    {131,132,4},{131,137,5},
    {132,133,4},{132,138,5},
    {133,134,4},{133,139,5},
    {134,135,4},
    {135,136,4},{135,144,7},
    {136,137,4},{136,145,7},
    {137,138,4},{137,146,7},
    {138,139,4},{138,147,8},
    {139,140,5},{139,148,8},
    {140,141,4},{140,149,8},
    {141,142,4},
    {142,143,4},
    {143,144,5},
    {144,145,4},
    {145,146,4},
    {146,147,5},
    {147,148,5},
    {148,149,5},
    /* Greater Noida internal */
    {150,151,4},{150,171,5},
    {151,152,3},{151,159,4},
    {152,153,3},{152,160,4},
    {153,154,3},{153,161,4},
    {154,155,3},{154,162,5},
    {155,156,4},{155,163,5},
    {156,157,4},{156,164,6},
    {157,158,4},{157,165,5},
    {158,159,4},{158,166,6},
    {159,160,4},{159,164,5},
    {160,161,4},{160,165,5},
    {161,162,4},{161,166,5},
    {162,163,5},{162,167,7},
    {163,164,5},{163,168,8},
    {164,165,4},
    {165,166,5},{165,177,6},
    {166,167,5},{166,178,7},
    {167,168,5},{167,179,8},
    {168,169,6},
    {169,170,8},
    {171,172,3},{172,173,3},{173,174,3},{174,175,3},{175,176,4},
    {176,177,5},{177,178,4},{178,179,4},
    /* NCR Fringe internal */
    {180,181,5},{180,184,4},
    {181,182,5},{181,183,4},
    {182,183,4},
    {183,184,3},
    {185,186,3},{185,198,5},
    {186,187,3},
    {187,188,6},
    {188,198,5},
    {189,190,4},{189,194,6},
    {190,191,5},
    {191,192,3},{192,193,3},{193,194,4},
    {195,196,6},{196,197,12},
    {198,185,5},
    {199,142,5},{199,149,6},
    /* ---- INTER-ZONE ARTERIAL ROADS ---- */
    /* Central <-> North */
    {0,15,3},{0,25,8},{0,27,6},
    {13,26,5},{13,27,5},
    {14,25,4},{14,29,6},
    {15,26,5},{15,27,4},
    {12,29,6},
    /* Central <-> West */
    {0,49,7},{0,50,8},
    {19,40,9},{19,48,8},
    {4,52,7},{4,53,7},
    {5,59,9},
    /* Central <-> South */
    {0,63,6},{0,62,7},
    {9,64,7},{9,65,8},
    {6,107,9},{6,108,8},
    {2,78,7},{2,77,8},
    /* Central <-> East */
    {9,80,6},{9,87,7},
    {8,80,7},{8,86,8},
    {11,91,7},{11,90,6},
    {16,86,8},
    /* Central <-> South-East */
    {9,107,8},{9,109,9},
    {10,108,7},{10,107,6},
    {11,105,8},{11,104,9},
    /* North <-> West */
    {20,48,7},{20,49,8},
    {39,48,4},{38,48,5},
    /* West <-> South */
    {59,78,5},{59,77,6},
    {40,60,8},{40,63,9},
    {54,60,7},{54,61,8},
    /* South <-> South-East */
    {70,100,4},{70,101,5},
    {69,104,5},{69,105,6},
    {64,107,4},{64,108,5},
    {65,108,4},{65,109,5},
    /* East <-> Noida (DND/Yamuna bridges) */
    {82,110,7},{82,111,8},
    {83,111,7},{83,112,8},
    {95,111,8},{95,112,8},
    {98,110,6},{98,113,9},
    {80,110,9},
    /* South-East <-> Noida (Kalindi Kunj/Okhla) */
    {103,124,6},{103,122,8},
    {102,122,7},{102,123,8},
    {100,121,9},{101,122,8},
    /* East Delhi <-> Ghaziabad */
    {89,180,8},{89,181,9},
    {95,181,7},{95,183,8},
    {88,182,9},
    /* South <-> Faridabad */
    {72,185,10},{72,186,11},
    {73,185,9},{73,198,8},
    {70,198,11},
    /* West <-> Gurugram */
    {56,191,18},{56,192,16},
    {58,192,12},{58,189,14},
    {55,194,16},{55,189,18},
    /* North <-> Bahadurgarh/Sonipat */
    {33,195,18},{32,196,12},
    {31,196,15},{31,197,20},
    /* Noida <-> Greater Noida */
    {144,150,6},{145,151,7},
    {146,152,8},{147,153,8},
    {148,154,9},{149,164,9},
    {143,199,5},{199,164,10},
    /* Greater Noida <-> Faridabad */
    {164,188,22},{165,188,20},
    /* Noida Expressway */
    {199,110,14},{199,116,10},
    {199,122,8},{199,130,6},
    {199,138,5},{199,144,4}
};
#define N_ROADS (int)(sizeof(ROADS)/sizeof(ROADS[0]))

/* ============================================================
   GLOBAL all-pairs distance table (static to avoid stack overflow)
   ============================================================ */
static int fw_dist[MAX_NODES][MAX_NODES];
static int fw_next[MAX_NODES][MAX_NODES];
static int fw_ready = 0;

/* ============================================================
   HELPERS
   ============================================================ */
static void read_line(char *buf, int sz) {
    if (fgets(buf, sz, stdin)) {
        int len = (int)strlen(buf);
        while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r'))
            buf[--len] = '\0';
    }
}

/* Case-insensitive substring search; returns node index or -1 */
static int find_node_by_name(const Graph *g, const char *q) {
    int qlen = (int)strlen(q);
    if (qlen == 0) return -1;
    for (int i = 0; i < g->num_nodes; i++) {
        const char *name = g->node_names[i];
        int nlen = (int)strlen(name);
        for (int s = 0; s <= nlen - qlen; s++) {
            int match = 1;
            for (int k = 0; k < qlen; k++) {
                if (tolower((unsigned char)name[s+k]) !=
                    tolower((unsigned char)q[k])) { match = 0; break; }
            }
            if (match) return i;
        }
    }
    return -1;
}

/* Show all locations that match query */
static void show_matches(const Graph *g, const char *q) {
    int qlen = (int)strlen(q), found = 0;
    for (int i = 0; i < g->num_nodes; i++) {
        const char *name = g->node_names[i];
        int nlen = (int)strlen(name);
        for (int s = 0; s <= nlen - qlen; s++) {
            int match = 1;
            for (int k = 0; k < qlen; k++) {
                if (tolower((unsigned char)name[s+k]) !=
                    tolower((unsigned char)q[k])) { match = 0; break; }
            }
            if (match) { printf("    [%3d] %s\n", i, name); found++; break; }
        }
    }
    if (!found) printf("    (no matches for '%s')\n", q);
}

/*
 * Prompt user to pick a location.
 * Accepts: partial name (e.g. "noida sec1"), or a plain number (e.g. "110").
 */
static int prompt_location(const Graph *g, const char *prompt) {
    char buf[128];
    while (1) {
        printf("  %s\n  Enter name or index [0-%d]: ", prompt, g->num_nodes - 1);
        read_line(buf, sizeof(buf));
        if (!buf[0]) continue;

        /* all digits -> treat as index */
        int all_digits = 1;
        for (int i = 0; buf[i]; i++)
            if (!isdigit((unsigned char)buf[i])) { all_digits = 0; break; }
        if (all_digits) {
            int idx = atoi(buf);
            if (idx >= 0 && idx < g->num_nodes) {
                printf("    -> [%d] %s\n", idx, g->node_names[idx]);
                return idx;
            }
            printf("    Index out of range (0-%d). Try again.\n", g->num_nodes-1);
            continue;
        }

        /* substring search */
        int node = find_node_by_name(g, buf);
        if (node >= 0) {
            printf("    -> [%d] %s\n", node, g->node_names[node]);
            return node;
        }

        printf("    No exact match. Closest results:\n");
        show_matches(g, buf);
        printf("    Please try again.\n");
    }
}

/* ============================================================
   FEATURE 1 -- Shortest path between two locations
   ============================================================ */
static void feature_shortest_path(const Graph *g) {
    printf("\n=== Shortest Path Finder (Dijkstra) ===\n");
    int src  = prompt_location(g, "SOURCE location:");
    int dest = prompt_location(g, "DESTINATION location:");

    if (src == dest) { printf("  Same source and destination!\n"); return; }

    int dist[MAX_NODES], prev[MAX_NODES];
    clock_t t0 = clock();
    dijkstra(g, src, dist, prev);
    double us = ((double)(clock()-t0)/CLOCKS_PER_SEC)*1e6;

    printf("\n  Source      : [%d] %s\n", src,  g->node_names[src]);
    printf("  Destination : [%d] %s\n", dest, g->node_names[dest]);

    if (dist[dest] == INF) {
        printf("  Result      : NO PATH EXISTS\n");
        return;
    }
    printf("  Distance    : %d km\n", dist[dest]);
    printf("  Est. Time   : ~%d min  (avg 30 km/h city traffic)\n",
           dist[dest] * 2);
    printf("  Compute Time: %.3f microseconds\n", us);
    printf("  Route       :");
    dijkstra_print_path(g, prev, src, dest);
    printf("\n");
}

/* ============================================================
   FEATURE 2 -- All distances from a hub
   ============================================================ */
static void feature_all_distances(const Graph *g) {
    printf("\n=== All Distances from a Hub (Dijkstra) ===\n");
    int src = prompt_location(g, "HUB / source location:");

    int dist[MAX_NODES], prev[MAX_NODES];
    dijkstra(g, src, dist, prev);

    printf("\n");
    dijkstra_print_table(g, src, dist, prev);
}

/* ============================================================
   FEATURE 3 -- Delivery agent enters stops -> best route
   ============================================================ */
static void feature_delivery_route(const Graph *g) {
    if (!fw_ready) {
        printf("  Computing all-pairs distances (Floyd-Warshall, %d nodes)...",
               g->num_nodes);
        fflush(stdout);
        floyd_warshall(g, fw_dist, fw_next);
        fw_ready = 1;
        printf(" done.\n");
    }

    printf("\n=== Delivery Route Planner ===\n");

    char agent_name[64];
    printf("  Your name (agent): ");
    read_line(agent_name, sizeof(agent_name));
    if (!agent_name[0]) strcpy(agent_name, "Agent");

    int max_stops = MAX_DP_NODES - 1;
    printf("\n  Number of delivery stops (1 to %d)  [depot added automatically]: ",
           max_stops);
    char buf[16];
    read_line(buf, sizeof(buf));
    int n_stops = atoi(buf);
    if (n_stops < 1 || n_stops > max_stops) {
        printf("  Invalid. Must be 1-%d.\n", max_stops);
        return;
    }

    int depot = prompt_location(g, "DEPOT (your starting location):");

    int stops[MAX_DP_NODES];
    stops[0] = depot;
    int nn = 1;

    for (int i = 0; i < n_stops; ) {
        char msg[64];
        snprintf(msg, 64, "Stop %d of %d:", i+1, n_stops);
        int node = prompt_location(g, msg);
        int dup = 0;
        for (int k = 0; k < nn; k++) if (stops[k] == node) { dup = 1; break; }
        if (dup) { printf("    (already added, skipping)\n"); continue; }
        stops[nn++] = node;
        i++;
    }

    printf("\n  Agent  : %s\n", agent_name);
    printf("  Depot  : [%d] %s\n", depot, g->node_names[depot]);
    printf("  Stops  :");
    for (int i = 1; i < nn; i++)
        printf(" [%d]%s", stops[i], g->node_names[stops[i]]);
    printf("\n");

    MetricsTable mt;
    metrics_init(&mt);

    /* Greedy NN */
    int g_tour[MAX_DP_NODES+1], g_len = 0;
    clock_t t0 = clock();
    int g_cost = tsp_greedy(fw_dist, stops, nn, depot, g_tour, &g_len);
    metrics_record(&mt, "Greedy NN TSP", t0, clock(), g_cost);
    printf("\n");
    tsp_greedy_print(g, g_tour, g_len, g_cost);

    /* Held-Karp DP (exact) */
    if (nn <= MAX_DP_NODES) {
        int d_tour[MAX_DP_NODES+1], d_len = 0;
        t0 = clock();
        int d_cost = tsp_dp(fw_dist, stops, nn, depot, d_tour, &d_len);
        metrics_record(&mt, "Held-Karp DP (Optimal)", t0, clock(), d_cost);
        tsp_dp_print(g, d_tour, d_len, d_cost);

        if (g_cost > 0 && d_cost > 0) {
            if (g_cost > d_cost)
                printf("\n  >> DP saves %.1f%% over Greedy  (%d km vs %d km)\n",
                       100.0*(g_cost-d_cost)/g_cost, d_cost, g_cost);
            else
                printf("\n  >> Greedy matched Optimal -- perfect heuristic!\n");
        }
    }

    metrics_print(&mt);
}

/* ============================================================
   FEATURE 4 -- Full SUDOS demo  (3 agents, real Delhi clusters)
   ============================================================ */
#define DEMO_AGENTS   3
#define DEMO_CLUSTERS 3

static void feature_full_demo(const Graph *g) {
    if (!fw_ready) {
        printf("  Computing Floyd-Warshall (%d nodes)...", g->num_nodes);
        fflush(stdout);
        floyd_warshall(g, fw_dist, fw_next);
        fw_ready = 1;
        printf(" done.\n");
    }

    MetricsTable mt;
    metrics_init(&mt);

    static const char *AGENT_NAMES[DEMO_AGENTS]  = {"Ramesh","Sunita","Arjun"};
    static const int   AGENT_CAP[DEMO_AGENTS]    = {4, 5, 4};
    static const char *CLUSTER_NAMES[DEMO_CLUSTERS] = {
        "Central-Delhi", "East-Delhi", "Noida-West"
    };
    static const int CLUSTER_DEMAND[DEMO_CLUSTERS] = {3, 4, 3};
    static const int AFFINITY[MAX_AGENTS][MAX_CLUSTERS] = {
        {1, 1, 0},
        {1, 1, 1},
        {0, 1, 1}
    };
    /* Delivery stops per cluster */
    static const int CLUSTER_STOPS[DEMO_CLUSTERS][6] = {
        { 0,  4,  7,  9, 12, -1},   /* Central */
        {80, 82, 86, 89, 95, -1},   /* East Delhi */
        {110,116,122,128,135, -1}   /* Noida */
    };
    static const int CLUSTER_SZ[DEMO_CLUSTERS] = {5,5,5};

    printf("\n============================================\n");
    printf("  PHASE 3: Agent Assignment (Max Flow)\n");
    printf("============================================\n");

    int assignment[MAX_AGENTS][MAX_CLUSTERS] = {{0}};
    clock_t t0 = clock();
    int total = max_flow_assign(DEMO_AGENTS, DEMO_CLUSTERS,
                                AGENT_CAP, CLUSTER_DEMAND,
                                AFFINITY, assignment);
    metrics_record(&mt, "Max Flow (Assignment)", t0, clock(), total);

    char an[MAX_AGENTS][32], cn[MAX_CLUSTERS][32];
    for (int i = 0; i < DEMO_AGENTS;   i++) strncpy(an[i], AGENT_NAMES[i], 31);
    for (int i = 0; i < DEMO_CLUSTERS; i++) strncpy(cn[i], CLUSTER_NAMES[i], 31);
    maxflow_print(DEMO_AGENTS, DEMO_CLUSTERS, an, cn, assignment, total);

    printf("\n============================================\n");
    printf("  PHASE 4: Routes per Agent\n");
    printf("============================================\n");

    for (int a = 0; a < DEMO_AGENTS; a++) {
        int c = -1;
        for (int k = 0; k < DEMO_CLUSTERS; k++)
            if (assignment[a][k] > 0) { c = k; break; }
        if (c < 0) {
            printf("\n  Agent %s: no cluster assigned.\n", AGENT_NAMES[a]);
            continue;
        }
        int nodes[MAX_DP_NODES], nn = 0;
        for (int k = 0; k < CLUSTER_SZ[c]; k++) {
            int nd = CLUSTER_STOPS[c][k];
            if (nd >= 0) nodes[nn++] = nd;
        }

        printf("\n  ----------------------------------------\n");
        printf("  Agent: %-10s | Cluster: %-16s | Stops: %d\n",
               AGENT_NAMES[a], CLUSTER_NAMES[c], nn);
        printf("  ----------------------------------------\n");

        int g_tour[MAX_DP_NODES+1], g_len = 0;
        char lbl[64];

        t0 = clock();
        int g_cost = tsp_greedy(fw_dist, nodes, nn, nodes[0], g_tour, &g_len);
        snprintf(lbl, 64, "Greedy NN [%s]", AGENT_NAMES[a]);
        metrics_record(&mt, lbl, t0, clock(), g_cost);
        tsp_greedy_print(g, g_tour, g_len, g_cost);

        if (nn <= MAX_DP_NODES) {
            int d_tour[MAX_DP_NODES+1], d_len = 0;
            t0 = clock();
            int d_cost = tsp_dp(fw_dist, nodes, nn, nodes[0], d_tour, &d_len);
            snprintf(lbl, 64, "DP Optimal [%s]", AGENT_NAMES[a]);
            metrics_record(&mt, lbl, t0, clock(), d_cost);
            tsp_dp_print(g, d_tour, d_len, d_cost);

            if (g_cost > 0 && d_cost > 0) {
                if (g_cost > d_cost)
                    printf("  >> DP saves %.1f%% over Greedy for %s\n",
                           100.0*(g_cost-d_cost)/g_cost, AGENT_NAMES[a]);
                else
                    printf("  >> Greedy matched Optimal for %s\n", AGENT_NAMES[a]);
            }
        }
    }

    printf("\n============================================\n");
    printf("  PHASE 5: Performance Summary\n");
    printf("============================================\n");
    metrics_print(&mt);
}

/* ============================================================
   FEATURE 5 -- List all 200 locations
   ============================================================ */
static void feature_list_all(const Graph *g) {
    static const char *ZONE_LABELS[] = {
        "Central Delhi  (0 - 19)",
        "North Delhi   (20 - 39)",
        "West Delhi    (40 - 59)",
        "South Delhi   (60 - 79)",
        "East Delhi    (80 - 99)",
        "SE Delhi     (100-109)",
        "Noida        (110-149)",
        "Greater Noida(150-179)",
        "NCR Fringe   (180-199)"
    };
    static const int ZS[] = {0,20,40,60,80,100,110,150,180};
    static const int ZE[] = {19,39,59,79,99,109,149,179,199};

    printf("\n=== All %d Locations ===\n", g->num_nodes);
    for (int z = 0; z < 9; z++) {
        printf("\n  [ %s ]\n", ZONE_LABELS[z]);
        for (int i = ZS[z]; i <= ZE[z]; i++)
            printf("    [%3d] %s\n", i, g->node_names[i]);
    }
    printf("\n");
}

/* ============================================================
   MAIN
   ============================================================ */
static void print_banner(void) {
    printf("\n");
    printf("+============================================================+\n");
    printf("|  SUDOS -- Smart Urban Delivery Optimization System         |\n");
    printf("|  DAA Project | Delhi, Noida & NCR | 200 Real Locations     |\n");
    printf("+------------------------------------------------------------+\n");
    printf("|  Algorithms: Dijkstra | Floyd-Warshall | Greedy TSP        |\n");
    printf("|              Held-Karp DP TSP | Edmonds-Karp Max Flow      |\n");
    printf("+============================================================+\n\n");
}

int main(void) {
    print_banner();

    printf("  Building city graph (%d locations, %d roads)...\n",
           N_CITY, N_ROADS);
    Graph *city = graph_create(N_CITY);
    if (!city) { fprintf(stderr, "ERROR: graph_create failed\n"); return 1; }
    for (int i = 0; i < N_CITY; i++)
        graph_set_name(city, i, CITY_NAMES[i]);
    for (int i = 0; i < N_ROADS; i++)
        graph_add_undirected_edge(city, ROADS[i][0], ROADS[i][1], ROADS[i][2]);
    printf("  Ready: %d nodes, %d directed edges.\n\n",
           city->num_nodes, city->num_edges);

    char choice[8];
    int running = 1;

    while (running) {
        printf("------------------------------------------------------------\n");
        printf("  SUDOS Main Menu\n");
        printf("------------------------------------------------------------\n");
        printf("  [1]  Find shortest path between two locations\n");
        printf("  [2]  Show all distances from a hub (Dijkstra table)\n");
        printf("  [3]  Delivery agent: enter your stops -> get best route\n");
        printf("  [4]  Run full SUDOS simulation (3 agents, real clusters)\n");
        printf("  [5]  List all 200 Delhi/Noida locations\n");
        printf("  [6]  Exit\n");
        printf("------------------------------------------------------------\n");
        printf("  Choice: ");
        read_line(choice, sizeof(choice));
        printf("\n");

        switch (choice[0]) {
            case '1': feature_shortest_path(city);  break;
            case '2': feature_all_distances(city);  break;
            case '3': feature_delivery_route(city); break;
            case '4': feature_full_demo(city);      break;
            case '5': feature_list_all(city);       break;
            case '6': running = 0;                  break;
            default:  printf("  Invalid choice. Enter 1-6.\n"); break;
        }
        printf("\n");
    }

    graph_destroy(city);
    printf("  SUDOS session ended. Goodbye!\n\n");
    return 0;
}

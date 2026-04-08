#include "metrics.h"
#include <stdio.h>
#include <string.h>

/* ------------------------------------------------------------------ */
void metrics_init(MetricsTable *t) {
    memset(t, 0, sizeof(*t));
}

/* ------------------------------------------------------------------ */
clock_t metrics_start(void) {
    return clock();
}

/* ------------------------------------------------------------------ */
void metrics_record(MetricsTable *t, const char *label,
                    clock_t start, clock_t end, int cost) {
    if (!t || t->count >= MAX_METRICS) return;
    MetricEntry *e = &t->entries[t->count++];
    strncpy(e->label, label, 63);
    e->label[63] = '\0';
    /* convert clock ticks to microseconds */
    e->time_us = ((double)(end - start) / CLOCKS_PER_SEC) * 1e6;
    e->cost    = cost;
}

/* ------------------------------------------------------------------ */
void metrics_print(const MetricsTable *t) {
    if (!t || t->count == 0) return;

    printf("\n+---------------------------+--------------+---------------+\n");
    printf("|  SUDOS - Algorithm Performance Comparison                 |\n");
    printf("+---------------------------+--------------+---------------+\n");
    printf("| %-25s | %12s | %13s |\n",
           "Algorithm", "Time (us)", "Route Cost");
    printf("+---------------------------+--------------+---------------+\n");

    for (int i = 0; i < t->count; i++) {
        const MetricEntry *e = &t->entries[i];
        char cost_str[16];
        if (e->cost < 0)
            snprintf(cost_str, 16, "%13s", "N/A");
        else
            snprintf(cost_str, 16, "%13d", e->cost);

        printf("| %-25s | %12.3f | %s |\n",
               e->label, e->time_us, cost_str);
    }

    int best_cost = -1;
    for (int i = 0; i < t->count; i++)
        if (t->entries[i].cost > 0 &&
            (best_cost < 0 || t->entries[i].cost < best_cost))
            best_cost = t->entries[i].cost;

    printf("+---------------------------+--------------+---------------+\n");
    if (best_cost > 0)
        printf("  * Best route cost: %d km\n", best_cost);

    int greedy_cost = -1, dp_cost = -1;
    for (int i = 0; i < t->count; i++) {
        if (strstr(t->entries[i].label, "Greedy")) greedy_cost = t->entries[i].cost;
        if (strstr(t->entries[i].label, "DP") || strstr(t->entries[i].label, "Optimal"))
            dp_cost = t->entries[i].cost;
    }
    if (greedy_cost > 0 && dp_cost > 0) {
        double overhead = 100.0 * (greedy_cost - dp_cost) / dp_cost;
        printf("  >> Greedy overhead vs Optimal: %.1f%%\n", overhead);
    }
    printf("\n");
}

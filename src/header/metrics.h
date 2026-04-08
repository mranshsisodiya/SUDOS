#ifndef METRICS_H
#define METRICS_H

#include <time.h>

/*
 * Lightweight performance metrics.
 *
 * Records wall-clock time (via clock()) and cost for each algorithm,
 * then prints a formatted comparison table.
 */

#define MAX_METRICS 16

typedef struct {
    char   label[64];     /* algorithm name            */
    double time_us;       /* execution time in µs      */
    int    cost;          /* route cost (-1 if N/A)    */
} MetricEntry;

typedef struct {
    MetricEntry entries[MAX_METRICS];
    int         count;
} MetricsTable;

void     metrics_init(MetricsTable *t);
void     metrics_record(MetricsTable *t, const char *label,
                        clock_t start, clock_t end, int cost);
void     metrics_print(const MetricsTable *t);

/* Convenience: start timer */
clock_t  metrics_start(void);

#endif /* METRICS_H */

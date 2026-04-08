# ============================================================
# SUDOS — Makefile
# Usage:
#   make          — build the binary
#   make run      — build and run
#   make clean    — remove build artifacts
#   make valgrind — run with memory checks
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -O2 -g
TARGET  = sudos

SRCS    = main.c graph.c dijkstra.c floyd.c \
          tsp_greedy.c tsp_dp.c maxflow.c metrics.c

OBJS    = $(SRCS:.c=.o)

.PHONY: all run clean valgrind

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: all
	./$(TARGET)

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

# Header dependencies
main.o:      main.c graph.h dijkstra.h floyd.h tsp_greedy.h tsp_dp.h maxflow.h metrics.h
graph.o:     graph.c graph.h
dijkstra.o:  dijkstra.c dijkstra.h graph.h
floyd.o:     floyd.c floyd.h graph.h
tsp_greedy.o:tsp_greedy.c tsp_greedy.h graph.h
tsp_dp.o:    tsp_dp.c tsp_dp.h graph.h
maxflow.o:   maxflow.c maxflow.h
metrics.o:   metrics.c metrics.h

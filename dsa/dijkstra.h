#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include "graph.h"

#ifdef __cplusplus
extern "C" {
#endif

int dijkstra(const int graph[GRAPH_VERTICES][GRAPH_VERTICES],
             int source, int destination, int *distance,
             int path[GRAPH_VERTICES], int *path_length);

#ifdef __cplusplus
}
#endif
#endif

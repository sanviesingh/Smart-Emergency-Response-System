#ifndef GRAPH_H
#define GRAPH_H

#ifdef __cplusplus
extern "C" {
#endif

#define GRAPH_VERTICES 5
#define GRAPH_INF 1000000

void graph_init(int graph[GRAPH_VERTICES][GRAPH_VERTICES]);
void graph_add_edge(int graph[GRAPH_VERTICES][GRAPH_VERTICES],
                    int from, int to, int distance);
const char *graph_vertex_name(int vertex);

#ifdef __cplusplus
}
#endif
#endif

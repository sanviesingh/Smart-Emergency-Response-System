#include "graph.h"

void graph_init(int g[GRAPH_VERTICES][GRAPH_VERTICES]) {
    for (int i = 0; i < GRAPH_VERTICES; ++i)
        for (int j = 0; j < GRAPH_VERTICES; ++j)
            g[i][j] = (i == j) ? 0 : GRAPH_INF;

    /* Illustrative distances in kilometres for the academic demo network. */
    graph_add_edge(g, 0, 1, 4);
    graph_add_edge(g, 0, 2, 7);
    graph_add_edge(g, 1, 2, 2);
    graph_add_edge(g, 1, 3, 5);
    graph_add_edge(g, 2, 3, 3);
    graph_add_edge(g, 2, 4, 6);
    graph_add_edge(g, 3, 4, 2);
}

void graph_add_edge(int g[GRAPH_VERTICES][GRAPH_VERTICES],
                    int from, int to, int distance) {
    if (!g || from < 0 || to < 0 || from >= GRAPH_VERTICES ||
        to >= GRAPH_VERTICES || distance < 0) return;
    g[from][to] = distance;
    g[to][from] = distance;
}

const char *graph_vertex_name(int v) {
    static const char *names[GRAPH_VERTICES] = {
        "Control Room", "North Zone", "East Zone", "South Zone", "West Zone"
    };
    return (v >= 0 && v < GRAPH_VERTICES) ? names[v] : "Unknown";
}

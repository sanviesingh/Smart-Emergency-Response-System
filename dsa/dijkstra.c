#include "dijkstra.h"

static int find_min(const int dist[], const int visited[]) {
    int min = GRAPH_INF, index = -1;
    for (int i = 0; i < GRAPH_VERTICES; ++i) {
        if (!visited[i] && dist[i] < min) {
            min = dist[i];
            index = i;
        }
    }
    return index;
}

int dijkstra(const int graph[GRAPH_VERTICES][GRAPH_VERTICES],
             int source, int destination, int *distance,
             int path[GRAPH_VERTICES], int *path_length) {
    if (!graph || !distance || !path || !path_length ||
        source < 0 || source >= GRAPH_VERTICES ||
        destination < 0 || destination >= GRAPH_VERTICES) return 0;

    int dist[GRAPH_VERTICES], visited[GRAPH_VERTICES] = {0};
    int previous[GRAPH_VERTICES];
    for (int i = 0; i < GRAPH_VERTICES; ++i) {
        dist[i] = GRAPH_INF;
        previous[i] = -1;
    }
    dist[source] = 0;

    for (int count = 0; count < GRAPH_VERTICES; ++count) {
        int u = find_min(dist, visited);
        if (u == -1) break;
        visited[u] = 1;
        for (int v = 0; v < GRAPH_VERTICES; ++v) {
            if (!visited[v] && graph[u][v] < GRAPH_INF &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                previous[v] = u;
            }
        }
    }

    if (dist[destination] >= GRAPH_INF) return 0;
    int reverse[GRAPH_VERTICES], count = 0, current = destination;
    while (current != -1 && count < GRAPH_VERTICES) {
        reverse[count++] = current;
        if (current == source) break;
        current = previous[current];
    }
    if (count == 0 || reverse[count - 1] != source) return 0;

    *path_length = count;
    for (int i = 0; i < count; ++i) path[i] = reverse[count - 1 - i];
    *distance = dist[destination];
    return 1;
}

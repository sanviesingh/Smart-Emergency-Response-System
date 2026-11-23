#include "priority_queue.h"

void pq_init(PriorityQueue *pq) {
    if (pq) pq->size = 0;
}

int pq_enqueue(PriorityQueue *pq, const EmergencyRecord *item) {
    if (!pq || !item || pq->size >= PQ_CAPACITY) return 0;
    pq->items[pq->size++] = *item;
    return 1;
}

/* Highest severity first; for equal severity, earlier inserted item first. */
int pq_dequeue(PriorityQueue *pq, EmergencyRecord *out) {
    if (!pq || !out || pq->size == 0) return 0;
    int best = 0;
    for (int i = 1; i < pq->size; ++i) {
        if (pq->items[i].severity > pq->items[best].severity) best = i;
    }
    *out = pq->items[best];
    for (int i = best; i < pq->size - 1; ++i) pq->items[i] = pq->items[i + 1];
    --pq->size;
    return 1;
}

int pq_peek(const PriorityQueue *pq, EmergencyRecord *out) {
    if (!pq || !out || pq->size == 0) return 0;
    int best = 0;
    for (int i = 1; i < pq->size; ++i)
        if (pq->items[i].severity > pq->items[best].severity) best = i;
    *out = pq->items[best];
    return 1;
}

int pq_size(const PriorityQueue *pq) { return pq ? pq->size : 0; }
void pq_clear(PriorityQueue *pq) { if (pq) pq->size = 0; }

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include "emergency.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PQ_CAPACITY 200

typedef struct {
    EmergencyRecord items[PQ_CAPACITY];
    int size;
} PriorityQueue;

void pq_init(PriorityQueue *pq);
int pq_enqueue(PriorityQueue *pq, const EmergencyRecord *item);
int pq_dequeue(PriorityQueue *pq, EmergencyRecord *out);
int pq_peek(const PriorityQueue *pq, EmergencyRecord *out);
int pq_size(const PriorityQueue *pq);
void pq_clear(PriorityQueue *pq);

#ifdef __cplusplus
}
#endif
#endif

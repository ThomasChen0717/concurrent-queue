// queue_seq.c
#include "queue.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

// Internal representation: bounded circular buffer (sequential).
struct Queue {
    int *data;      // array of length capacity
    int capacity;   // maximum number of elements
    int head;       // index of next element to dequeue
    int tail;       // index of next slot to enqueue
    int size;       // current number of elements
};

Queue* create(int capacity) {
    // Edge case: capacity <= 0
    if (capacity <= 0) return NULL;

    // Allocate memory for the queue struct
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (!q) return NULL;

    // Allocate memory for the data array
    q->data = (int *)malloc(sizeof(int) * capacity);
    if (!q->data) {
        free(q);
        return NULL;
    }

    // Initialize fields
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->size = 0;

    return q;
}

void destroy(Queue *q) {
    //Edge case: q is NULL
    if (!q) return;
    //Free the data array
    free(q->data);
    //Free the queue struct
    free(q);
}

bool enqueue(Queue *q, int value) {
    //Edge case: q is NULL
    if (!q) return false;

    // Queue is full
    if (q->size == q->capacity) {
        return false;
    }

    // Enqueue the value at tail
    q->data[q->tail] = value;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;

    return true;
}

bool dequeue(Queue *q, int *out) {
    //Edge case: q or out is NULL
    if (!q || !out) return false;

    // Queue is empty
    if (q->size == 0) {
        return false;
    }
    // Dequeue the value at head
    *out = q->data[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size--;

    return true;
}

bool is_empty(const Queue *q) {
    if (!q) return true;
    return (q->size == 0);
}

bool is_full(const Queue *q) {
    if (!q) return false;
    return (q->size == q->capacity);
}

int size(const Queue *q) {
    if (!q) return 0;
    return q->size;
}

int capacity(const Queue *q) {
    if (!q) return 0;
    return q->capacity;
}

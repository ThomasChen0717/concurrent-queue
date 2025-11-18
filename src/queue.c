//queue.c 
#include "queue.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <omp.h>

// Internal representation: bounded circular buffer.
struct Queue {
    int *data;      // array of length capacity
    int capacity;   // maximum number of elements
    int head;       // index of next element to dequeue
    int tail;       // index of next slot to enqueue
    int size;       // current number of elements
    omp_lock_t head_lock;   // protects head movement
    omp_lock_t tail_lock;   // protects tail movement
};


Queue* create(int capacity) {
    //Edge case: capacity <= 0
    if (capacity <= 0) return NULL;

    //Allocate memory for the queue struct
    Queue *q = (Queue *)malloc(sizeof(Queue));
    //Edge case: malloc fails
    if(!q) return NULL;

    //Allocate memory for the data array
    q->data = (int *)malloc(sizeof(int) * capacity);
    //Edge case: malloc fails
    if (!q->data) {
        free(q);
        return NULL;
    }

    //Initialize the queue fields
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->size = 0;

    // Initialize locks
    omp_init_lock(&q->head_lock);
    omp_init_lock(&q->tail_lock);

    return q;
}

void destroy(Queue *q) {
    //Edge case: q is NULL
    if (!q) return;

    // Destroy locks (caller must ensure no one is using q anymore)
    omp_destroy_lock(&q->head_lock);
    omp_destroy_lock(&q->tail_lock);

    //Free the data array
    free(q->data);
    //Free the queue struct
    free(q);
}

bool enqueue(Queue *q, int value) {
    //Edge case: q is NULL
    if (!q) return false;

    // Acquire the tail lock
    omp_set_lock(&q->tail_lock);

    int s;
    #pragma omp atomic read
    s = q->size;

    // Queue is full
    if (s == q->capacity) {
        omp_unset_lock(&q->tail_lock);
        return false; 
    }

    // Enqueue the value at tail
    q->data[q->tail] = value;
    q->tail = (q->tail + 1) % q->capacity;

    // Increment size atomically
    #pragma omp atomic update
    q->size++;

    //Release the tail lock
    omp_unset_lock(&q->tail_lock);
    return true;
}

bool dequeue(Queue *q, int *out) {
    //Edge case: q or out is NULL
    if (!q || !out) return false;

    // Acquire the head lock
    omp_set_lock(&q->head_lock);

    // Check if queue is empty (read size atomically)
    int s;
    #pragma omp atomic read
    s = q->size;

    // Queue is empty
    if (s == 0) {
        omp_unset_lock(&q->head_lock);
        return false; 
    }

   // Dequeue the value at head
    *out = q->data[q->head];
    q->head = (q->head + 1) % q->capacity;

    // Update size atomically
    #pragma omp atomic update
    q->size--;
    //Release the head lock
    omp_unset_lock(&q->head_lock);
    return true;
}

bool is_empty(const Queue *q) {
    if (!q) return true;

    int s;
    
    #pragma omp atomic read
    s = q->size;
    return (s == 0);
}

bool is_full(const Queue *q) {
    if (!q) return false;

    int s;
    #pragma omp atomic read
    s = q->size;
    return (s == q->capacity);
}

int size(const Queue *q) {
    if (!q) return 0;

    int s;
    #pragma omp atomic read
    s = q->size;
    return s;
}

int capacity(const Queue *q) {
    if (!q) return 0;
    // capacity is immutable after creation, so no lock/atomic needed
    return q->capacity;
}
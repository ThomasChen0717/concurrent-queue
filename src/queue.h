//queue.h
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct Queue Queue; 


/**
 * Create a new queue with given capacity.
 * Returns NULL on failure or if capacity <= 0.
 */
Queue* create(int capacity);

/**
 * Free all memory associated with the queue.
 * Safe to call with NULL (no-op).
 */
void destroy(Queue *q);

/**
 * Enqueue a value.
 * Returns true on success, false if the queue is full or q is NULL.
 */
bool enqueue(Queue *q, int value);

/**
 * Dequeue a value into *out.
 * Returns true on success, false if the queue is empty or q/out is NULL.
 */
bool dequeue(Queue *q, int *out);

/**
 * Returns true if the queue is empty.
 * If q is NULL, returns true.
 */
bool is_empty(const Queue *q);

/**
 * Returns true if the queue is full.
 * If q is NULL, returns false.
 */
bool is_full(const Queue *q);

/**
 * Current number of elements in the queue.
 * If q is NULL, returns 0.
 */
int size(const Queue *q);

/**
 * Maximum number of elements that can be stored.
 * If q is NULL, returns 0.
 */
int capacity(const Queue *q);

#endif // QUEUE_H


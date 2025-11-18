//queue.c 
#include "queue.h" 
#include <stdlib.h> 
#include <stddef.h> 
#include <stdbool.h> 
#include <omp.h> 
// Internal representation: bounded circular buffer. 
struct Queue { 
    int *data; // array of length capacity 
    int capacity; // maximum number of elements 
    int head; // index of next element to dequeue 
    int tail; // index of next slot to enqueue 
    int size; // current number of elements 
    omp_lock_t lock; 
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
    if (!q->data) { free(q); return NULL; } 
    //Initialize the queue fields 
    q->capacity = capacity; 
    q->head = 0; 
    q->tail = 0; 
    q->size = 0; 
    omp_init_lock(&q->lock); 
    
    return q; 
} 
    
void destroy(Queue *q) {
    //Edge case: q is NULL
    if (!q) return;
    
    // Destroy lock (caller must ensure no one is using q anymore)
    omp_destroy_lock(&q->lock);
    //Free the data array
    free(q->data);
    //Free the queue struct
    free(q);
}

bool enqueue(Queue *q, int value) {
    //Edge case: q is NULL
    if (!q) return false;
    //Acquire the lock
    omp_set_lock(&q->lock);
    //Edge case: queue is full
    if (q->size == q->capacity) {
        omp_unset_lock(&q->lock);
        return false;
    }
    //Enqueue the value
    q->data[q->tail] = value;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
    //Release the lock
    omp_unset_lock(&q->lock);
    return true;
}

bool dequeue(Queue *q, int *out) {
    //Edge case: q or out is NULL
    if (!q || !out) return false;
    //Acquire the lock
    omp_set_lock(&q->lock);
    //Edge case: queue is empty
    if (q->size == 0) {
        omp_unset_lock(&q->lock);
        return false;
    }
    //Dequeue the value
    *out = q->data[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size--;
    //Release the lock
    omp_unset_lock(&q->lock);
    return true;
}

bool is_empty(const Queue *q) {
    //Edge case: q is NULL
    if (!q) return true;
    //Acquire the lock
    omp_set_lock((omp_lock_t *)&q->lock);
    int r = (q->size == 0);
    //Release the lock
    omp_unset_lock((omp_lock_t *)&q->lock);
    return r;
}

bool is_full(const Queue *q) {
    //Edge case: q is NULL
    if (!q) return false;
    //Acquire the lock
    omp_set_lock((omp_lock_t *)&q->lock);
    int r = (q->size == q->capacity);
    //Release the lock
    omp_unset_lock((omp_lock_t *)&q->lock);
    return r;
}

int size(const Queue *q) {
    //Edge case: q is NULL
    if (!q) return 0;
    //Acquire the lock
    omp_set_lock((omp_lock_t *)&q->lock);
    int r = q->size;
    //Release the lock
    omp_unset_lock((omp_lock_t *)&q->lock);
    return r;
}

int capacity(const Queue *q) {
    //Edge case: q is NULL
    if (!q) return 0;
    return q->capacity;
}
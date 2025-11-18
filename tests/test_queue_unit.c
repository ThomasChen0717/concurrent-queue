// tests/test_queue_unit.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "queue.h"   

static void test_create_destroy(void) {
    Queue *q = create(10);
    assert(q != NULL);
    assert(capacity(q) == 10);
    assert(size(q) == 0);
    assert(is_empty(q));
    assert(!is_full(q));
    destroy(q);

    // Edge cases
    assert(create(0) == NULL);
    assert(create(-5) == NULL);
}

static void test_enqueue_dequeue_basic(void) {
    Queue *q = create(3);
    assert(q != NULL);

    int v;

    // Queue initially empty
    assert(is_empty(q));
    assert(size(q) == 0);

    // Enqueue some items
    assert(enqueue(q, 1));
    assert(enqueue(q, 2));
    assert(enqueue(q, 3));
    assert(is_full(q));
    assert(size(q) == 3);

    // No more room
    assert(!enqueue(q, 4));

    // Dequeue in FIFO order
    assert(dequeue(q, &v) && v == 1);
    assert(dequeue(q, &v) && v == 2);
    assert(dequeue(q, &v) && v == 3);
    assert(is_empty(q));
    assert(size(q) == 0);

    // Dequeue from empty
    assert(!dequeue(q, &v));

    destroy(q);
}

static void test_wraparound(void) {
    Queue *q = create(3);
    assert(q != NULL);

    int v;

    // Fill: [10,20,30]
    assert(enqueue(q, 10));
    assert(enqueue(q, 20));
    assert(enqueue(q, 30));
    assert(is_full(q));

    // Pop one (10), push another (40) -> forces wraparound
    assert(dequeue(q, &v) && v == 10);
    assert(enqueue(q, 40));
    assert(is_full(q));

    // Now queue should contain [20,30,40] in that order
    assert(dequeue(q, &v) && v == 20);
    assert(dequeue(q, &v) && v == 30);
    assert(dequeue(q, &v) && v == 40);
    assert(is_empty(q));

    destroy(q);
}

static void test_null_arguments(void) {
    int v;
    // All should be safe (return false / 0, not crash)
    assert(!enqueue(NULL, 1));
    assert(!dequeue(NULL, &v));
    assert(!dequeue(NULL, NULL));
    assert(is_empty(NULL));
    assert(!is_full(NULL));
    assert(size(NULL) == 0);
    assert(capacity(NULL) == 0);
}

int main(void) {
    printf("Running unit tests...\n");

    test_create_destroy();
    test_enqueue_dequeue_basic();
    test_wraparound();
    test_null_arguments();

    printf("All unit tests PASSED.\n");
    return 0;
}

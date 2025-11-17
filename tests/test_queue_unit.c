#include <assert.h>
#include <stddef.h>
#include "queue.h"

int main(void) {
    assert(create(0) == NULL);
    assert(create(-1) == NULL);

    Queue *q = create(4);
    assert(q);
    assert(capacity(q) == 4);
    assert(is_empty(q));

    int x;
    assert(enqueue(q, 1));
    assert(enqueue(q, 2));
    assert(enqueue(q, 3));
    assert(enqueue(q, 4));
    assert(is_full(q));
    assert(!enqueue(q, 5)); 

    assert(dequeue(q, &x) && x == 1);
    assert(dequeue(q, &x) && x == 2);
    assert(dequeue(q, &x) && x == 3);
    assert(dequeue(q, &x) && x == 4);
    assert(is_empty(q));
    assert(!dequeue(q, &x));

    destroy(q);
    assert(size(NULL) == 0);
    return 0;
}

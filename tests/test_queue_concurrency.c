#include <assert.h>
#include <stdatomic.h>
#include <omp.h>
#include "queue.h"


int main(void) {
    int P = 4, C = 4, items = 10000, cap = 128;
    Queue *q = create(cap);
    atomic_int produced = 0, consumed = 0;
    int total = P * items;

    #pragma omp parallel num_threads(P + C) shared(q)
    {
        int tid = omp_get_thread_num();
        if (tid < P) {
            for (int i = 0; i < items; i++) {
                while (!enqueue(q, i)) { /* spin until space */ }
                atomic_fetch_add(&produced, 1);
            }
        } else {
            int v;
            while (atomic_load(&consumed) < total) {
                if (dequeue(q, &v)) {
                    atomic_fetch_add(&consumed, 1);
                }
            }
        }
    }

    assert(atomic_load(&produced) == total);
    assert(atomic_load(&consumed) == total);
    assert(is_empty(q));
    destroy(q);
    return 0;
}

// tests/test_queue_concurrency.c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <omp.h>

#include "queue.h"

// A simple multi-producer / multi-consumer test.
// Not a formal proof of correctness, but it will catch a lot of bugs.
static void test_mp_mc(int cap, int P, int C, int items_per_prod) {
    Queue *q = create(cap);
    assert(q != NULL);

    int total_items = P * items_per_prod;
    int consumed_total = 0;

    // Optional: a simple checksum to see if anything goes badly wrong.
    long long sum_enq = 0;
    long long sum_deq = 0;

    // We’ll assign each producer a disjoint range of values:
    // producer k enqueues values [k*items_per_prod .. (k+1)*items_per_prod-1]
    #pragma omp parallel num_threads(P + C) shared(q, consumed_total, sum_enq, sum_deq)
    {
        int tid = omp_get_thread_num();

        if (tid < P) {
            int base = tid * items_per_prod;
            for (int i = 0; i < items_per_prod; i++) {
                int val = base + i;
                // Spin until enqueue succeeds
                while (!enqueue(q, val)) { /* busy-wait */ }

                #pragma omp atomic
                sum_enq += val;
            }
        } else {
            int v;
            while (1) {
                int c;
                #pragma omp atomic read
                c = consumed_total;
                if (c >= total_items) break;

                if (dequeue(q, &v)) {
                    #pragma omp atomic
                    consumed_total++;

                    #pragma omp atomic
                    sum_deq += v;
                }
            }
        }
    }

    // After all threads finish:
    assert(consumed_total == total_items);
    assert(is_empty(q));
    assert(size(q) == 0);
    assert(sum_enq == sum_deq);

    destroy(q);

    printf("  [OK] cap=%d P=%d C=%d items=%d\n",
           cap, P, C, items_per_prod);
}

int main(void) {
    printf("Running concurrency tests...\n");

    int caps[] = {8, 64, 256};
    int pcs[]  = {1, 2, 4, 8};
    int items  = 1000;

    for (int ci = 0; ci < (int)(sizeof(caps)/sizeof(caps[0])); ++ci) {
        for (int i = 0; i < (int)(sizeof(pcs)/sizeof(pcs[0])); ++i) {
            int cap = caps[ci];
            int P = pcs[i];
            int C = pcs[i];   // same number of consumers
            test_mp_mc(cap, P, C, items);
        }
    }

    printf("All concurrency tests PASSED.\n");
    return 0;
}

#include <stdio.h>
#include "queue.h"
#include <omp.h>

int main(void) {
    int cap = 128;
    Queue *q = create(cap);

    int producers = 4;
    int consumers = 4;
    int items_per_producer = 1000;
    int total = producers * items_per_producer;
    int consumed = 0;

    #pragma omp parallel num_threads(producers + consumers) shared(q, consumed, total, producers, items_per_producer)
    {
        int tid = omp_get_thread_num();
        if (tid < producers) {
            for (int i = 0; i < items_per_producer; i++) {
                int val = tid * items_per_producer + i;
                while (!enqueue(q, val)) {}
            }
        } else {
            int x;
            while (1) {
                int current;
                #pragma omp atomic read
                current = consumed;
                if (current >= total) break;
                if (dequeue(q, &x)) {
                    #pragma omp atomic
                    consumed++;
                }
            }
        }
    }

    printf("produced: %d\n", total);
    printf("consumed: %d\n", consumed);
    printf("empty? %d\n", is_empty(q));

    destroy(q);
    return 0;
}

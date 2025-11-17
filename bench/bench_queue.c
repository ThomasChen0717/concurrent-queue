#include <stdio.h>
#include <omp.h>
#include "queue.h"


static double run_once(int cap, int P, int C, int items) {
    Queue *q = create(cap);
    int total = P * items;
    int consumed = 0;
    double t0 = omp_get_wtime();

    #pragma omp parallel num_threads(P + C) shared(q, consumed)
    {
        int tid = omp_get_thread_num();
        if (tid < P) {
            for (int i = 0; i < items; i++) {
                while (!enqueue(q, i)) { }
            }
        } else {
            int v;
            while (1) {
                int c;
                #pragma omp atomic read
                c = consumed;
                if (c >= total) break;
                if (dequeue(q, &v)) {
                    #pragma omp atomic
                    consumed++;
                }
            }
        }
    }

    double t1 = omp_get_wtime();
    destroy(q);
    return t1 - t0;
}

int main(void) {
    int caps[] = {64, 256, 1024};
    int pc[] = {1, 2, 4, 8};
    int items = 100000;

    printf("impl,cap,P,C,items,time_s,throughput_ops_per_s\n");
    for (int ci = 0; ci < 3; ++ci)
        for (int i = 0; i < 4; ++i) {
            int cap = caps[ci], P = pc[i], C = pc[i];
            double sec = run_once(cap, P, C, items);
            double ops = (double)(2LL * (long long)P * items) / sec; // enq+deq
            printf("%s,%d,%d,%d,%d,%.6f,%.1f\n",
                   "default", cap, P, C, items, sec, ops);
        }
    return 0;
}

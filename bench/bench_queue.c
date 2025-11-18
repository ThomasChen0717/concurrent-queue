#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // for strcmp
#include <omp.h>
#include "queue.h"
#include "utils.c"

// Allow the implementation name to be injected at compile time.
// Example:
//   gcc -O2 -fopenmp -DIMPL_NAME=\"twolock\" -o bench_queue bench_queue.c queue.c
#ifndef IMPL_NAME
#define IMPL_NAME "default"
#endif

// How many times to repeat each (cap, P, C, items) configuration
#ifndef N_TRIALS
#define N_TRIALS 5
#endif

#ifndef CSV_ONLY
#define USE_PRETTY_TABLE 1
#endif

// ---------------------------------------------------------------------
// Concurrent benchmark: multiple producers + consumers, OpenMP parallel
// ---------------------------------------------------------------------
static double run_once_concurrent(int cap, int P, int C, int items) {
    Queue *q = create(cap);
    int total = P * items;
    int consumed = 0;

    double t0 = omp_get_wtime();

    #pragma omp parallel num_threads(P + C) shared(q, consumed)
    {
        int tid = omp_get_thread_num();
        if (tid < P) {
            // producers: thread 0 .. P-1
            for (int i = 0; i < items; i++) {
                // busy-wait until enqueue succeeds
                while (!enqueue(q, i)) {
                    // spin
                }
            }
        } else {
            // consumers: thread P .. P+C-1
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

// ---------------------------------------------------------------------
// Sequential benchmark: no P/C, just enqueue all then dequeue all
// ---------------------------------------------------------------------
static double run_once_seq(int cap, int items) {
    Queue *q = create(cap);

    if (!q) {
        fprintf(stderr, "Failed to create queue (cap=%d)\n", cap);
        exit(1);
    }

    double t0 = omp_get_wtime();

    int v;
    for (int i = 0; i < items; i++) {
        // Enqueue one item
        while (!enqueue(q, i)) {
            // If cap >= 1, this should basically never spin,
            // but keep it here for safety.
        }

        // Dequeue one item
        while (!dequeue(q, &v)) {
            // Should succeed immediately right after enqueue.
        }
    }

    double t1 = omp_get_wtime();
    destroy(q);
    return t1 - t0;
}

// Helper: detect if we’re benchmarking the sequential implementation
static int is_sequential_impl(void) {
    return strcmp(IMPL_NAME, "seq") == 0 ||
           strcmp(IMPL_NAME, "sequential") == 0;
}

int main(void) {
    int caps[] = {64, 256, 1024};
    int pc[]   = {1, 2, 4, 8};
    int items  = 100000;

    // Print CSV Header
    printf("impl,cap,P,C,items,trials,"
           "time_avg_s,time_min_s,time_max_s,"
           "throughput_avg_ops_per_s\n");


    if (is_sequential_impl()) {
        #ifdef USE_PRETTY_TABLE
            print_header_seq();
        #endif
        // -------------------------------------------------------------
        // SEQUENTIAL: only vary capacity, ignore P and C
        // -------------------------------------------------------------
        for (int ci = 0; ci < (int)(sizeof(caps)/sizeof(caps[0])); ++ci) {
            int cap = caps[ci];

            double sum = 0.0;
            double tmin = 1e300;
            double tmax = 0.0;

            for (int t = 0; t < N_TRIALS; ++t) {
                double sec = run_once_seq(cap, items);
                sum += sec;
                if (sec < tmin) tmin = sec;
                if (sec > tmax) tmax = sec;
            }

            double avg = sum / (double)N_TRIALS;

            // total operations: items enqueued + items dequeued
            double total_ops = 2.0 * (double)items;
            double throughput_avg = total_ops / avg;

            #ifdef USE_PRETTY_TABLE
                print_row_seq(IMPL_NAME, cap, items,
                      N_TRIALS, avg, tmin, tmax, throughput_avg);
            #else 
                printf("%s,%d,%d,%d,%d,%d,%.6f,%.6f,%.6f,%.1f\n",
                       IMPL_NAME, cap, 0, 0, items, N_TRIALS,
                       avg, tmin, tmax, throughput_avg);
            #endif
        }
    } else {
        // -------------------------------------------------------------
        // CONCURRENT: vary capacity and P/C
        // -------------------------------------------------------------
        #ifdef USE_PRETTY_TABLE
            print_header();
        #endif
        for (int ci = 0; ci < (int)(sizeof(caps)/sizeof(caps[0])); ++ci) {
            for (int i = 0; i < (int)(sizeof(pc)/sizeof(pc[0])); ++i) {
                int cap = caps[ci];
                int P = pc[i];
                int C = pc[i];

                double sum = 0.0;
                double tmin = 1e300;
                double tmax = 0.0;

                for (int t = 0; t < N_TRIALS; ++t) {
                    double sec = run_once_concurrent(cap, P, C, items);
                    sum += sec;
                    if (sec < tmin) tmin = sec;
                    if (sec > tmax) tmax = sec;
                }

                double avg = sum / (double)N_TRIALS;

                // total operations: each produced item is enqueued and dequeued
                double total_ops = 2.0 * (double)P * (double)items;
                double throughput_avg = total_ops / avg;

                #ifdef USE_PRETTY_TABLE
                    print_row(IMPL_NAME, cap, P, C, items,
                              N_TRIALS, avg, tmin, tmax, throughput_avg);
                #else 
                    printf("%s,%d,%d,%d,%d,%d,%.6f,%.6f,%.6f,%.1f\n",
                           IMPL_NAME, cap, P, C, items, N_TRIALS,
                           avg, tmin, tmax, throughput_avg);
                #endif
            }
        }
    }

    print_footer();
    return 0;
}

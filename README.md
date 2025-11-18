# Concurrent Queue (OpenMP)

Simple bounded circular queue in C with a thread-safe variant using OpenMP. The public API is kept minimal and prefix-free (`create`, `enqueue`, `dequeue`, `destroy`, `size`, `capacity`, `is_empty`, `is_full`).

## Layout
- `bench/` benchmark directory
  - `csv/` directory for CSV files
  - `imgs/` directory for images
  - `bench_queue.c` benchmark program
  - `plot_bench.py` Python script to plot benchmarks from CSV files
- `bin/` binary directory
- `src/` source directory
  - `src/queue.h` public API (opaque `Queue` type)
  - `src/queue.c` queue with two OpenMP locks for concurrency (one for enqueue, one for dequeue) and atomic operations for size
  - `src/queue_v1.c` version 1 implementation using a single lock for both enqueue and dequeue
  - `src/queue_seq.c` sequential implementation for reference and benchmarking
- `tests/` test directory
  - `tests/test_queue_concurrency.c` test program for `src/queue.c` and `src/queue_v1.c` with concurrency
  - `tests/test_queue_unit.c` test program for `src/queue_seq.c`
- `gitignore` file
- `Makefile` build and run helpers
- `README.md` this file

## Build & Run
- Run tests
  - `make test` Defaults to `src/queue.c`
  - `make test MODE=one` Runs tests for `src/queue_v1.c`
  - `make test MODE=seq` Runs tests for `src/queue_seq.c`
- Run benchmarks
  - `make bench` Defaults to `src/queue.c`
  - `make bench MODE=one` Runs benchmarks for `src/queue_v1.c`
  - `make bench MODE=seq` Runs benchmarks for `src/queue_seq.c`
- Clean binaries:
  - `make clean`



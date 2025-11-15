# Concurrent Queue (OpenMP)

Simple bounded circular queue in C with a thread-safe variant using OpenMP. The public API is kept minimal and prefix-free (`create`, `enqueue`, `dequeue`, `destroy`, `size`, `capacity`, `is_empty`, `is_full`).

## Layout
- `src/queue.h` public API (opaque `Queue` type)
- `src/queue.c` queue with two OpenMP locks for concurrency (one for enqueue, one for dequeue) and atomic operations for size
- `src/queue_v1.c` version 1 implementation using a single lock for both enqueue and dequeue
- `src/main.c` demo program (multi-threaded producers/consumers using OpenMP)
- `Makefile` build and run helpers

## Requirements
- GCC with OpenMP support (`-fopenmp`) – MinGW-w64 on Windows works
- `make` installed (MSYS/MinGW ships a `make`)

## Build & Run
- Default run (builds and runs `src/queue.c`):
  - `make run`
- Run the v1 implementation (`src/queue_v1.c`):
  - `make run MODE=v1`
- Build all binaries:
  - `make all`
- Clean binaries:
  - `make clean`

## Manual Compile (without Makefile)
- Default implementation:
  - `gcc -std=c11 -Wall -O2 -fopenmp src/queue.c src/main.c -o test_queue`
  - `./test_queue`
- v1 implementation:
  - `gcc -std=c11 -Wall -O2 -fopenmp src/queue_v1.c src/main.c -o test_queue_v1`
  - `./test_queue_v1`

## Notes
- `capacity` is immutable after creation and read without locking.
CC := gcc
CFLAGS := -std=c11 -Wall -O2 -fopenmp
SRC_DIR := src

QUEUE_SRCS := $(SRC_DIR)/queue.c $(SRC_DIR)/main.c
QUEUE_V1_SRCS := $(SRC_DIR)/queue_v1.c $(SRC_DIR)/main.c

BIN_QUEUE := test_queue
BIN_QUEUE_V1 := test_queue_v1

MODE ?=

ifeq ($(MODE),v1)
RUN_BIN := $(BIN_QUEUE_V1)
RUN_SRCS := $(QUEUE_V1_SRCS)
else
RUN_BIN := $(BIN_QUEUE)
RUN_SRCS := $(QUEUE_SRCS)
endif

.PHONY: run all clean run_queue run_queue_v1

# Default target: run the selected binary
run: $(RUN_BIN)
	./$(RUN_BIN)

all: $(BIN_QUEUE) $(BIN_QUEUE_V1)

$(BIN_QUEUE): $(QUEUE_SRCS)
	$(CC) $(CFLAGS) $(QUEUE_SRCS) -o $(BIN_QUEUE)

$(BIN_QUEUE_V1): $(QUEUE_V1_SRCS)
	$(CC) $(CFLAGS) $(QUEUE_V1_SRCS) -o $(BIN_QUEUE_V1)

run_queue: $(BIN_QUEUE)
	./$(BIN_QUEUE)

run_queue_v1: $(BIN_QUEUE_V1)
	./$(BIN_QUEUE_V1)

ANN_SRC_DIR := src
ANN_BIN_DIR := bin_ann
$(shell mkdir -p $(ANN_BIN_DIR))
ANN_MODE ?= default
ifeq ($(ANN_MODE),v1)
  ANN_SRC_Q := $(ANN_SRC_DIR)/queue_v1.c
  ANN_RUN_SRCS := $(ANN_SRC_DIR)/queue_v1.c $(ANN_SRC_DIR)/main.c
  ANN_RUN_BIN := $(ANN_BIN_DIR)/demo_v1
else
  ANN_SRC_Q := $(ANN_SRC_DIR)/queue.c
  ANN_RUN_SRCS := $(ANN_SRC_DIR)/queue.c $(ANN_SRC_DIR)/main.c
  ANN_RUN_BIN := $(ANN_BIN_DIR)/demo
endif
ANN_CC ?= $(CC)
ANN_CFLAGS ?= $(CFLAGS)
ANN_LDFLAGS ?= $(LDFLAGS)
ifeq ($(strip $(ANN_CFLAGS)),)
  ANN_CFLAGS := -std=c11 -Wall -O2 -fopenmp
endif
ifeq ($(strip $(ANN_LDFLAGS)),)
  ANN_LDFLAGS := -fopenmp
endif
.PHONY: ann-run ann-test ann-bench ann-bench_v1 ann-asan ann-tsan ann-coverage ann-clean

ann-run: $(ANN_RUN_BIN)
	./$(ANN_RUN_BIN)

$(ANN_RUN_BIN): $(ANN_RUN_SRCS) $(ANN_SRC_DIR)/queue.h
	$(ANN_CC) $(ANN_CFLAGS) $^ -o $@ $(ANN_LDFLAGS)

ann-test: $(ANN_BIN_DIR)/test_unit $(ANN_BIN_DIR)/test_conc
	./$(ANN_BIN_DIR)/test_unit
	./$(ANN_BIN_DIR)/test_conc

$(ANN_BIN_DIR)/test_unit: $(ANN_SRC_Q) tests/test_queue_unit.c $(ANN_SRC_DIR)/queue.h
	$(ANN_CC) $(ANN_CFLAGS) $^ -o $@ $(ANN_LDFLAGS)

$(ANN_BIN_DIR)/test_conc: $(ANN_SRC_Q) tests/test_queue_concurrency.c $(ANN_SRC_DIR)/queue.h
	$(ANN_CC) $(ANN_CFLAGS) $^ -o $@ $(ANN_LDFLAGS)

ann-bench: $(ANN_BIN_DIR)/bench_default
	./$(ANN_BIN_DIR)/bench_default | head -n 20

ann-bench_v1: $(ANN_BIN_DIR)/bench_v1
	./$(ANN_BIN_DIR)/bench_v1 | head -n 20

$(ANN_BIN_DIR)/bench_default: $(ANN_SRC_DIR)/queue.c bench/bench_queue.c $(ANN_SRC_DIR)/queue.h
	$(ANN_CC) $(ANN_CFLAGS) $^ -o $@ $(ANN_LDFLAGS)

$(ANN_BIN_DIR)/bench_v1: $(ANN_SRC_DIR)/queue_v1.c bench/bench_queue.c $(ANN_SRC_DIR)/queue.h
	$(ANN_CC) $(ANN_CFLAGS) $^ -o $@ $(ANN_LDFLAGS)

ann-asan:
	$(MAKE) ann-clean
	$(MAKE) ANN_CFLAGS="$(ANN_CFLAGS) -fsanitize=address -fno-omit-frame-pointer" ann-test

ann-tsan:
	$(MAKE) ann-clean
	$(MAKE) ANN_CFLAGS="$(ANN_CFLAGS) -fsanitize=thread -fno-omit-frame-pointer" ann-test

ann-coverage:
	$(MAKE) ann-clean
	$(MAKE) ANN_CFLAGS="$(ANN_CFLAGS) --coverage -O0" ANN_LDFLAGS="$(ANN_LDFLAGS) --coverage" ann-test

ann-clean:
	rm -rf $(ANN_BIN_DIR) *.gc* *.dSYM
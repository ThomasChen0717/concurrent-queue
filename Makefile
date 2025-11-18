CC      := gcc
BIN_DIR := bin

ONELOCK_SRC   := src/queue_v1.c
TWOLOCK_SRC   := src/queue.c
SEQ_SRC       := src/queue_seq.c
QUEUE_HDR     := src/queue.h

UNIT_TEST_SRC := tests/test_queue_unit.c
CONC_TEST_SRC := tests/test_queue_concurrency.c
BENCH_SRC     := bench/bench_queue.c

MODE ?= two

ifeq ($(MODE),two)
    IMPL_SRC := $(TWOLOCK_SRC)
    IMPL_NAME := twolock
else ifeq ($(MODE),one)
    IMPL_SRC := $(ONELOCK_SRC)
    IMPL_NAME := onelock
else ifeq ($(MODE),seq)
    IMPL_SRC := $(SEQ_SRC)
    IMPL_NAME := sequential
else
    $(error Unknown MODE '$(MODE)'; use MODE=two | MODE=one | MODE=seq)
endif

CFLAGS_BASE := -std=c11 -Wall -O2 -fopenmp -Isrc -DIMPL_NAME=\"$(IMPL_NAME)\"
CFLAGS := $(CFLAGS_BASE) $(CFLAGS_EXTRA)

UNIT_BIN := $(BIN_DIR)/test_unit_$(IMPL_NAME)
CONC_BIN := $(BIN_DIR)/test_conc_$(IMPL_NAME)
BENCH_BIN := $(BIN_DIR)/bench_$(IMPL_NAME)

ifeq ($(OS),Windows_NT)
    UNAME_S := Windows
    RM_DIR  := rmdir /s /q
    MKDIR_P := mkdir
else
    UNAME_S := $(shell uname)
    RM_DIR  := rm -rf
    MKDIR_P := mkdir -p
endif


# Ensure bin directory exists
$(BIN_DIR):
	$(MKDIR_P) $(BIN_DIR)

# ============================
# Build rules
# ============================
$(UNIT_BIN): $(BIN_DIR) $(IMPL_SRC) $(UNIT_TEST_SRC) $(QUEUE_HDR)
	$(CC) $(CFLAGS) $(IMPL_SRC) $(UNIT_TEST_SRC) -o $@

$(CONC_BIN): $(BIN_DIR) $(IMPL_SRC) $(CONC_TEST_SRC) $(QUEUE_HDR)
	$(CC) $(CFLAGS) $(IMPL_SRC) $(CONC_TEST_SRC) -o $@ -fopenmp

$(BENCH_BIN): $(BIN_DIR) $(IMPL_SRC) $(BENCH_SRC) $(QUEUE_HDR)
	$(CC) $(CFLAGS) $(IMPL_SRC) $(BENCH_SRC) -o $@ -fopenmp


# ============================
# Individual test targets
# ============================
.PHONY: test_unit test_conc

test_unit: $(UNIT_BIN)
	@echo "=== Running UNIT TEST ($(IMPL_NAME)) ==="
	./$(UNIT_BIN)

test_conc: $(CONC_BIN)
	@echo "=== Running CONCURRENCY TEST ($(IMPL_NAME)) ==="
	./$(CONC_BIN)


# ============================
# High-level "test" target
#   - seq  -> unit tests only
#   - one/two -> concurrency tests only
# ============================
.PHONY: test

ifeq ($(MODE),seq)
test: test_unit
else
test: test_conc
endif


# ============================
# Run benchmark
# ============================
.PHONY: bench
bench: $(BENCH_BIN)
	@echo "=== Running BENCHMARK ($(IMPL_NAME)) ==="
	./$(BENCH_BIN)

# ============================
# Clean
# ============================
.PHONY: clean
clean:
	@$(RM_DIR) $(BIN_DIR)
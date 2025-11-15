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

clean:
	rm -f $(BIN_QUEUE) $(BIN_QUEUE_V1)
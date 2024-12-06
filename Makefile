# Variables
CC = gcc
CFLAGS = -std=c99
SRC = demografics.c
OUT = demografics

# Default target (what make runs when no target is specified)
all: $(OUT)

# Rule to create the executable
$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

# Clean target to remove the executable
clean:
	rm -f $(OUT)

# Phony targets (non-file targets)
.PHONY: all clean

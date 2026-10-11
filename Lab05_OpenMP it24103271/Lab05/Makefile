CC = gcc
CFLAGS = -O2 -std=c11 -Wall -Wextra -Wpedantic -fopenmp
SOURCES = $(wildcard src/*.c)
BINS = $(patsubst src/%.c,bin/%,$(SOURCES))
all: $(BINS)
bin/%: src/%.c
	mkdir -p bin
	$(CC) $(CFLAGS) $< -o $@ -lm
clean:
	rm -rf bin
.PHONY: all clean

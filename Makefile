CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic

all: utils.o

debug: CFLAGS += -Og -g
debug: all

clean:
	$(RM) a.out $(wildcard *.o)

.PHONY: clean

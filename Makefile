CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic

all: utils.h utils.o

ansi: CFLAGS += -ansi
ansi: all

debug: CFLAGS += -Og -g
debug: all

clean:
	$(RM) a.out $(wildcard *.o)

.PHONY: clean

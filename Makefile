# OS/architecture conditions
# http://git.kernel.org/cgit/git/git.git/tree/Makefile?id=c965c029330b1f81cc107c5d829e7fd79c61d8ea

CC=gcc
CFLAGS=-Wall -Wextra -march=native
LDFLAGS=-s -lpthread

ifeq ($(CC), gcc)
	GCCVERSION := $(shell $(CC) -dumpversion|cut -d. -f1,2)
	HAS_OFAST := $(shell echo '$(GCCVERSION) >= 4.6' | bc)
	HAS_WPEDANTIC := $(shell echo '$(GCCVERSION) >= 4.8' | bc)
endif

ifeq ($(CC), clang)
	HAS_OFAST := 1
	HAS_WPEDANTIC := 1
endif

ifeq ($(HAS_WPEDANTIC), 1)
	CFLAGS += -Wpedantic
else
	CFLAGS += -pedantic
endif

ifeq ($(OS), SunOS)
	LDFLAGS += -lsocket -lnsl
endif

all: utils.h utils.o

ansi: CFLAGS += -ansi
ansi: all

debug: CFLAGS += -Og -g -ggdb -DDEBUG
debug: all

clean:
	$(RM) a.out $(wildcard *.o)

.PHONY: clean

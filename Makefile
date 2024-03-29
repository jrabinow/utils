# OS/architecture conditions
# http://git.kernel.org/cgit/git/git.git/tree/Makefile?id=c965c029330b1f81cc107c5d829e7fd79c61d8ea

CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-s -lpthread
OS:= $(shell uname)

ifeq ($(CC), gcc)
	GCCVERSION := $(shell $(CC) -dumpversion|cut -d. -f1,2)
	HAS_OFAST := $(shell echo '$(GCCVERSION) >= 4.6' | bc)
	HAS_WPEDANTIC := $(shell echo '$(GCCVERSION) >= 4.8' | bc)
	# Could we please stop with this type of masquerading? It's BS https://stackoverflow.com/questions/38499462/how-to-tell-clang-to-stop-pretending-to-be-other-compilers/55656789#comment98007763_55656789
	HAS_MARCH_NATIVE := $(shell echo "$$( $(CC) --version|grep clang|wc -c) == 0" | bc)
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

ifeq ($(HAS_MARCH_NATIVE), 1)
	CFLAGS += -march=native
endif

ifeq ($(OS), SunOS)
	LDFLAGS += -lsocket -lnsl
endif

ifdef MAIN
all: objfile $(addsuffix .o, $(MAIN))
endif

objfile: utils.h utils.o

ansi: CFLAGS += -ansi
ifdef MAIN
ansi: all
else
ansi: objfile
endif

debug: CFLAGS += -Og -g -ggdb -DDEBUG
debug: all

clean:
	$(RM) a.out $(wildcard *.o)

.PHONY: clean

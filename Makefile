CC=gcc
CFLAGS=-Wall

CFLAGS+=-flto -DBF_COLLECT_OPTIMIZE -DBF_LOOP_OPTIMIZE
LDFLAGS=-flto

ifeq "$(DEBUG)" "Y"
	CFLAGS+=-g -Og
else
	CFLAGS+=-s -Os -DNDEBUG
endif

ifneq "$(BF_PRGM_SIZE)" ""
	CFLAGS+=-DBF_PRGM_SIZE=$(BF_PRGM_SIZE)
endif

ifneq "$(BF_MEM_SIZE)" ""
	CFLAGS+=-DBF_MEM_SIZE=$(BF_MEM_SIZE)
endif

all: dump bf

bf: bf.o parse.o exec.o
	$(CC) $(LDFLAGS) -o $@ bf.o parse.o exec.o

dump: dump.o parse.o
	$(CC) $(LDFLAGS) -o $@ dump.o parse.o

parse.o: parse.c bf.h

bf.o: bf.c bf.h

exec.o: exec.c bf.h

dump.o: dump.c bf.h

clean:
	rm *.o

.PHONY: clean

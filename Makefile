help:
	@echo "small debug fast"

clean:
	rm -f sighandler.o
	rm -f libsighandler.so

INCDIR=include/
SRCDIR=src/
SRCS=$(wildcard $(SRCDIR)*.c)
CC=gcc
CFLAGS=-Wall -Wextra -Wfatal-errors -D_POSIX_C_SOURCE=200809L\
	-std=c11

IFLAGS=-I$(INCDIR)
TYPE=debug

debug:	CFLAGS += -g -Og
small:	CFLAGS += -flto -Os -s
small:	TYPE=small
fast:	CFLAGS += -O2
fast:	TYPE=fast

debug:	objs
small:	objs
fast:	objs

objs:	clean

objs:
	$(CC) -c -fPIC $(CFLAGS) $(IFLAGS) $(SRCS)
	$(CC) $(CFLAGS) -shared -o libsighandler.so sighandler.o

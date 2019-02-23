CFLAGS=-Iinclude -flto -Wall -Wextra  -Wfatal-errors -D_POSIX_C_SOURCE=200112L\
       -std=c11

run:
	$(info debug or optimized)

debug:	clean
debug:	CFLAGS+= -g
debug:	all
optimized:	clean
optimized:	CFLAGS+= -O2
optimized:	all
clean:
	rm -f *.o *.so
all:
	$(CC) $(CFLAGS) -fPIC -c src/sighandler.c
	$(CC) $(CFLAGS) -shared sighandler.o -o libsighandler.so

CFLAGS=-I../include -L../ -Wall -Wextra  -Wfatal-errors -std=c11

run:
	$(info debug or optimized)

debug:	clean
debug:	CFLAGS+= -g
debug:	all
optimized:	clean
optimized:	CFLAGS+= -O2
optimized:	all
clean:
	rm -f one_to_many many_to_many many_to_one
all:
	$(CC) $(CFLAGS) one_to_many.c -o one_to_many -lpthread -lsighandler
	$(CC) $(CFLAGS) many_to_one.c -o many_to_one -lpthread -lsighandler
	$(CC) $(CFLAGS) many_to_many.c -o many_to_many -lpthread -lsighandler

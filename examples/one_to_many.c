#include "examples.h"
#define FMT_PRNT "caught %d\n"
#include <stdlib.h>
#include <errno.h>
void many(int n)
{
	switch (n) {
	case SIGINT:
		paste(FMT_PRNT, SIGINT);
		break;
	case SIGQUIT:
		paste(FMT_PRNT, SIGQUIT);
		break;
	case SIGUSR2:
		paste(FMT_PRNT, SIGUSR2);
		signal_cond();
		break;
	}
}

int main(int UNUSED argc, char UNUSED **argv)
{
	char *err = "reg_sig(SIGINT)";
	if (!reg_sig(SIGINT, many, 0, 0))
		goto return0;

	err = "reg_sig(SIGQUIT)";
	if (!reg_sig(SIGQUIT, many, 0, 0))
		goto return0;

	err = "reg_sig(SIGUSR2)";
	if (!reg_sig(SIGUSR2, many, 0, 0))
		goto return0;

	err = "waiting for SIGUSR2";
	paste(FMT_STR, "i am waiting for SIGUSR2");
	errno = 0;
	wait_on_cond();
	return0:
		fprintf(stderr, "%s %s\n\n", err, strerror(errno));
		return 0;
}

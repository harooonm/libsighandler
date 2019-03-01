#include "examples.h"
#define FMT_PRNT "caught %d\n"
#include <stdlib.h>

void many(int n)
{
	switch(n) {
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
	char *err ="reg_handler(SIGINT)";
	if (!reg_handler(SIGINT, many))	
		goto perror_exit;

	err = "reg_handler(SIGQUIT)";
	if (!reg_handler(SIGQUIT, many))	
		goto perror_exit;

	err = "reg_handler(SIGUSR2)";
	if (!reg_handler(SIGUSR2, many))	
		goto perror_exit;

	err = "waiting for SIGUSR2";
	paste(FMT_STR, "i am waiting for SIGUSR2");
	wait_on_cond();
perror_exit:
	perror(err);
	return errno;
}

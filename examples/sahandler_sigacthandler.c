#include "examples.h"
#include <stdio.h>

void act_handler(int num, siginfo_t *info , void *e)
{
	paste("caught in act_handler %d  sending pid = %d \n",
			info->si_signo, info->si_pid);
	signal_cond();
}

int main()
{
	if (!reg_sigaction(SIGINT, act_handler, 0, SA_SIGINFO)) {
		paste(FMT_STR, "error in registering sigint");
		return 0;
	}

	puts("waiting for ctrl+c");
	wait_on_cond();
	unreg_sigaction(SIGINT, act_handler);
	return 0;
}

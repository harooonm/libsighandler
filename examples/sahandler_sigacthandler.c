#include "examples.h"
#include <stdio.h>


/*XXX:this is an insance hacky example
 * noramlly when you register sigaction and sahandler on the same signal
 * linux only calls the sigaction handler
 * but using this library you can call both :p
 * */

void act_handler(int num, siginfo_t *info , void *e)
{
	paste("caught in act_handler %d  sending pid = %d \n",
			info->si_signo, info->si_pid);
	signal_cond();
}


void sig_handler(int num)
{
	paste(FMT_STR, "caught sigint sig_handler ");
}

int main()
{
	if (!reg_sigaction(SIGINT, act_handler, 0, SA_SIGINFO)) {
		paste(FMT_STR, "error in registering sigint");
		return 0;
	}

	if (!reg_sig(SIGINT, sig_handler, 0, 0)) {
			paste(FMT_STR, "error in registering sigint");
			return 0;
	}
	puts("waiting for ctrl+c");
	wait_on_cond();
	return 0;
}

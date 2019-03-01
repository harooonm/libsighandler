#include "examples.h"

void catch_int(int UNUSED n)
{
	paste(FMT_STR, "i am interrupted");
}

void catch_quit(int UNUSED n)
{
	paste(FMT_STR, "i am quited");
}

void catch_abrt(int UNUSED n)
{
	paste(FMT_STR, "i am aborted");
	signal_cond();
}


int main(int UNUSED argc, char UNUSED **argv)
{
	char *err ="reg_handler(SIGINT)";
	if (!reg_handler(SIGINT, catch_int))	
		goto perror_exit;
	
	err = "reg_handler(SIGQUIT)";
	if (!reg_handler(SIGQUIT, catch_quit))	
		goto perror_exit;

	err = "reg_handler(SIGABRT)";
	if (!reg_handler(SIGABRT, catch_abrt))	
		goto perror_exit;

	err = "waiting for abort";
	paste(FMT_STR, "i am waiting for SIGABRT");
	wait_on_cond();
perror_exit:
	perror(err);
	return errno;
}

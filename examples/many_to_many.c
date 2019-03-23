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
	char *err ="reg_sig(SIGINT)";
	if (!reg_sig(SIGINT, catch_int, 0, 0))
		goto return0;
	
	err = "reg_sig(SIGQUIT)";
	if (!reg_sig(SIGQUIT, catch_quit, 0, 0))
		goto return0;

	err = "reg_sig(SIGABRT)";
	if (!reg_sig(SIGABRT, catch_abrt, 0, 0))
		goto return0;

	err = "waiting for abort";
	paste(FMT_STR, "i am waiting for SIGABRT");
	wait_on_cond();

	return0:
		return 0;
}

#include "examples.h"

void one(int UNUSED n)
{
	paste(FMT_STR, "i am one");
}

void two(int UNUSED n)
{
	paste(FMT_STR, "i am two");
}

void three(int UNUSED n)
{
	paste(FMT_STR, "i am three");
	signal_cond();
}


int main(int UNUSED argc, char UNUSED **argv)
{
	char *err ="reg_handler(one)";
	if (!reg_handler(SIGINT, one))	
		goto perror_exit;
	
	err = "reg_handler(two)";
	if (!reg_handler(SIGINT, two))	
		goto perror_exit;

	err = "reg_handler(three)";
	if (!reg_handler(SIGINT, three))	
		goto perror_exit;

	err = "waiting for three";
	paste(FMT_STR, "i am waiting for three to be called press CTRL + C");
	wait_on_cond();
perror_exit:
	perror(err);
	return errno;
}

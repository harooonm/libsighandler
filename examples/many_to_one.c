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
	char *err ="reg_sig(one)";
	if (!reg_sig(SIGINT, one, 0, 0))
		goto return0;

	err = "reg_sig(two)";
	if (!reg_sig(SIGINT, two, 0, 0))
		goto return0;

	err = "reg_sig(three)";
	if (!reg_sig(SIGINT, three, 0, 0))
		goto return0;

	err = "waiting for three";
	paste(FMT_STR, "i am waiting for three to be called press CTRL + C");
	errno = 0;
	wait_on_cond();

	return0:
		fprintf(stderr, "%s %s\n", err, strerror(errno));
		return 0;
}

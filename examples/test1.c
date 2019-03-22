#include "sighandler.h"
void s(int n)
{

}
int main(void)
{
	reg_sa_handler(0, s, 0, 0);
	unreg_sa_handler(0, s);
	return 0;
}

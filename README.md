# libsighandler
A shared library that C applications can use for signal handling.
Usually in C when an application needs to do signal handling , 
sigaction is used, and if you want to have multiple signals
you have to manage the lifecycle i.e registration and unregistration of the signal.
this library makes it simple.
one can just do the following

# Usage
```code C

#include "signalhandler.h"

void onsig(int num)
{
    do_something();
}

int main (int argc, char **argv)
{
    reg_handler(SIGNUM, onsig);
    unreg_handler(SIGNUM, onsig);//not necessary but good to do.
    return 0;
}
```
# Purpose
The benefit with this library is that you can have multiple handlers for the same signal
or you can have the same handler attached to multiple signals.

the library takes care of calling them in sequence and registration and unregistration.

if you happen to use it please feel free to critique , report (portability or otherwise ) bugs  to me at maqsood3525@live.com
i would be more than willing to fix or help you.

if you find a memory leak please do drop me an email.

the goal is to get my code review and testing done by real people other than my self :).

# Build:
In some directory.
for Release build
	run cmake -DCMAKE_BUILD_TYPE=Release .
for Debug build
	run cmake -DCMAKE_BUILD_TYPE=Debug .
#Usage:
    see above, or examples

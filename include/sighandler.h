#ifndef INCLUDE_SIGHANDLER_H_
#define INCLUDE_SIGHANDLER_H_

#include <signal.h>

extern int _reg_sig(int sig_nr, const void(*sahandler)(int),
		const void(*sigact_handler)(int, siginfo_t *, void *),
		int blck_mask, int flags);
extern void _unreg_sig(int sig_num, const void(*sahandler)(int),
		const void(*sigact_handler)(int, siginfo_t *, void *));


#define reg_sig(num, callback, mask, flags)   _reg_sig(num, callback, NULL, mask, flags)

#define reg_sigaction(num, callback, mask, flags)   _reg_sig(num, NULL, callback, mask, flags)

#define unreg_sig(num, callback)   _unreg_sig(num, callback, NULL)

#define unreg_sigaction(num, callback)   _unreg_sig(num, NULL, callback)


#endif

#ifndef INCLUDE_SIGHANDLER_H_
#define INCLUDE_SIGHANDLER_H_

extern int reg_sig(int sig_num, void(*sahandler)(int), int mask, int flags);
extern int reg_sigaction(int sig_num,
		void(*sigact_handler)(int, siginfo_t *, void *),
		int mask, int flags);

extern unreg_sig(int sig_num, void(*sahandler)(int));
extern unreg_sigaction(int sig_num, void(*sigact_handler)(int, siginfo_t *, void *));

#endif

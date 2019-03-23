#ifndef INCLUDE_SIGHANDLER_H_
#define INCLUDE_SIGHANDLER_H_

#include <signal.h>

extern int __attribute__((nonnull(2))) reg_sig(const int sig_num,
		const void(*sahandler)(int), const int mask, const int flags);

extern int __attribute__((nonnull(2))) reg_sigaction(const int sig_num,
		const void(*sigact_handler)(int, siginfo_t *, void *),
		const int mask, const int flags);

extern void __attribute__((nonnull(2))) unreg_sig(const int sig_num,
		const void(*sahandler)(int));

extern void __attribute__((nonnull(2))) unreg_sigaction(const int sig_num,
		const void(*sigact_handler)(int, siginfo_t *, void *));

#endif

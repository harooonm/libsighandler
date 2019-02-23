/*
 * sighandler.h
 *
 *  Created on: Feb 21, 2019
 *      Author: haroon
 */

#ifndef INCLUDE_SIGHANDLER_H_
#define INCLUDE_SIGHANDLER_H_

#include <signal.h>
#include <stdbool.h>

typedef void (*on_signal)(int);

extern bool reg_handler(int sig_num, on_signal callback) __attribute__((nonnull(2)));
extern void unreg_handler(int sig_num, on_signal handler) __attribute__((nonnull(2)));

#endif

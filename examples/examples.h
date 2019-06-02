#ifndef __EXAMPLES_H_
#define __EXAMPLES_H_

#include "sighandler.h"
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define FMT_STR "%s\n"
#define paste(fmt,...) fprintf(stdout, fmt, __VA_ARGS__);
#define UNUSED __attribute__((unused))

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void wait_on_cond()
{
	pthread_mutex_lock(&mtx);
	pthread_cond_wait(&cond, &mtx);
	pthread_mutex_unlock(&mtx);

	pthread_mutex_destroy(&mtx);
	pthread_cond_destroy(&cond);
}

void signal_cond()
{
	pthread_mutex_lock(&mtx);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mtx);
}
#endif

#ifndef _Z_THREAD_
#define _Z_THREAD_

#include <pthread.h>
#include <stdlib.h>
typedef pthread_t thread_t;

thread_t thread_init_i(void (*fun)(int), int arg);

thread_t thread_init(void (*fun)());

#endif
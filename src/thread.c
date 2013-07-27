#include "thread.h"

struct thread_arg_noarg{
	void (*fun)(void*);
	void* arg;
};

void* start_thread(void* args){
	struct thread_arg_noarg* t_arg = (struct thread_arg_noarg*)args;
	t_arg->fun(t_arg->fun);

	pthread_exit(NULL);
}

thread_t thread_init(void (*fun)(void *)){
	pthread_t th;
	struct thread_arg_noarg t_arg;
	t_arg.fun = fun;
	t_arg.arg = NULL;
	pthread_create(&th, NULL, start_thread, (void*)&t_arg);
	return th;
}

#include "thread.h"

struct thread_arg_noarg{
	void (*fun)();
};

struct thread_arg_int{
	void (*fun)(int);
	int arg;
};


void* start_thread_noarg(void* args){
	struct thread_arg_noarg* t_arg = (struct thread_arg_noarg*)args;
	t_arg->fun(NULL);
	free(t_arg);
	pthread_exit(NULL);
}

void* start_thread_i(void* args){
	struct thread_arg_int* t_arg = (struct thread_arg_int*)args;
	t_arg->fun(t_arg->arg);
	pthread_exit(NULL);
}

thread_t thread_init_i(void (*fun)(int), int arg){
	pthread_t th;
	struct thread_arg_int t_arg;
	t_arg.fun = fun;
	t_arg.arg = arg;
	pthread_create(&th, NULL, start_thread_i, (void*)&t_arg);
	return th;
}

thread_t thread_init(void (*fun)()){
	pthread_t th;
	struct thread_arg_noarg* t_arg = (struct thread_arg_noarg*) malloc(sizeof(struct thread_arg_noarg));
	t_arg->fun = fun;
	pthread_create(&th, NULL, start_thread_noarg, (void*)t_arg);
	return th;
}
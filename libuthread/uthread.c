#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

#define STACK_SIZE  4096

#define STATE_BLOCKED 5
#define STATE_RUNNING 6
#define STATE_READY 7

queue_t thread_queue;
uthread_t current_thread_id = 1; //start execution from main thread
int thread_count = 1; //count the total number of threads in order to assign thread ids ???

typedef struct {
    uthread_t id;
    int state;
    uthread_ctx_t* context; //pretty sure backup registers are contained in context
	char stack[STACK_SIZE]; //TODO may not be char

} thread_control_block;


void uthread_yield(void)
{
	//from what I understand this involves swapping the thread context of current TCB with context of next TCB in the queue

}

uthread_t uthread_self(void)
{
	return 0;
}

int uthread_create(uthread_func_t func, void *arg)
{
	// TODO: only save main TCB on first call
	// Main TCB
	thread_control_block* mainTCB = (thread_control_block*) malloc(sizeof(thread_control_block));
	uthread_ctx_t* mainCXT = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
	mainTCB->context = mainCXT;
	mainTCB->id = thread_count;
	mainTCB->state = STATE_RUNNING;
	
	if(thread_queue == NULL){
		thread_queue = (queue_t)queue_create(); //will have a queue of thread control blocks
	}
	
	uthread_t tid = ++thread_count;
	thread_control_block* tcb = (thread_control_block*) malloc(sizeof(thread_control_block));
	uthread_ctx_t* cxt = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
	


	void* stack = uthread_ctx_alloc_stack(); //not sure if this is necessary
	int init_failed = uthread_ctx_init(cxt, stack, func, arg);

	if(init_failed == -1){
		 //TODO: the intializattion failed, not sure what to do from here
	}
	
	// Set thread control block
	tcb->context = cxt;
	tcb->id = tid;
	tcb->state = STATE_READY;

	queue_enqueue(thread_queue, (void*) tcb);
	
	// context switch [running the next thread]
	uthread_ctx_switch(mainCXT, cxt);

	return tid;
}

void uthread_exit(int retval)
{
	//end and free current thread context
	//set next thread context in queue as current context

}

int uthread_join(uthread_t tid, int *retval)
{
	//TODO: PHASE 3
	return 0;
}


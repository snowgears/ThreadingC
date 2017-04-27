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


typedef struct {
    uthread_t id;
    int state;
    uthread_ctx_t* context; //pretty sure backup registers are contained in context
    char stack[STACK_SIZE]; //TODO may not be char
    int index;

} thread_control_block;


// GLOBAL VARS
int cur_thread_index = 0;
queue_t thread_queue;
int thread_count = 0; //count the total number of threads in order to assign thread ids ???
thread_control_block* TCB_array[512];


/* Free all of the memory of a given thread control block */
void free_thread(thread_control_block* tcb)
{
        if(tcb == NULL){
                return;
        }
        //only free the context if it has allocated memory
        if(tcb->context != NULL){
                free(tcb->context);
        }
        free(tcb);
}



void uthread_yield(void)
{
    //from what I understand this involves swapping the thread context of current TCB with context of next TCB in the queue
    //swap contexts of current thread and next ready thread in queue
    if(thread_queue == NULL || queue_length(thread_queue) == 0){
        return;
    }

    // Get the next ready thread
    thread_control_block* popped = NULL;
    queue_dequeue(thread_queue, (void**)&popped);

    // Push current thread to ready queue
    queue_enqueue(thread_queue, TCB_array[cur_thread_index]);

    // Context switch
    int temp_index = cur_thread_index;
    cur_thread_index = popped->index;
    uthread_ctx_switch(TCB_array[temp_index]->context, (void*) popped->context);
}

uthread_t uthread_self(void)
{
    if(thread_count == 0){
        return 1;
    }
    return TCB_array[cur_thread_index]->id;
}

int uthread_create(uthread_func_t func, void *arg)
{
    //if this is the first call to create_thread(), initialze queue and save TCB of main to current TCB
    if(thread_queue == NULL){
        thread_queue = (queue_t)queue_create(); //will have a queue of thread control blocks

        thread_control_block* mainTCB = (thread_control_block*) malloc(sizeof(thread_control_block));
        uthread_ctx_t* mainCXT = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
        //save the context of the main thread
        getcontext(mainCXT);
        mainTCB->context = mainCXT;
        mainTCB->id = thread_count;
        mainTCB->state = STATE_RUNNING;
        mainTCB->index = cur_thread_index;
        // Add to array
        TCB_array[cur_thread_index] = mainTCB;

        //increment thread count since main was created
        thread_count++;
    }

    //initialize a new thread to execute the provided function with the given arguments
    //increment thread count to accomidate new thread being created
    thread_count++;
    thread_control_block* nextTCB = (thread_control_block*) malloc(sizeof(thread_control_block));
    uthread_ctx_t* cxt = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));

    void* stack = uthread_ctx_alloc_stack(); //not sure if this is necessary
    int init_failed = uthread_ctx_init(cxt, stack, func, arg);

    if(init_failed == -1){
         free_thread(nextTCB);
         return -1;
    }

    // Set thread control block
    nextTCB->context = cxt;
    nextTCB->id = thread_count;
    nextTCB->state = STATE_READY;
    nextTCB->index = thread_count;

    // Add to array
    TCB_array[thread_count] = nextTCB;

    // Push to queue
    queue_enqueue(thread_queue, (void*) nextTCB);

    // context switch [running the next thread]

    return nextTCB->id;
}

void uthread_exit(int retval)
{
    //end and free current thread context
    //set next thread context in queue as current context

}

int uthread_join(uthread_t tid, int *retval)
{
    while(1){
        if(queue_length(thread_queue) == 0){
            return 0;
        }
        uthread_yield();
        return 0;

    }
    return 0;
}

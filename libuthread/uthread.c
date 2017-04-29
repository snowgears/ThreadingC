#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

#define STACK_SIZE  4096

// THREAD STATES
#define STATE_BLOCKED 5
#define STATE_RUNNING 6
#define STATE_READY   7
#define STATE_ZOMBIE  8


typedef struct thread_control_block{
    uthread_t id;                        //the unqiue id of the TCB
    int state;                           //the state of the TCB
    uthread_ctx_t* context;              //contains information about function, arguments, and stack
    int index;                           //index of struct in the TCB_array (for retreiving later)
    struct thread_control_block* joined_thread; //the joined TCB if there is one
    int* ret_ptr;                        //the return value of the thread
} thread_control_block;


// GLOBAL VARS
int cur_thread_index = 0;
queue_t thread_queue;
int thread_count = 0; //count the total number of threads in order to assign thread ids ???
thread_control_block* TCB_array[512]; // Array to hold non-ready threads


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

    //only enqueue current thread again if its state is RUNNING
    if( TCB_array[cur_thread_index]->state == STATE_RUNNING ){
             //since current thread will no longer be running, set to ready state in queue
             TCB_array[cur_thread_index]->state = STATE_READY;
             // Push current thread to ready queue
             queue_enqueue(thread_queue, TCB_array[cur_thread_index]);
    }

    // context switch [running the next thread]
    int temp_index = cur_thread_index;
    cur_thread_index = popped->index;
    popped->state = STATE_RUNNING;
    uthread_ctx_switch(TCB_array[temp_index]->context, (void*) popped->context);
}

uthread_t uthread_self(void)
{
    if(thread_count == 0){
        return 1;
    }
    return TCB_array[cur_thread_index]->id;
}

// Add main thread to the thread array
void uthread_init_main(void){
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
}

int uthread_create(uthread_func_t func, void *arg)
{
    //if this is the first call to create_thread(), initialze queue and save TCB of main to current TCB
    if(thread_queue == NULL){
        uthread_init_main();
    }

    thread_count++;

    //initialize a new thread to execute the provided function with the given arguments
    thread_control_block* nextTCB = (thread_control_block*) malloc(sizeof(thread_control_block));
    uthread_ctx_t* cxt = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));

    void* stack = uthread_ctx_alloc_stack();
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

    return nextTCB->id;
}

void uthread_exit(int retval)
{
    // Set thread to zombie (not runnig anymore)
    TCB_array[cur_thread_index]->state = STATE_ZOMBIE;

    if(TCB_array[cur_thread_index]->joined_thread != NULL){
            ((thread_control_block*)(TCB_array[cur_thread_index]->joined_thread))->state = STATE_READY;
            queue_enqueue(thread_queue, (void*) TCB_array[cur_thread_index]->joined_thread);
    }

    // yield to allow next ready thread to run
    uthread_yield();

}

int uthread_join(uthread_t tid, int *retval)
{
        if(tid <= 0                                //thread provided is main thread or negative
                || tid == cur_thread_index                 //thread being joined is already the current thread
                || tid > thread_count                      //thread provided does not exist
                || TCB_array[tid]->joined_thread != NULL){ //thread is already joined
                return -1;
        }


        TCB_array[tid]->joined_thread = TCB_array[cur_thread_index];
        TCB_array[tid]->ret_ptr = retval;


        if(TCB_array[tid]->state != STATE_ZOMBIE){
                TCB_array[cur_thread_index]->state = STATE_BLOCKED;
                uthread_yield();
        }
        return 0;
}

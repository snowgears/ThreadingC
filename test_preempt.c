/*
 * Preempt test
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep

#include "libuthread/uthread.h"
#include "libuthread/preempt.h"

extern volatile short hit;

// Prints str
int thread2(char* str)
{
	printf("%s\n", str);
	return 0;
}

// Sleeps for 3 sec
int thread1(void* arg)
{
	uthread_create( (int(*)(void*))thread2, "I am thread2");
	uthread_yield();
    sleep(2);
	printf("thread%d\n", uthread_self());
	return 0;
}

int main(void)
{
        preempt_enable();
        preempt_start();

    uthread_t child1 = uthread_create(thread1, NULL);
    int child1_ret;
	uthread_join(child1, &child1_ret);
    printf("Main is done waiting for thread1\n");
    
    while(hit == 0){
        printf("hit: %d\n", hit);
    }
    printf("hit: %d\n", hit);
	return 0;
}

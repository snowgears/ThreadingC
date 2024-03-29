/*
 * Thread join test
 *
 * This will create thread1 which will create then 2 then yield.
 * Then thread2 will print a string and exit.
 * Then thread1 will resume and sleep for 2 sec and print thread1
 * Main will wait until 1 is done and print.
 *
 *
 * Output should be:
 * I am thread2
 * thread1
 * Main is done waiting for thread1
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep

#include "libuthread/uthread.h"

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
    uthread_t child1 = uthread_create(thread1, NULL);
    int child1_ret;
	uthread_join(child1, &child1_ret);
    printf("Main is done waiting for thread1\n");

	return 0;
}

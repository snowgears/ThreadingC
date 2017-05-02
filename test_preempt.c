/*
 * Preempt test
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For sleep

#include "libuthread/uthread.h"
#include "libuthread/preempt.h"

volatile short thread2_hit =0;
volatile short thread3_hit =0;

int thread3(void* arg){
    thread3_hit = 1;
    printf("thread%d\n", uthread_self());
    return 0;
}

// Prints str
int thread2(char* str)
{
    thread2_hit = 1;
    printf("%s\n", str);
    
    // Create thread3
    uthread_create( (int(*)(void*))thread3, NULL);
    preempt_disable();
    return 0;
}

// Sleeps for 3 sec
int thread1(void* arg)
{
    uthread_create( (int(*)(void*))thread2, "I am thread2");
    preempt_disable();
    int x;
    for(x =0; x< 99999999; x++){
        if(thread2_hit == 1){
            printf("thread2 hit after %d iterations\n", x);
            thread2_hit = 2;
            
        }
        if(thread3_hit == 1){
            printf("thread3 hit after %d iterations\n", x);
            thread3_hit = 2;
            
        }
    }
    printf("thread%d\n", uthread_self());
    return 0;
}

int main(void)
{
    preempt_start();

    uthread_t child1 = uthread_create(thread1, NULL);
    preempt_disable();
    int child1_ret;
    uthread_join(child1, &child1_ret);
    printf("Main is done waiting for thread1\n");
    
    // while(hit == 0){
    //     printf("hit: %d\n", hit);
    // }
    // printf("hit: %d\n", hit);
    return 0;
}

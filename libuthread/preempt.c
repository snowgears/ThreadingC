#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>  // For the timer
#include <signal.h>    // For signal handler

#include "preempt.h"
#include "uthread.h"


// Define the timer interval
struct timeval interval;


// Define timer val
struct itimerval timerval;


// Define sigaction to perform on timer alarm
struct sigaction s_action;

int is_enabled;



// Signal hander fn
void timer_handler(int signum){
    printf("I am the timer handler\n");
    return;
}

void preempt_disable(void)
{
        //TODO disable timers and whatnot
        is_enabled = 0;
}

void preempt_enable(void)
{
        is_enabled = 1;
}

void preempt_start(void){
        if(is_enabled == 0)
                return;

    // Set timer interval
    interval.tv_sec = (time_t) 0;
    interval.tv_usec = (long int) 10000; // 10,000 μs = 1/100 sec

    // Set timer val
    timerval.it_interval = interval;
    timerval.it_value = interval;

    // Set sigaction
    s_action.sa_handler = timer_handler;
    //s_action.sa_mask = 0;
    s_action.sa_flags = 0;

    // Register the signal handler
    sigaction (SIGVTALRM, &s_action, NULL);


    // Start the timer 100Hz
    setitimer(ITIMER_VIRTUAL , &timerval , NULL);
}

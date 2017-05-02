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
struct sigaction sig_action;

// Define signal set to block on preempt_disable
sigset_t sig_set;


// Signal hander fn
void timer_handler(int signum){
    // Forcibly make thread yield
    uthread_yield();
    return;
}

// Initialize vars
void preempt_init(){
    // TIMER
    
    // Set timer interval
    interval.tv_sec = (time_t) 0;
    interval.tv_usec = (long int) 10000; // 10,000 μs = 1/100 sec

    // Set timer val
    timerval.it_interval = interval;
    timerval.it_value = interval;

    // Set sigaction
    sig_action.sa_handler = timer_handler;
    //sig_action.sa_mask = 0;
    sig_action.sa_flags = 0;
    
    
    
    // ENABLE/DISABLE
    
    // set signal set to be empty
    sigemptyset(&sig_set);
    
    // Add SIGVTALRM to the set of signals to ignore on disable
    sigaddset(&sig_set, SIGVTALRM);
    
}



void preempt_disable(void){
    sigprocmask (SIG_BLOCK, &sig_set, NULL);
}

void preempt_enable(void){
    sigprocmask (SIG_UNBLOCK, &sig_set, NULL);
}

void preempt_start(void){
    // Initialize all vars
    preempt_init();
    preempt_enable();


    // Register the signal handler
    sigaction (SIGVTALRM, &sig_action, NULL);


    // Start the timer 100Hz
    setitimer(ITIMER_VIRTUAL , &timerval , NULL);
}

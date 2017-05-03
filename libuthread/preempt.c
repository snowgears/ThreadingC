#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>  // For the timer
#include <signal.h>    // For signal handler

#include "preempt.h"
#include "uthread.h"




// Define signal set to block on preempt_disable
// sigset_t sig_set;
//int is_enabled = 0; //THIS IS NOT ALLOWED> PROFESSOR SAID WONT GET POINTS FOR IMPLEMENTATION IF USED

// Signal hander fn
void timer_handler(int signum){
    //if(is_enabled == 1){
        // Forcibly make thread yield
        uthread_yield();
    //}
}

void preempt_disable(void){
    sigset_t sig_set;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGVTALRM);
    sigprocmask (SIG_BLOCK, &sig_set, NULL);
    //is_enabled = 0;

    kill(0,SIGVTALRM); //test that the signal to kill is blocked (WORKS SO SIGNAL IS BEING BLOCKED)
}

void preempt_enable(void){
    sigset_t sig_set;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGVTALRM);
    sigprocmask (SIG_UNBLOCK, &sig_set, NULL);
    //is_enabled = 1;
}

void preempt_start(void){
    // Define timer val
    struct itimerval timerval;
    // Define sigaction to perform on timer alarm
    struct sigaction sig_action;


    // Set timer val
    timerval.it_interval.tv_sec = (time_t) 0;
    timerval.it_interval.tv_usec = (long int) 10000; // 10,000 μs = 1/100 sec

    timerval.it_value.tv_sec = (time_t) 0;
    timerval.it_value.tv_usec = (long int) 10000; // 10,000 μs = 1/100 sec

    // Set sigaction
    sig_action.sa_handler = timer_handler;
    //sig_action.sa_mask = 0;
    sig_action.sa_flags = SA_RESTART;

    sigemptyset(&sig_action.sa_mask);


    // Register the signal handler
    if(sigaction (SIGVTALRM, &sig_action, NULL)){
            printf("Sigaction failed.\n");
            exit(1);
    }

    // Start the timer 100Hz
    if(setitimer(ITIMER_VIRTUAL, &timerval , NULL)){
            printf("Sig timer failed.\n");
            exit(1);
    }
}

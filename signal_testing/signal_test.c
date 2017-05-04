#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>  // For the timer
#include <signal.h>    // For signal handler


// Signal hander fn
void handler(int signum){
    printf("timer goes off\n");
}

void disable_sig(void){
    sigset_t sig_set;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGVTALRM);
    if(sigprocmask (SIG_BLOCK, &sig_set, NULL) != 0){
        printf("disable failed\n");
    }
}

void enable_sig(void){
    sigset_t sig_set;
    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGVTALRM);
    if(sigprocmask (SIG_UNBLOCK, &sig_set, NULL) != 0){
        printf("enable failed\n");
    }
}

void setup(void){
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
    sig_action.sa_handler = handler;
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

int main(void){
    setup();
    enable_sig();
    // Remove disable_sig to see the timer prints
    disable_sig();
    while(1){
        // Wait for sig handler
    }
    
}
# Threading Library Report

## Implementation

#### queue.c

For the queue, we implemented it using nodes in a similar fashion to a doubley    
linked list. To do this, we created a struct called **Node** that keeps track of  
the previous and next Node in the list, along with storing arbitrary data.  

```c
typedef struct node* nodePtr;

typedef struct node{
	void* data;
	nodePtr next;
	nodePtr prev;
}  Node;
```

Once we defined the nodes, we define a queue struct that stores a pointer to the  
head node and the tail node of the queue, along with the size of the queue.  

```c
struct queue {
	nodePtr head; // pushed onto
	nodePtr tail; // popped from
	int size;
};
```

From here it was very easy to implement the queue methods because we simply had    
to update the head and tail values (along with their previous and next pointers)    
depending on the method such as *enqueue* or *dequeue*. It also becomes easy to   
free the memory of a specific node without breaking other nodes as long as the     
associated values are updated.

#### uthread.c

This step was probably the most difficult to grasp conceptually because we had  
to read through the documentation in the header files and find out what  
everything was supposed to be used for. Once we pieced everything together and  
found out what our specific **thread control block** needed to store, we defined  
a struct for it.  

```c
typedef struct thread_control_block{
    uthread_t id;                        //the unqiue id of the TCB
    int state;                           //the state of the TCB
    uthread_ctx_t* context;              //contains information about function, arguments, and stack
    int index;                           //index of struct in the TCB_array (for retreiving later)
    struct thread_control_block* joined_thread; //the joined TCB if there is one
    int* ret_ptr;                        //the return value of the thread
} thread_control_block;
```

We first started with the **create method** which was easy enough since all we    
needed to do was define a new struct of type **thread_control_block** and   
populate the values. For id, we simply declare a global int and increment it   
each time a new thread is created so that each thread has a unique id. We set   
the state to *STATE_READY* (a macro) as the default and define the index at   
which we store the struct in our global array. We then initialize the context   
using the methods in *context.h* so that the thread struct knows what function   
to run and with which arguments. We track how threads are joined with the   
pointer to the joined thread and the return value of another thread if that     
specific struct is joined to another thread. Once everything is initialized  
within the struct, we push it onto our queue using the *enqueue* method.  

Next we wrote the **yield method** where we essentially pop the next *thread     
control block* off of the queue and push the current running thread onto the end     
of the queue. We then set the popped *thread control block* to the running state  
(*STATE_RUNNING*)and call a context switch from the previous thread to the new  
thread.  

```c
// context switch [running the next thread]
int temp_index = cur_thread_index;
cur_thread_index = popped->index;
popped->state = STATE_RUNNING;
uthread_ctx_switch(TCB_array[temp_index]->context, (void*) popped->context);
```

For our **exit method**, we set the current running state to *STATE_ZOMBIE* and  
then check if there is a joined thread to the current thread that is ending. If  
there is, we set the joined thread to ready and *enqueue* it. We then **yield**   
to allow the next thread in the queue to run.

Finally, we wrote the **join method**. In this, we join our current running   
thread control block to the passed in thread control block along with the return  
value. Then we call **yield** but only if the passed in thread is not in a  
zombie state (*STATE_ZOMBIE*).  
```c
TCB_array[tid]->joined_thread = TCB_array[cur_thread_index];
TCB_array[tid]->ret_ptr = retval;  

if(TCB_array[tid]->state != STATE_ZOMBIE){
        TCB_array[cur_thread_index]->state = STATE_BLOCKED;
        uthread_yield();
}
```

#### preempt.c

The **preemption step** of this project probably took the most time due to      
debugging how the handler was called but eventually we discovered the behavior     
was acting as it should the whole time, we were simply confused why that was the  
case.  

We started off by writing the **timer start** method in which we declare a  
*itimerval* and set it to run every 1/100th of a second (*or 10,000 μs*). We  
then declare a *sigaction* which we set the *sa_handler* field to run a timer  
handler method in which we simply call **yeild** to force the thread to yield  
to the next thread every time the timer goes off. Then we register the signal  
action to be *SIGVTALRM* and set the timer.  
```c
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
```

For **enabling preemption**, we use the *sigprocmask* method using the   
*SIG_UNBLOCK* argument and pass in a *sigset_t* containing only the *SIGVTALRM*  
signal. This will unblock any of the *SIGVTALRM* signals if they were currently  
being blocked.  
```c
sigset_t sig_set2;
sigemptyset(&sig_set2);
sigaddset(&sig_set2, SIGVTALRM);
if(sigprocmask (SIG_UNBLOCK, &sig_set2, NULL) != 0){
    printf("enable failed\n");
}
```

**Disabling preemption** is essentially the exact same process, only instead of  
using *SIG_UNBLOCK*, we use *SIG_BLOCK* to make sure that the signal that calls  
our yield method is blocked while preemption is disabled to make sure the thread  
only yields when it is ready.
```c
sigset_t sig_set;
sigemptyset(&sig_set);
sigaddset(&sig_set, SIGVTALRM);
if(sigprocmask (SIG_BLOCK, &sig_set, NULL) != 0){
    printf("disable failed\n");
}
```  

## Test Files

#### test_queue.c
This test file simply tests our queue implementation by queueing and dequeueing  
from the queue and then asserting that the changes are correct.  

#### uthread_yield.c
This test file tests if the yielding of threads works correctly. It calls yield  
every time a new thread is created so everything is enqueued as it is created.  
This allowed us to see that yielding threads would run the next thread in the  
queue and allow execution of each one to finish as it is supposed to.  

#### uthread_join.c
This test file simply tests that joining threads will allow for them to yeild and  
wait for the return value in order to store it. This particular file joins the  
main thread with thread 1, which in turn creates a thread 2. Main then finishes  
last as it yields and stores the result of thread 1 within its control block.  

#### test_preempt.c
This test file is supposed to test the preemption and disable preemption  
implementation. As the file is, it will test that disable works. So to test if  
preemption works enabled you have to comment out lines 25, 37, and 64. Once they  
are commented out you have to remake. When it is run, the main thread will wait  
for thread1 to finish. Thread 1 will run the huge loop and be preempted and run  
thread2. This loop ensures that the timer goes off during thread1 so that we know  
thread2 should run next. Then after thread2 terminates, thread1 will continue its  
huge loop and be preemped again and run thread3. This happens just as expected.  
Then thread3 exits and thread1 continues, thread1 exits, and main thread continues  
and exits. This works fine.

Testing for disable is a bit more interesting. Make sure all the lines mentioned  
above are uncommented. Then recompile and run. The main thread will create and  
wait for thread1 to finish. Thread1 starts and enters the huge loop. Thread1 will  
NOT be preemped in the loop and will terminate. When there is a context switch to  
thread2, enable is called in context.c (we cannot change that) and it will be  
enabled. Then all the alarms that were blocked will be caught and call yield. This  
means that thread2 will yield and the main thread will continue. Then the main  
thread will terminate and thread2 and 3 never run. This is intended behavior as it  
is the user's job to make sure threads actually run. Notice that the preemption   
that previously happened in the loop doesn't happen this time. This proves that  
the preemption was disabled. Also note that it will always be enabled on context  
switch.

## Sources Used:

* [GNU: System Contexts process](https://www.gnu.org/software/libc/manual/html_mono/libc.html#System-V-contexts)
* [GNU: Signal Actions process](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Signal-Actions)
* [GNU: Setting An Alarm process](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Setting-an-Alarm)
* [GNU: Blocking Signals process](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Blocking-Signals)

### Authors
Tanner,Embry,913351817,tnembry@ucdavis.edu  
Cuauhtemoctzin,Rodriguez,999174733,ctrodriguez@ucdavis.edu  

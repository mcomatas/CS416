// File:	mypthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
int idCounter = 0;
int runningId = 0; //id of currently running thread

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg) {
       // create Thread Control Block
       // create and initialize the context of this thread
       // allocate space of stack for this thread to run
       // after everything is all set, push this thread int
       // YOUR CODE HERE
	   if(idCounter == 0) startup(); //initialize timer and scheduler context upon creation of first thread
	   //initialize new thread

		tcb newThread;
		newThread.tid = idCounter; 

		idCounter++;

		newThread.status = READY;
		newThread.quantumsElapsed = 0;

		newThread.threadStack = malloc(SIGSTKSZ);
		if(newThread.threadStack == NULL){
			handle_error("threadStack malloc error");
		}
		
		if(getcontext(&newThread.threadContext) < 0){
			handle_error("thread context error");
		}

		newThread.threadContext.uc_stack.ss_sp = newThread.threadStack;
		newThread.threadContext.uc_stack.ss_size = STACK_SIZE;
		newThread.threadContext.uc_stack.ss_flags = 0; //this mgiht change, its how many arguments it takes i think
		newThread.threadContext.uc_link = NULL;
		makecontext(&newThread.threadContext, (void*)function, 1, arg);

		//insert TCB into queue, runqueue is in header file after queue typedef
		enqueue(runQueue, newThread);

    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int mypthread_yield() {

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// switch from thread context to scheduler context

	//could possibly look at front of queue? if the front of the queue is the one that is running, but the one that is running might not be in the queue
	//look at the front of the queue and dequeue it because the front of the queue is the current running thread, we then swap context to the scheduler

	tcb thread = dequeue( runQueue ); //get the front of the queue aka the current running thread
	//I think we need to then swap context from this tcb context to the scheduler context, then all we do is enqueue this thread to the rear, since it is not terminating
	swapcontext( &thread.threadContext, &schedContext );
	thread.status = WAITING; //change status to waiting if it is in queue

	//I think then we enqueue the tcb back into the queue
	enqueue( runQueue, thread );
	// YOUR CODE HERE
	
	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread

	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex

	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread

        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.

	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init

	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library
	// should be contexted switched from thread context to this
	// schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (STCF or MLFQ)

	// if (sched == STCF)
	//		sched_stcf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

// schedule policy
#ifndef MLFQ
	// Choose STCF
#else
	// Choose MLFQ
#endif

}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {
	// Your own implementation of STCF
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

// Feel free to add any other functions you need

// YOUR CODE HERE
void startup(){
	//setup timer for schedule
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &schedule; //what function is called when timer signal happens
	sigaction (SIGPROF, &sa, NULL);
	//timer struct
	struct itimerval timer;
	//when does the timer reset
	timer.it_interval.tv_usec = QUANTUM * 1000; //1000 microsecs = 1 ms
	timer.it_interval.tv_sec = 0;
	//set up current timer
	timer.it_value.tv_usec = QUANTUM * 1000;
	timer.it_value.tv_sec = 0;
    setitimer(ITIMER_PROF, &timer, NULL);

	//set up scheduler context
	void* schedulerStack = malloc(STACK_SIZE);
	if(schedulerStack == NULL){
		handle_error("bad scheduler malloc");
	}
	if(getcontext(&schedContext) < 0){
		handle_error("getcontext schedContext error");	
	}
	schedContext.uc_stack.ss_sp = schedulerStack;
	schedContext.uc_stack.ss_size = STACK_SIZE;
	schedContext.uc_stack.ss_flags = 0;
	schedContext.uc_link = NULL;
	makecontext(&schedContext, (void*)schedule, 0);
}
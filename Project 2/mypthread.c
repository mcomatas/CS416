// File:	mypthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
static tcb runQueue[150];
static mypthread_t idCounter = 0;
static mypthread_t current;
static int mutexIdCounter = 0;
int justExited = 0;


/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr,
                      void *(*function)(void*), void * arg) {
    if(idCounter == 0){
    	//setup array
    	int i = 0;
    	for(i = 0; i < 150; i++){
    		runQueue[i].status = NOTUSED;
    	}
    	//make main thread
    	runQueue[0].tid = idCounter;
    	runQueue[0].status = READY;
    	runQueue[0].waitingOn = -1;
    	runQueue[0].beingWaitedOnBy = -1;
    	runQueue[0].waitingOnMutex = -1;
    	runQueue[0].quantumsElapsed = 0;
    	
    	current = idCounter;
    	
    	idCounter++;
		
		//setup timer for schedule
		struct sigaction sa;
		memset (&sa, 0, sizeof (sa));
		sa.sa_handler = &swapToScheduler; //what function is called when timer signal happens
		sigaction (SIGPROF, &sa, NULL);
		//when does the timer reset
		timer.it_interval.tv_usec = QUANTUM * 1000; //1000 microsecs = 1 ms
		timer.it_interval.tv_sec = 0;
		//set up current timer
		timer.it_value.tv_usec = QUANTUM * 1000;
		timer.it_value.tv_sec = 0;
		setitimer(ITIMER_PROF, &timer, NULL);
    }
    
    runQueue[idCounter].tid = idCounter;
    runQueue[idCounter].status = READY;
    runQueue[idCounter].waitingOn = -1;
    runQueue[idCounter].beingWaitedOnBy = -1;
    runQueue[idCounter].waitingOnMutex = -1;
    runQueue[idCounter].quantumsElapsed = 0;
    
    void* threadStack = malloc(STACK_SIZE);
    
    getcontext(&runQueue[idCounter].threadContext);
    runQueue[idCounter].threadContext.uc_stack.ss_sp = threadStack;
    runQueue[idCounter].threadContext.uc_stack.ss_size = STACK_SIZE;
    runQueue[idCounter].threadContext.uc_stack.ss_flags = 0;
    runQueue[idCounter].threadContext.uc_link = NULL;
    makecontext(&runQueue[idCounter].threadContext, (void*)function, 1, arg);
    if(DEBUG) printf("made thread %d\n", idCounter);
    *thread = idCounter;
    idCounter++;
    if(idCounter == 2){
    	getcontext(&runQueue[0].threadContext);
    }

    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int mypthread_yield() {

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// wwitch from thread context to scheduler context
	justExited = 0;
	schedule();
	// YOUR CODE HERE
	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread
	pauseTimer();
	runQueue[current].status = DONE;
	if(runQueue[current].beingWaitedOnBy != -1){
		runQueue[runQueue[current].beingWaitedOnBy].waitingOn = -1;
		runQueue[runQueue[current].beingWaitedOnBy].status = READY;
		if(DEBUG)printf("thread %d exit, %d can now proceed\n", current, runQueue[current].beingWaitedOnBy);
	}
	free(runQueue[current].threadStack);
	if(DEBUG) printf("thread %d exit\n", current);
	justExited = 1;
	resumeTimer();
	schedule();
	// YOUR CODE HERE
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {

	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread
	if(runQueue[thread].status == DONE) {
		justExited = 0;
		schedule();
		return 0;
	}
	runQueue[thread].beingWaitedOnBy = current;
	runQueue[current].status = WAITBLOCK;
	runQueue[current].waitingOn = thread;
	justExited = 0;
	ucontext_t currentctx;
	getcontext(&currentctx);
	if(DEBUG) printf("thread %d waiting on %d\n", current, thread);
	schedule();
	return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex
	*mutex = (mypthread_mutex_t){
		.mutexId = mutexIdCounter,
		.lockState = UNLOCKED,
	};
	mutexIdCounter++;
	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
  	  while(__atomic_test_and_set((volatile void*)&mutex->lockState, __ATOMIC_RELAXED)){
       	runQueue[current].status = WAITBLOCK;
       	runQueue[current].waitingOnMutex = mutex->mutexId;
        justExited = 0;
        schedule();
      }
        
      if(mutex->lockState == UNLOCKED) mutex->lockState = LOCKED;
      return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	mutex->lockState = UNLOCKED;
	int i = 0;
	while(runQueue[i].status != NOTUSED){
		if(runQueue[i].waitingOnMutex == mutex->mutexId){
			runQueue[i].waitingOnMutex = -1;
			runQueue[i].status = READY;
		}
		i++;
	}
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init
	mutex->lockState = UNLOCKED;
	int i = 0;
	while(runQueue[i].status != NOTUSED){
		if(runQueue[i].waitingOnMutex == mutex->mutexId){
			runQueue[i].waitingOnMutex = -1;
			runQueue[i].status = READY;
		}
		i++;
	}
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
	pauseTimer();
// schedule policy
	sched_stcf();
//#ifndef MLFQ
	// Choose STCF
//#else
	// Choose MLFQ
//#endif

}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {
	// Your own implementation of STCF
	// (feel free to modify arguments and return types)
	int i = 0;
	int lowestQuants = 69420;
	int toRun = -1;
	int oldRunner = current;
	while(runQueue[i].status != NOTUSED){
		if(runQueue[i].waitingOn == -1 && runQueue[i].waitingOnMutex == -1 && runQueue[i].status != DONE && runQueue[i].status != WAITBLOCK && runQueue[i].quantumsElapsed < lowestQuants){
			toRun = i;
			lowestQuants = runQueue[i].quantumsElapsed;
		}
		i++;
	}
	if(justExited == 1 && lowestQuants != 69420){
		//if(DEBUG) printf("scheduler to go from thread %d to %d\n", oldRunner, toRun);
		current = toRun;
		resumeTimer();
		setcontext(&runQueue[toRun].threadContext);
	}
	else if(justExited == 0 && lowestQuants != 69420){
		//if(DEBUG) printf("scheduler to go from thread %d to %d\n", oldRunner, toRun);
		current = toRun;
		resumeTimer();
		swapcontext(&runQueue[oldRunner].threadContext, &runQueue[toRun].threadContext);
	}
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
void swapToScheduler(){
	pauseTimer();
	runQueue[current].quantumsElapsed = runQueue[current].quantumsElapsed + 1;
	runQueue[current].status = READY;
	justExited = 0;
	schedule();
}

void pauseTimer(){
	struct itimerval zero_timer = {0};
	setitimer(ITIMER_PROF, &zero_timer, NULL);
}

void resumeTimer(){
	setitimer(ITIMER_PROF, &timer, NULL);
}

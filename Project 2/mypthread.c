// File:	mypthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
int idCounter = 0;
int mutexIdCounter = 0;

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
		//pauseTimer();
		tcb newThread;
		newThread.tid = idCounter;
		if(DEBUGMODE) printf("new thread with id %d created\n", idCounter);
		newThread.status = READY;
		newThread.quantumsElapsed = 0;
		newThread.waitingOn = -1;
		newThread.beingWaitedOnBy = -1;
		newThread.waitingOnMutex = -1;
		*thread = idCounter;
		idCounter++;

		newThread.threadStack = malloc(SIGSTKSZ);
		if(newThread.threadStack == NULL){
			handle_error("threadStack malloc error");
		}
		
		if(getcontext(&newThread.threadContext) < 0){
			handle_error("thread context error");
		}
		//if(DEBUGMODE) printf("initializing thread context...\n");
		newThread.threadContext.uc_stack.ss_sp = newThread.threadStack;
		newThread.threadContext.uc_stack.ss_size = STACK_SIZE;
		newThread.threadContext.uc_stack.ss_flags = 0; 
		newThread.threadContext.uc_link = NULL;
		makecontext(&newThread.threadContext, (void*)function, 1, arg);
		//if(DEBUGMODE) printf("initialized thread context\n");
		//insert TCB into queue, runqueue is in header file after queue typedef
		enqueue(runQueue, newThread);
		//if(DEBUGMODE) printf("enqueued thread %d\n", newThread.tid);
		//resumeTimer();
    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int mypthread_yield() {

	// change thread state from Running to Ready
	// save context of this thread to its thread control block
	// switch from thread context to scheduler context
	//pauseTimer();
	runQueue->array[runQueue->front].status = READY;
	tcb thread = dequeue( runQueue ); //get the front of the queue aka the current running thread
	enqueue( runQueue, thread );
	if(DEBUGMODE) printf("thread %d yielding\n", thread.tid);
	//resumeTimer();
	swapcontext( &thread.threadContext, &schedContext );

	// YOUR CODE HERE
	
	return 0;
};

/* terminate a thread */
void mypthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread
	// YOUR CODE HERE
	//pauseTimer();
	tcb finished = dequeue(runQueue);
	finished.status = DONE;
	//check if any thread is waiting on it, if there is then tell it it's good to go
	if(finished.beingWaitedOnBy != -1){
		int waiterIndex = findThread(runQueue, finished.beingWaitedOnBy);
		if(waiterIndex != -342){
			runQueue->array[waiterIndex].waitingOn = -1;
			runQueue->array[waiterIndex].status = READY;
		}
	}
	free(finished.threadStack);
	if(DEBUGMODE) printf("thread %d exiting\n", finished.tid);
	//resumeTimer();
	//swapcontext(&finished.threadContext, &schedContext);
	setcontext(&schedContext);
};


/* Wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr) {
	// wait for a specific thread to terminate
	// de-allocate any dynamic memory created by the joining thread
	//pauseTimer();
	if(DEBUGMODE) printf("joining thread %d with thread %d\n", front(runQueue).tid, thread);
	int waitingOnThreadIndex = findThread(runQueue, thread);
	runQueue->array[runQueue->front].waitingOn = runQueue->array[waitingOnThreadIndex].tid;
	runQueue->array[waitingOnThreadIndex].beingWaitedOnBy = runQueue->array[runQueue->front].tid;
	runQueue->array[runQueue->front].status = WAITING;
	//resumeTimer();
	if(DEBUGMODE) printf("thread %d now waiting on thread %d\n", runQueue->array[runQueue->front].tid, runQueue->array[waitingOnThreadIndex].tid);
	swapcontext(&runQueue->array[runQueue->front].threadContext, &schedContext); //back to the scheduler
	// YOUR CODE HERE
	return 0;
};

/* initialize the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex,
                          const pthread_mutexattr_t *mutexattr) {
	//initialize data structures for this mutex
	//pauseTimer();
	*mutex = (mypthread_mutex_t){
		.mutexId = mutexIdCounter,
		.lockState = UNLOCKED,
		//.currentHolder = front(runQueue).tid
	};
	mutexIdCounter++;
	if(DEBUGMODE) printf("created mutex %d\n", mutex->mutexId);
	//resumeTimer();
	// YOUR CODE HERE
	return 0;
};

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex) {
        // use the built-in test-and-set atomic function to test the mutex
        // if the mutex is acquired successfully, enter the critical section
        // if acquiring mutex fails, push current thread into block list and //
        // context switch to the scheduler thread
		//pauseTimer();
		//if(DEBUGMODE) printf("locking mutex %d\n", mutex->mutexId);
		while(__atomic_test_and_set((volatile void*)&mutex->lockState, __ATOMIC_RELAXED)){
			tcb lockedOut = dequeue(runQueue);
			lockedOut.waitingOnMutex = mutex->mutexId;
			lockedOut.status = WAITING;
			enqueue(runQueue, lockedOut);
			//resumeTimer();
			swapcontext(&lockedOut.threadContext, &schedContext);
		}

		//if thread made it past the while loop, it gets to use the mutex. lock it
		if(mutex->lockState == UNLOCKED) mutex->lockState = LOCKED;
		//resumeTimer();
        // YOUR CODE HERE
        return 0;
};

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex) {
	// Release mutex and make it available again.
	// Put threads in block list to run queue
	// so that they could compete for mutex later.
	//pauseTimer();
	//if(DEBUGMODE) printf("unlocking mutex %d\n", mutex->mutexId);
	mutex->lockState = UNLOCKED;
	int i, j;
	for(i = runQueue->front, j = 0; j < runQueue->size; i++, j++){
		if(i == runQueue->capacity) i = 0;
		if(runQueue->array[i].waitingOnMutex == mutex->mutexId){
			runQueue->array[i].waitingOnMutex = -1;
			runQueue->array[i].status = READY;
		}
	}
	//resumeTimer();
	// YOUR CODE HERE
	return 0;
};


/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in mypthread_mutex_init
	// i dont think we had to allocate any memory, 
	//so i'll just release the lock and put all of the threads waiting on it back to ready mode
	//pauseTimer();
	if(DEBUGMODE) printf("destroying mutex %d\n", mutex->mutexId);
	mutex->lockState = UNLOCKED;
	int i, j;
	for(i = runQueue->front, j = 0; j < runQueue->size; i++, j++){
		if(i == runQueue->capacity) i = 0;
		if(runQueue->array[i].waitingOnMutex == mutex->mutexId){
			runQueue->array[i].waitingOnMutex = -1;
			runQueue->array[i].status = READY;
		}
	}
	//resumeTimer();
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
	if(DEBUGMODE) printf("entering scheduler\n");

	//schedule policy
	// #ifndef MLFQ
	// 	// Choose STCF
	// #else
	// 	// Choose MLFQ
	// #endif
	sched_stcf();
}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {
	// Your own implementation of STCF
	// (feel free to modify arguments and return types)
	while(1){
		int i, j;
		int currentLowestQuantum = -1;
		int lowestQuantumTid = -1;
		//find a starter set of values that isnt waiting on another thread or a mutex and isnt a leftover done
		for(i = runQueue->front, j = 0; j < runQueue->size; i++, j++){ 
			if(i == runQueue->capacity) i = 0;
			if(runQueue->array[i].waitingOn == -1 && runQueue->array[i].waitingOnMutex == -1 &&
			 runQueue->array[i].status != DONE){
				currentLowestQuantum = runQueue->array[i].quantumsElapsed;
				lowestQuantumTid = runQueue->array[i].tid;
				break;
			}
		}

		if(DEBUGMODE) printf("sched start point: thread %d with %d q. elapsed\n", lowestQuantumTid, currentLowestQuantum);
		//find highest priority tcb (lowest runtime thus far) that isnt waiting on a thread/mutex or isnt a leftover done
		for(i = runQueue->front, j = 0; j < runQueue->size; i++, j++){ 
			if(i == runQueue->capacity) i = 0;
			if(runQueue->array[i].quantumsElapsed < currentLowestQuantum && runQueue->array[i].waitingOn == -1 &&
			 runQueue->array[i].waitingOnMutex == -1 && runQueue->array[i].status != DONE){ 
				currentLowestQuantum = runQueue->array[i].quantumsElapsed;
				lowestQuantumTid = runQueue->array[i].tid;
			}
		}
		if(DEBUGMODE) printf("next thread: thread %d with %d q. elapsed\n", lowestQuantumTid, currentLowestQuantum);
		//rotating runqueue until highest priority is in the running position
		if(DEBUGMODE) printf("shifting thread %d to run position\n", lowestQuantumTid);
		while(front(runQueue).tid != lowestQuantumTid){
			tcb temp = dequeue(runQueue);
			enqueue(runQueue, temp);
		} 
		runQueue->array[runQueue->front].status = RUNNING;
		if(DEBUGMODE) printf("now running thread %d\n", front(runQueue).tid);
		resumeTimer(); //back to action
		swapcontext(&schedContext, &runQueue->array[runQueue->front].threadContext);
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
	//pause timer
	pauseTimer();
	if(DEBUGMODE) printf("time. thread %d has currently run for %d quantums; ", runQueue->array[runQueue->front].tid, runQueue->array[runQueue->front].quantumsElapsed);
	runQueue->array[runQueue->front].quantumsElapsed++;
	runQueue->array[runQueue->front].status = READY;
	if(DEBUGMODE) printf("thread %d has now run for %d quantums\n", runQueue->array[runQueue->front].tid, runQueue->array[runQueue->front].quantumsElapsed);
	swapcontext(&runQueue->array[runQueue->front].threadContext, &schedContext);
}

void pauseTimer(){
	struct itimerval zero_timer = {0};
    //setitimer(ITIMER_PROF, &zero_timer, &timer);
    setitimer(ITIMER_PROF, &zero_timer, NULL);
}

void resumeTimer(){
	setitimer(ITIMER_PROF, &timer, NULL);
}

void startup(){
	//initialize queue
	runQueue = createQueue(QUEUE_SIZE);
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
	//go
	resumeTimer();
}

struct Queue* createQueue(unsigned cap)
{
	struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
	queue->capacity = cap;
	queue->front = 0; //just initializing things to 0 here to begin -- these should be ok to keep at 0 since they are like indices for the array
	queue->size = 0;

	queue->rear = cap - 1;
	queue->array = (tcb*)malloc(queue->capacity * sizeof(tcb)); //the tcb used to be int here
	return queue;
}

int isFull(struct Queue* queue)
{
	if( queue->size == queue->capacity )
	{
		return 1; //true
	}
	else
	{
		return 0; //false
	}
}

int isEmpty(struct Queue* queue)
{
	if( queue->size == 0 )
	{
		return 1; //true
	}
	else
	{
		return 0; //false 
	}
}

void enqueue(struct Queue* queue, tcb item)//changed int item to mypthread_t item
{
	if( isFull(queue) == 1 )
	{
		return;//not sure what to return here 
	}
	else
	{
		queue->rear = (queue->rear + 1) % queue->capacity;
		queue->array[queue->rear] = item;
		queue->size = queue->size + 1;
		//if(DEBUGMODE) printf("enqueued thread %d, ", item.tid);
	}
}

tcb dequeue(struct Queue* queue)//changed return type from int to mypthread_t
{
	if( isEmpty(queue) == 1 )
	{
		tcb filler;
		return filler; //filler -1 for now, could be INT_MIN or something but wont be returning ints in the end
	}
	//int item = queue->array[queue->front];
	//mypthread_t item = queue->array[queue->front];
	tcb item = queue->array[queue->front];
	//if(DEBUGMODE) printf("dequeued thread %d, ", item.tid);
	queue->front = (queue->front + 1) % queue->capacity;
	queue->size = queue->size - 1;
	return item;
}

tcb front(struct Queue* queue)//changed retrun type
{
	if( isEmpty(queue) == 1 )
	{
		tcb filler;
		return filler; //filler again, I don't know what to return if empty since it contains tcb, could it be a blank return statement, or maybe NULL?
	}
	else
	{
		return queue->array[queue->front];
	}
}

tcb rear(struct Queue* queue)
{
	if( isEmpty(queue) == 1 )
	{
		tcb filler;
		return filler; //filler again, maybe NULL again? just made a black tcb variable to return basically 
	}
	else
	{
		return queue->array[queue->rear];
	}
	
}

int findThread(struct Queue* queue, int targetTid){
	int i, j;
	for(i = queue->front, j = 0; j < queue->size; i++, j++){
		if(i == queue->capacity) i = 0;
		if(queue->array[i].tid == targetTid) return i;
	}
	return -342;
}
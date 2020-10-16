// File:	mypthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "mypthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE
int idCounter = 0;
int currentQuantumsElapsed = -1; //global variable to hold quantumsElapsed of current thread
int currentId = -1; //tid of current/want to be current thread

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
	while(1){
		int i = 1;
		int currentLowestQuantum = -1;
		int lowestQuantumTid = -1;
		for(i = 1; i < runQueue->size; i++){
			if(runQueue->array[i].quantumsElapsed < currentQuantumsElapsed){ //found one that has been running for less time
				// currentId = runQueue->array[i].tid;
				// tcb oldFront = front(runQueue);
				// while(front(runQueue).tid != currentId){
				// 	tcb temp = dequeue(runQueue);
				// 	enqueue(runQueue, temp);
				// }
				// resumeTimer();
				// swapcontext(&oldFront.threadContext, &runQueue->array[runQueue->front].threadContext);
				currentLowestQuantum = runQueue->array[i].quantumsElapsed;
				lowestQuantumTid = runQueue->array[i].tid;
			}
		}
		tcb oldFront = front(runQueue);
		while(front(runQueue).tid != lowestQuantumTid){
			tcb temp = dequeue(runQueue);
			enqueue(runQueue, temp);
		}
		resumeTimer();
		if(lowestQuantumTid == currentId) swapcontext(&schedContext, &oldFront.threadContext);
		else swapcontext(&schedContext, &runQueue->array[runQueue->front].threadContext);
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
	//swap the currently running thread's context (front of the queue) to scheduler context
	// tcb toSwap = dequeue(runQueue);
	// enqueue(runQueue, toSwap);
	currentQuantumsElapsed = front(runQueue).quantumsElapsed;
	currentId = front(runQueue).tid;
	swapcontext(&runQueue->array[runQueue->front].threadContext, &schedContext);
}

void pauseTimer(){
	struct itimerval zero_timer = {0};
    setitimer(ITIMER_PROF, &zero_timer, &timer);
}

void resumeTimer(){
	setitimer(ITIMER_PROF, &timer, NULL);
}

void startup(){
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
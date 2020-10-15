// File:	mypthread_t.h

// List all group member's name:
// username of iLab:
// iLab Server:

#ifndef MYTHREAD_T_H
#define MYTHREAD_T_H

#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_MYTHREAD macro */
#define USE_MYTHREAD 1

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <string.h>

#define STACK_SIZE SIGSTKSZ
#define QUEUE_SIZE 50
#define QUANTUM 20 //milliseconds
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

typedef uint mypthread_t;

enum my_pthread_state {
	/*ACTIVE, // 0
	BLOCKED, // 1
	DEAD // 2 */

	READY, // 0
	START, // 1
	RUNNING, // 2
	WAITING, // 3
	DONE // 4
};

typedef struct threadControlBlock {
	/* add important states in a thread control block */
	// thread Id
	// thread status
	// thread context
	// thread stack
	// thread priority
	// And more ...

	// YOUR CODE HERE
	int tid; // thread id
	enum my_pthread_state status;
	int quantumsElapsed; //lower quantums elapsed = higher priority for scheduler
	//enum thread_priority priority; // I'm thinking the lower the int the higher priority?
	void* threadStack;
	ucontext_t threadContext;
	
} tcb;

/* mutex struct definition */
typedef struct mypthread_mutex_t {
	/* add something here */
	// YOUR CODE HERE
	int currentHolder; //thread id of current thread using mutex
} mypthread_mutex_t;

/* define your data structures here: */
// Feel free to add your own auxiliary data structures (linked list or queue etc...)

// YOUR CODE HERE

// queue data structure here for run queue
typedef struct Queue
{
	int front, rear, size; //using queue of ints for now to set up data structure, this is ok to keep int because they are indexs of what is the front and rear
	unsigned capacity;
	//int* array;
	tcb* array; //change this from int array to mypthread_t array
} queue;

queue* runQueue; //runqueue
static ucontext_t schedContext;

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


/* Function Declarations: */
void startup();
void swapToScheduler();

/* create a new thread */
int mypthread_create(mypthread_t * thread, pthread_attr_t * attr, void
    *(*function)(void*), void * arg);

/* give CPU pocession to other user level threads voluntarily */
int mypthread_yield();

/* terminate a thread */
void mypthread_exit(void *value_ptr);

/* wait for thread termination */
int mypthread_join(mypthread_t thread, void **value_ptr);

/* initial the mutex lock */
int mypthread_mutex_init(mypthread_mutex_t *mutex, const pthread_mutexattr_t
    *mutexattr);

/* aquire the mutex lock */
int mypthread_mutex_lock(mypthread_mutex_t *mutex);

/* release the mutex lock */
int mypthread_mutex_unlock(mypthread_mutex_t *mutex);

/* destroy the mutex */
int mypthread_mutex_destroy(mypthread_mutex_t *mutex);

#ifdef USE_MYTHREAD
#define pthread_t mypthread_t
#define pthread_mutex_t mypthread_mutex_t
#define pthread_create mypthread_create
#define pthread_exit mypthread_exit
#define pthread_join mypthread_join
#define pthread_mutex_init mypthread_mutex_init
#define pthread_mutex_lock mypthread_mutex_lock
#define pthread_mutex_unlock mypthread_mutex_unlock
#define pthread_mutex_destroy mypthread_mutex_destroy
#endif

#endif

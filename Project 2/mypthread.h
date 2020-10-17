// File:	mypthread_t.h

// List all group member's name:
// username of iLab:
// iLab Server:

#ifndef MYTHREAD_T_H
#define MYTHREAD_T_H

#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_MYTHREAD macro */
#define USE_MYTHREAD 1

#define DEBUGMODE 1 //1 for debugging prints, 0 for none. 0 for final submission!

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
#define QUEUE_SIZE 125
#define QUANTUM 10 //milliseconds
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
	int waitingOn; //tid of thread it is waiting on (thread join)
	int beingWaitedOnBy; //tid of thread it is being waited on by (thread join)
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
	int mutexId;
	int locked;
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
struct itimerval timer;

/* Function Declarations: */
void startup();
void swapToScheduler();
void pauseTimer();
void resumeTimer();
struct Queue* createQueue(unsigned cap);
int isFull(struct Queue* queue);
int isEmpty(struct Queue* queue);
int isEmpty(struct Queue* queue);
void enqueue(struct Queue* queue, tcb item);
tcb dequeue(struct Queue* queue);
tcb front(struct Queue* queue);
tcb rear(struct Queue* queue);
tcb findThread(struct Queue* queue, int targetTid);

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

// File:	mypthread_t.h

// List all group member's name: Elijah Ongoco, Michael Comatas
// username of iLab: edo16, mac776
// iLab Server: ilab3.cs.rutgers.edu, cd.cs.rutgers.edu

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
//#include <valgrind/valgrind.h>

#define STACK_SIZE SIGSTKSZ
#define QUANTUM 10000 //milliseconds
#define DEBUG 0

typedef uint mypthread_t;

enum pthreadState{
	RUNNING = 0,
	READY = 1,
	DONE = 2,
	WAITBLOCK = 3,
	NOTUSED = 4,
	CLEANED = 5
};

enum mutexState{
	UNLOCKED = 0,
	LOCKED = 1
};

struct itimerval timer;
ucontext_t schedContext;

typedef struct threadControlBlock {
	/* add important states in a thread control block */
	// thread Id
	// thread status
	// thread context
	// thread stack
	// thread priority
	// And more ...
	mypthread_t tid;
	enum pthreadState status;
	int waitingOn;
	int beingWaitedOnBy;
	int waitingOnMutex;
	int quantumsElapsed;

	void* threadStack;
	ucontext_t threadContext;

	void* returnValue;	

	// YOUR CODE HERE
} tcb;

tcb runQueue[300];

/* mutex struct definition */
typedef struct mypthread_mutex_t {
	int mutexId;
	int lockState;
} mypthread_mutex_t;

/* define your data structures here: */
// Feel free to add your own auxiliary data structures (linked list or queue etc...)

// YOUR CODE HERE


/* Function Declarations: */
void swapToScheduler();
void pauseTimer();
void resumeTimer();
void cleanup();
static void sched_stcf();
static void schedule();

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
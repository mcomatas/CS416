#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../mypthread.h"

/* A scratch program template on which to call and
 * test mypthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 */
struct itimerval timer;
pthread_mutex_t   mutex;
ucontext_t mainctx, functx;
void printQueueTids(struct Queue* queue);
void ring();
void pauseTimer();
void resumeTimer();

int main(int argc, char **argv) {
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &ring; //what function is called when timer signal happens
	sigaction (SIGPROF, &sa, NULL);
	//when does the timer reset
	timer.it_interval.tv_usec = QUANTUM * 1000; //1000 microsecs = 1 ms
	timer.it_interval.tv_sec = 0;
	//set up current timer
	timer.it_value.tv_usec = QUANTUM * 1000;
	timer.it_value.tv_sec = 0;
	setitimer(ITIMER_PROF, &timer, NULL);
    printf("managed to finish\n");
	return 0;
}

void ring(){
    printf("done");
}

void printQueueTids(struct Queue* queue){
    int i, j;
    for(i = queue->front, j = 0; i <= runQueue->rear, j < queue->size; i++, j++){
        if(i == queue->capacity) i = 0;
        if(runQueue->array[i].waitingOn != -1){
            break;
        }
        printf("tid in array %d: %d \n", i, runQueue->array[i].tid);
    }
}
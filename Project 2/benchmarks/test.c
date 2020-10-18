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

void printQueueTids(struct Queue* queue);
void ring();
void pauseTimer();
void resumeTimer();

int main(int argc, char **argv) {
	pthread_mutex_init(&mutex, NULL);
    printf("managed to finish\n");
	return 0;
}

ring(){
    pauseTimer();
    int i = 0;
    while(i < 999){
        printf("%d", i);
        i++;
    }
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
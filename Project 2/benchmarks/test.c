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

void printQueueTids(struct Queue* queue);


int main(int argc, char **argv) {
	mypthread_t threadnum = 2;
    tcb thing;

    runQueue = createQueue(QUEUE_SIZE);
    mypthread_create(&threadnum, NULL, NULL, 0);
    mypthread_create(&threadnum, NULL, NULL, 0);
    mypthread_create(&threadnum, NULL, NULL, 0);
    printf("queue size: %d\n", runQueue->size);

    printQueueTids(runQueue);

    thing = dequeue(runQueue);
    thing = dequeue(runQueue);
    thing = dequeue(runQueue);
    printf("queue size: %d\n", runQueue->size);

    printQueueTids(runQueue);

    mypthread_create(&threadnum, NULL, NULL, 0);
    printf("queue size: %d\n", runQueue->size);

    printQueueTids(runQueue);


	return 0;
}

void printQueueTids(struct Queue* queue){
    int i;
    for(i = 0; i < runQueue->size; i++){
        if(runQueue->array[i].waitingOn != -1){
            break;
        }
        printf("tid in array %d: %d \n", runQueue->array[i].tid);
    }
}
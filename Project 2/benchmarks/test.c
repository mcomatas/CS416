#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../mypthread.h"
#include "../mypthread.c"

/* A scratch program template on which to call and
 * test mypthread library functions as you implement
 * them.
 *
 * You can modify and use this program as much as possible.
 * This will not be graded.
 */
int main(int argc, char **argv) {

	mypthread_t threadnum = 2;
    runQueue = createQueue(QUEUE_SIZE);
    mypthread_create(&threadnum, NULL, NULL, 0);
    mypthread_create(&threadnum, NULL, NULL, 0);
    mypthread_create(&threadnum, NULL, NULL, 0);
    // printf("%d", runQueue->array[0].tid);
    // printf("%d", runQueue->array[1].tid);
    // printf("%d", runQueue->array[2].tid);
	
	return 0;
}

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <string.h>
#include "mypthread.h"
#include "mypthread.c"

int main(int argc, char** argv){
    mypthread_t threadnum = 2;
    threadQueue = createQueue(QUEUE_SIZE);
    mypthread_create(&threadnum, NULL, NULL, 0);
    mypthread_create(&threadnum, NULL, NULL, 0);
    mypthread_create(&threadnum, NULL, NULL, 0);
    printf("%d", threadQueue->array[0].tid);
    printf("%d", threadQueue->array[1].tid);
    printf("%d", threadQueue->array[2].tid);
    return 0;
}
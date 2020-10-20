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

int main(int argc, char **argv) {
    int** vpointer1;
    int* vpointer2;
    int* returnval;

    int test = 69;

    returnval = test;
    vpointer2 = returnval;

    vpointer1 = vpointer2;
    printf("%d %d %d", returnval, vpointer1, vpointer2);
}

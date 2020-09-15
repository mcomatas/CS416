#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>

// Place any necessary global variables here

//double startTime, endTime;

int main(int argc, char *argv[]){

	/*
	 *
	 * Implement Here
	 *
	 *
	 */

    //Author: Michael Comatas

    struct timeval currTime;
    double startTime, endTime;

    int i = 0;
    //startTime = gettimeofday( &currTime, NULL );
    gettimeofday( &currTime, NULL );
    startTime = currTime.tv_usec;
    while( i < 100000 )
    {
        getpid();
        i++;
    }
    //endTime = gettimeofday( &currTime, NULL );
    gettimeofday( &currTime, NULL );
    endTime = currTime.tv_usec;

    double totalTime = endTime - startTime;
    double avgTime = totalTime / 100000;

    printf( "Syscalls Performed: %d\n", i );
    printf( "Total Elapsed Time: %f microseconds\n", totalTime );
    printf( "Average Time Per Syscall: %f microseconds\n", avgTime );

	return 0;

}
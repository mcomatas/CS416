#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>

// Place any necessary global variables here

int main(int argc, char *argv[]){
    //Author: Michael Comatas

    //struct timeval currTime;
    struct timeval startTime, endTime;
    //double startTime, endTime;

    int i = 0;
    //startTime = gettimeofday( &currTime, NULL );
    //gettimeofday( &currTime, NULL );
    //startTime = currTime.tv_usec;
    gettimeofday( &startTime, NULL );
    while( i < 100000 )
    {
        getpid();
        i++;
    }
    //endTime = gettimeofday( &currTime, NULL );
    //gettimeofday( &currTime, NULL );
    //endTime = currTime.tv_usec;
    gettimeofday( &endTime, NULL );

    //double totalTime = endTime - startTime;
    double seconds = (endTime.tv_sec - startTime.tv_sec);
    double micros = ((seconds * 1000000) + endTime.tv_usec) - (startTime.tv_usec);
    //long totalTime_v2 = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    double avgTime = micros / 100000;

    printf( "Syscalls Performed: %d\n", i );
    printf( "Total Elapsed Time: %f microseconds\n", micros );
    //printf( "Total Elapsed Time: %f microseconds (basic way)\n", totalTime );
    printf( "Average Time Per Syscall: %f microseconds\n", avgTime );

	return 0;

}
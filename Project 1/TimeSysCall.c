#include <stdlib.h>
#include <stdio.h>

#include <sys/time.h>

// Place any necessary global variables here

int main(int argc, char *argv[]){

    struct timeval startTime, endTime;
    int i = 0;
    
    gettimeofday( &startTime, NULL );
    while( i < 100000 )
    {
        getpid();
        i++;
    }
    gettimeofday( &endTime, NULL );

    double seconds = (endTime.tv_sec - startTime.tv_sec);
    double micros = ((seconds * 1000000) + endTime.tv_usec) - (startTime.tv_usec);
    double avgTime = micros / 100000;

    printf( "Syscalls Performed: %d\n", i );
    printf( "Total Elapsed Time: %f microseconds\n", micros );
    printf( "Average Time Per Syscall: %f microseconds\n", avgTime );

	return 0;

}
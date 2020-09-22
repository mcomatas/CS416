#include <stdlib.h>
#include <stdio.h>

#include <signal.h>

// Place any necessary global variables here
int i = 0;
struct timeval startTime, endTime;

void handle_sigfpe(int signum){

	// Handler code goes here
	i++;;
	if( i == 100000 )
	{
		gettimeofday( &endTime, NULL );

		double seconds = (endTime.tv_sec - startTime.tv_sec);
    	double micros = ((seconds * 1000000) + endTime.tv_usec) - (startTime.tv_usec);

		double avgTime = micros / 100000;

		printf( "Exceptions Occurred: %d\n", i );
		printf( "Total Elapsed Time: %f microseconds\n", micros );
		printf( "Average Time Per Exception: %f microseconds\n", avgTime );

		exit(0);
	}

}

int main(int argc, char *argv[]){

	int x = 5;
	int y = 0;
	int z = 0;
	gettimeofday( &startTime, NULL );
	signal( SIGFPE, handle_sigfpe );
	z = x / y;

	return 0;

}
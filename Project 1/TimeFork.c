#include <stdlib.h>
#include <stdio.h>

// Place any necessary global variables here

int main(int argc, char *argv[]){

	/*
	 *
	 * Implement Here
	 *
	 *
	 */

	struct timeval currTime;
    double startTime, endTime;

	int i = 0;

	gettimeofday( &currTime, NULL );
    startTime = currTime.tv_usec;
	while( i < 5000 )
	{
		if( fork() == 0 )
		{
			exit(0);
		}
		wait();
		i++;
	}
	gettimeofday( &currTime, NULL );
    endTime = currTime.tv_usec;

	double totalTime = endTime - startTime;
    double avgTime = totalTime / 5000;

	printf( "Forks Performed: %d\n", i );
	printf( "Total Elapsed Time: %f microseconds\n", totalTime );
	printf( "Average Time Per Fork: %f microseconds\n", avgTime );

	return 0;

}
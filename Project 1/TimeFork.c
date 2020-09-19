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

	struct timeval startTime, endTime;
    //double startTime, endTime;

	int i = 0;

	//gettimeofday( &currTime, NULL );
    //startTime = currTime.tv_usec;
	gettimeofday( &startTime, NULL );
	while( i < 5000 )
	{
		if( fork() == 0 )
		{
			exit(0);
		}
		wait();
		i++;
	}
	//gettimeofday( &currTime, NULL );
    //endTime = currTime.tv_usec;
	gettimeofday( &endTime, NULL );

	//double totalTime = endTime - startTime;
	double seconds = (endTime.tv_sec - startTime.tv_sec);
    double micros = ((seconds * 1000000) + endTime.tv_usec) - (startTime.tv_usec);

	//double totalTime_v2 = (endTime.tv_sec - startTime.tv_sec) + (endTime.tv_usec - startTime.tv_usec);
    double avgTime = micros / 5000;

	printf( "Forks Performed: %d\n", i );
	printf( "Total Elapsed Time: %f microseconds\n", micros );
	printf( "Average Time Per Fork: %f microseconds\n", avgTime );

	return 0;

}
#include <stdlib.h>
#include <stdio.h>

// Place any necessary global variables here

int main(int argc, char *argv[]){

	struct timeval startTime, endTime;
	int i = 0;

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
	gettimeofday( &endTime, NULL );

	double seconds = (endTime.tv_sec - startTime.tv_sec);
    double micros = ((seconds * 1000000) + endTime.tv_usec) - (startTime.tv_usec);
    double avgTime = micros / 5000;

	printf( "Forks Performed: %d\n", i );
	printf( "Total Elapsed Time: %f microseconds\n", micros );
	printf( "Average Time Per Fork: %f microseconds\n", avgTime );

	return 0;

}
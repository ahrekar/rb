#include <stdio.h>
#include "interfacing/interfacing.h"

int main(int argc, char** argv){
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec

	initializeBoard();
	initializeUSB1208();

	getWaveMeter(&wavemeterReturn);
	printf("The laser is at: %f nm (%f GHz)\n",wavemeterReturn,speedOfLight/(wavemeterReturn));

	getDetuning(&wavemeterReturn);
	printf("The laser is at(DET): %f GHz\n",wavemeterReturn);

	return 0;
}

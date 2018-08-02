#include <stdio.h>
#include "interfacing.h"

int main(int argc, char** argv){
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec

	initializeBoard();
	initializeUSB1208();

//	printf("The laser is at: %f\n", getWaveMeter());
	

	wavemeterReturn=getPumpFreq();
	printf("The pump laser is at: %f (%f)\n",wavemeterReturn,speedOfLight/(wavemeterReturn));
	wavemeterReturn=getProbeFreq();
	printf("The probe laser is at: %f (%f)\n",wavemeterReturn,speedOfLight/(wavemeterReturn));

	return 0;
}

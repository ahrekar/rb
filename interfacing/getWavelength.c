#include <stdio.h>
#include "interfacing.h"

int main(int argc, char** argv){
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec

	initializeBoard();
	initializeUSB1208();

//	printf("The laser is at: %f\n", getWaveMeter());
	

	//wavemeterReturn=getPumpFrequency(&wavemeterReturn);
	//printf("The pump laser is at: %f (%f)\n",wavemeterReturn,speedOfLight/(wavemeterReturn));
	wavemeterReturn=getProbeFrequency(&wavemeterReturn);
	printf("The probe laser is at: %f (%f)\n",wavemeterReturn,speedOfLight/(wavemeterReturn));

	return 0;
}

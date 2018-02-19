#include <stdio.h>
#include "waveMeter.h"

int main(int argc, char** argv){
	float wavemeterReturn, frequency, detuning;
	float speedOfLight=299792458; //meters/sec
	
	wavemeterReturn=getWaveMeter();

	printf("The wavelength reads: %f (%f)\n",wavemeterReturn/10000.,speedOfLight/(wavemeterReturn/10000.));

	return 0;
}

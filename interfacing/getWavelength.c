#include <stdio.h>
#include "waveMeter.h"

int main(int argc, char** argv){
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec
	
	wavemeterReturn=getPumpFreq();
	printf("The pump laser is at: %f (%f)\n",wavemeterReturn/10000.,speedOfLight/(wavemeterReturn/10000.));
	wavemeterReturn=getProbeFreq();
	printf("The probe laser is at: %f (%f)\n",wavemeterReturn/10000.,speedOfLight/(wavemeterReturn/10000.));

	return 0;
}

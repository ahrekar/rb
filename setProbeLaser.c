
/*
   program to set analog output

   RasPi connected to USB 1208LS.

   Sets analog voltage for probe laser. Uses Analog out port 0. Final output to probe laser is through 
   op-amp circuit. see page 98.  Page 99 shows calibration data.

   Usage '$ sudo ./setProbeLaser xxx' where xxx is an integer value between 0 and 1024
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>
#include "interfacing/sacherLaser.h"
#include "interfacing/USB1208.h"



int main (int argc, char *argv[])
{
	float value;

	if (argc==2) {
		value=atof(argv[1]);
	}else{
		printf("Usage '$ sudo ./setProbeLaser xxx' where xxx is a float value between 65.0 and 170.0 \n");

		value=getSacherCurrent();	
		printf("Laser is currently at %3.1f mA\n",value*1000);
		
		return 0;
	}

	if (value<0) value=0;

	if (value>170) value=170;

	initializeBoard();
	initializeUSB1208();
	initializeSacher();

	printf("Going to set laser current to: %f mA\n",value*1000);

	setSacherCurrent(value);

	value=getSacherCurrent();	
	printf("Laser is currently at %3.1f mA\n",value*1000);

	closeUSB1208();
	return 0;
}

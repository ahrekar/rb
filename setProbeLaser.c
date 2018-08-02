
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
#include "interfacing/interfacing.h"



int main (int argc, char *argv[])
{
	float value;

	if (argc==2) {
		value=atof(argv[1]);
	}else{
		printf("Usage '$ sudo ./setProbeLaser xxx' where xxx is an integer value between 0 and 1024\n");

		getVortexPiezo(&value);	
		printf("Laser is currently at %3.1f V\n",value);
		
		return 0;
	}

	if (value<0) value=0;

	if (value>1023) value=1023;

	initializeBoard();
	initializeUSB1208();

	setVortexPiezo(value);

	closeUSB1208();

	printf("Voltage %3.1f \n",value);

	return 0;
}

/*
   program to set analog output for helium target

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
#include "interfacing/RS485Devices.h"
#include "interfacing/Sorensen120.h"

#define GPIBBRIDGE1 0XC9 // the gpib bridge can have many gpib devices attached to it, so will also need the GPIB address of each
// this is the GPIB addresses of each respective instrument attached to this bridge
#define SORENSEN120 0x0C

int main (int argc, char *argv[])
{
	float value = 0;
	int i;

	if (argc==2) {
		value=atof(argv[1]);
	}else{
		printf("Usage '$ sudo ./setProbeLaser xxx' where xxx is an integer value between 0 and 1024\n");
		value=0;
	}

	if (value<0) value=0;

	if (value>120) value=120;

	initializeBoard();
	initializeUSB1208();

	setUSB1208AnalogOut(HETARGET,value);

// i=resetGPIBBridge(GPIBBRIDGE1);
//	delay(200);
//	i=initSorensen120(SORENSEN120,GPIBBRIDGE1);
//
//	i = setSorensen120Volts(value,SORENSEN120,GPIBBRIDGE1);
//	if(i!=0){
//		printf("Error setting Sorensen Code: %d\n",i);
//	}

	closeUSB1208();


	return 0;
}

/*
 * stepmotor.c
********************************************************************
GPIO 4 output -> clock source for stepper motor.  one clock transistion = 0->1 == one step
GPIO 2 output -> binary direction (1=up, 0=down)

compile
~ $ gcc -o setPlaneAngle setPlaneAngle.c -l wiringPi

execute
~ $ sudo ./setPlaneAngle angleinmrad

where steps is integer number of steps and dir is 0 or 1

 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include "interfacing/interfacing.h"

int main (int argc, char *argv[]) {

	wiringPiSetup();
	int newpos;

	if (argc ==2) {
		newpos = atoi(argv[1]);
	} else {
		printf("Usage: sudo ./setWavePlate <step location>");
		return 1;
	}
	setMotor(2,newpos);

	return 0;
}

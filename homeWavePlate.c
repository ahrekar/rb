/*
 * stepmotor.c
********************************************************************

 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include "stepperMotorControl.h"

#define DIR0 0
#define CLK0 1
#define INP0 2
#define DELAY 1500 //move slower


int main (int argc, char *argv[]) {
	homeMotor(2);
	return 0;
}

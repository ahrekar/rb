/*
 * stepmotor.c
********************************************************************

 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include "interfacing/interfacing.h"

int main (int argc, char *argv[]) {
	initializeBoard();
	initializeUSB1208();

	homeMotor(PUMP_MOTOR);

	closeUSB1208();

	return 0;
}

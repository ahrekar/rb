/*

uses the RasPi GPIO ports to bit-band a stepper motor driver.  

uses two ports (defined below) to set the direction and number of stepts.
usage:
~$ sudo ./stepmotor 100 1
steps the motor 100 steps in direction 1.
~$ sudo ./stepmotor 500 0
steps the motor 500 steps in direction 0.

compile
~$  gcc -o stepmotor stepmotor.c -l wiringPi

*/

#include <stdio.h>
#include "interfacing/interfacing.h"

int main (int argc, char *argv[]){
	unsigned short motor;

	if (argc==2){
		motor = (unsigned short) atoi(argv[1]); // which stepper motor
	} else {
		printf("Usage:  ~$sudo ./homemotor <motor(0,1,2)>\n");
		printf("                           0=polarimeter  \n");
		printf("                           1=probeAnalysis\n");
		printf("                           2=pumpQWP\n");
		motor = 3;// not part of the switch statment, so nuthing happens
	}

	initializeBoard();
	initializeUSB1208();

	homeMotor(motor);

	closeUSB1208();

	return 0;
}


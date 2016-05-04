/*

Uses the RasPi GPIO ports to bit-band a stepper motor driver.  

Uses three ports (defined below) to select the motor, direction, 
and number of stepts.

usage:
~$ sudo ./stepmotor 0 100 1
steps motor "0" 100 steps in direction 1.

~$ sudo ./stepmotor 1 500 0
steps motor "1" 500 steps in direction 0.

Currently, the motors are determined by the pins used on the
raspberry pi to control them. These are set up in the
stepperMotorControl.h file. Right now,

 Motor 0 -> Polarimeter
 Motor 1 -> Absorption Analyzer
 Motor 2 -> QWP

compile
~$  gcc -o stepmotor stepmotor.c -l wiringPi

*/

#include <stdio.h>
#include <wiringPi.h>
#include "stepperMotorControl.h"

int main (int argc, char *argv[]){
	int i, steps, dir;
	int motor;

	if (argc==4){
		motor = atoi(argv[1]); // which steper motor
		steps = atoi(argv[2]); // get steps from command line
		dir = atoi(argv[3]);  // get dir from command line
	} else {

	printf("Ussage:  ~$sudo ./stepmotor <motor(0,1,2)> <steps> <dir(0,1)>\n");
		motor = 3;// not part of the switch statment, so nuthing happens
		steps=0;
		dir=0;
	}

	moveMotor(motor,dir,steps);

	return 0;
}


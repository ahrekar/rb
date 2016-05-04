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
#include <wiringPi.h>
#include "stepperMotorControl.h"

int main (int argc, char *argv[]){
	int i, steps, dir;
	int motor;

	if (argc==2){
		motor = atoi(argv[1]); // which steper motor
	} else {

	printf("Usage:  ~$sudo ./homemotor2 <motor(0,1,2)> \n");
		motor = 3;// not part of the switch statment, so nuthing happens
		steps=0;
		dir=0;
	}

	homeMotor(motor);

return 0;
}


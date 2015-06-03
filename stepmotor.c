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

//define the GPIO ports for the clock and direction TTL signals
#define CLK 0
#define DIR 1


int main (int argc, char *argv[]){
	int i, steps, dir;
	int del;

	del = 2;
	if (argc=3){
		steps = atoi(argv[1]); // get steps from command line
		dir = atoi(argv[2]);  // get dir from command line
	} else {
		steps=0;
		dir=0;
	}

	wiringPiSetup();

	pinMode(CLK,OUTPUT); //define port CLK to be an output
	pinMode(DIR,OUTPUT); //define port DIR to be an output

	digitalWrite (DIR, dir);  // sets direction

	for (i=0;i<steps;i++){
		digitalWrite(CLK,HIGH);
		//delay(del);
		delayMicrosecondsHard(2000); // this delay will(should?) not allow other OS processes
		digitalWrite (CLK,LOW);
		//delay(del);
		delayMicrosecondsHard(2000);
	}

return 0;
}


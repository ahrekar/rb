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
#define CLK0 3
#define DIR0 4
#define HOME0 5
// these are the wiringPi GPIO numbers
#define CLK1 21
#define DIR1 26
#define HOME1 22

#define CLK2 1
#define DIR2 0
#define HOME2 2



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


	switch (motor) {
		case (0):   // this is the polarimeter
			wiringPiSetup();
			pinMode(CLK0,OUTPUT); //define port CLK to be an output
			pinMode(DIR0,OUTPUT); //define port DIR to be an output
			digitalWrite (DIR0, dir);  // sets direction
			digitalWrite (CLK0,LOW);
		for (i=0;i<steps;i++){
			digitalWrite(CLK0,HIGH);
			delayMicrosecondsHard(5000); // this delay will(should?) not allow other OS processes
			digitalWrite (CLK0,LOW);
			delayMicrosecondsHard(5000);
		}
		break;
		case (1):  // this is the absorption analyzer
			wiringPiSetup();
			pinMode(CLK1,OUTPUT); //define port CLK to be an output
			pinMode(DIR1,OUTPUT); //define port DIR to be an output
			digitalWrite (DIR1, dir);  // sets direction
			digitalWrite (CLK1,LOW);
		for (i=0;i<steps;i++){
			digitalWrite(CLK1,HIGH);
			delayMicrosecondsHard(1500); // this delay will(should?) not allow other OS processes
			digitalWrite (CLK1,LOW);
			delayMicrosecondsHard(1500);
		}
		break;
		case (2): // this will be the pump laser quarterwave plate
			wiringPiSetup();
			pinMode(CLK2,OUTPUT); //define port CLK to be an output
			pinMode(DIR2,OUTPUT); //define port DIR to be an output
			digitalWrite (DIR2, dir);  // sets direction
			digitalWrite (CLK2,LOW);
		for (i=0;i<steps;i++){
			digitalWrite(CLK2,HIGH);
			delayMicrosecondsHard(3000); // this delay will(should?) not allow other OS processes
			digitalWrite (CLK2,LOW);
			delayMicrosecondsHard(3000);
		}
		break;

	}



return 0;
}


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

	if (argc==2){
		motor = atoi(argv[1]); // which steper motor
	} else {

	printf("Ussage:  ~$sudo ./homemotor2 <motor(0,1,2)> \n");
		motor = 3;// not part of the switch statment, so nuthing happens
		steps=0;
		dir=0;
	}
			wiringPiSetup();


	switch (motor) {
		case (0):   // this is the polarimeter
			dir=0;
			pinMode(CLK0,OUTPUT); //define port CLK to be an output
			pinMode(DIR0,OUTPUT); //define port DIR to be an output
			pinMode(HOME0,INPUT);
			digitalWrite (CLK0,LOW);
		if (digitalRead(HOME0)){ // already in home
			digitalWrite (DIR0, dir);  // sets direction
			printf("already in home, reversing 100 steps...");
			fflush(stdout);
			for (i=0;i<100;i++){
			digitalWrite(CLK0,HIGH);
			delayMicrosecondsHard(2000); // this delay will(should?) not allow other OS processes
			digitalWrite (CLK0,LOW);
			delayMicrosecondsHard(2000);
			}
		}
		i=0;
		dir = 1;
		while (!digitalRead(HOME0)) {
			digitalWrite (DIR0, dir);  // sets direction
			digitalWrite(CLK0,HIGH);
			delayMicrosecondsHard(2000); // this delay will(should?) not allow other OS processes
			digitalWrite (CLK0,LOW);
			delayMicrosecondsHard(2000);
		}
		if (digitalRead(HOME0)) printf("found home \n");
		break;
		case (1):
			printf("This motor not setup for home detection\n");

		break;
		case (2):
			dir=0;
			pinMode(CLK2,OUTPUT); //define port CLK to be an output
			pinMode(DIR2,OUTPUT); //define port DIR to be an output
			pinMode(HOME2,INPUT);
			digitalWrite (CLK2,LOW);
		if (digitalRead(HOME2)){ // already in home
			digitalWrite (DIR2, dir);  // sets direction
			printf("already in home, reversing 100 steps...");
			fflush(stdout);
			for (i=0;i<100;i++){
			digitalWrite(CLK2,HIGH);
			delayMicrosecondsHard(1200); // this delay will(should?) not allow other OS processes
			digitalWrite (CLK2,LOW);
			delayMicrosecondsHard(1200);
			}
		}
		i=0;
		dir = 1;
		while (!digitalRead(HOME2)) {
			digitalWrite (DIR2, dir);  // sets direction
			digitalWrite(CLK2,HIGH);
			delayMicrosecondsHard(1200); // this delay will(should?) not allow other OS processes
			digitalWrite (CLK2,LOW);
			delayMicrosecondsHard(1200);
		}
		if (digitalRead(HOME2)) printf("found home \n");
		break;

	}



return 0;
}


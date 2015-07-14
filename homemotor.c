/*

uses the RasPi GPIO ports to bit-band a stepper motor driver.  

uses two ports (defined below) to set the direction and number of stepts.

usage:

~$  gcc -o homemotor homepmotor.c -l wiringPi


// move motor until read IR LED and PhotoNPN alignment.
// digitalRead(HOMEIN) = 0 when not aligned
// 			=1 when aligned


*/

#include <stdio.h>
#include <wiringPi.h>

//define the GPIO ports for the clock and direction TTL signals
#define CLK 0
#define DIR 1
#define HOMECTR 2
#define HOMEIN 5

int main (int argc, char *argv[]){
	int i, steps, dir;
//	int del;

/*	del = 2;
	if (argc=3){
		steps = atoi(argv[1]); // get steps from command line
		dir = atoi(argv[2]);  // get dir from command line
	} else {
		steps=0;
		dir=0;
	}
*/
	wiringPiSetup();
	pinMode(CLK,OUTPUT); //define port CLK to be an output
	pinMode(DIR,OUTPUT); //define port DIR to be an output

	pinMode(HOMECTR,OUTPUT); // pin to turn on and off the IR LED in the home mechanism
	pinMode(HOMEIN,INPUT);//pin to read status

	digitalWrite(HOMECTR,0); // turn on IR LED

	delayMicrosecondsHard(5000); // delay needed - to allow detector circuit to power up. 

	digitalWrite (DIR, 1);  // sets direction

// if happen to detect alignment already, back the motor up so that it is not aligned.
	if (digitalRead(HOMEIN)){
		printf("Started in home. Reversing motor and reestablishing home\n");
		for (i=0;steps<100;steps++){
		digitalWrite(CLK,HIGH);
		delayMicrosecondsHard(5000); // this delay will(should?) not allow other OS processes
		digitalWrite (CLK,LOW);
		delayMicrosecondsHard(5000);
		}
		if (digitalRead(HOMEIN)) printf("Error: motor failed to move\n");
	}
// move motor until read IR LED and PhotoNPN alignment. This way, the alignment is approached the same way every time.
// if steps exceeds 1300 then the hole was not found and there is something wrong.
	digitalWrite (DIR, 0);  // sets direction
	steps=0;
	while (!digitalRead(HOMEIN)  && (steps < 1300)){
		digitalWrite(CLK,HIGH);
		delayMicrosecondsHard(5000); // this delay will(should?) not allow other OS processes
		digitalWrite (CLK,LOW);
		delayMicrosecondsHard(5000);
	steps++;
	}

if (steps==1300) printf("Error: home alignment not found\n");
printf("Moved %d steps\n",steps);
digitalWrite(HOMECTR,1); //turn off IR LED

return 0;
}


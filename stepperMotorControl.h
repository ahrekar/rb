#include <stdlib.h>
#include <unistd.h>

#define CLK 0		// We will control the clock with pin 0 on the RPi
#define DIR 1		// We will control the direction with pin 1 on the RPi
#define DEL 2000	// Whenever a delay is needed, use this value
#define CWISE 0		// Clockwise =0
#define CCWISE 1	// CounterClockwise =1

void moveStepperMotorSingleStep();
void moveStepperMotorSingleStepWithDirection(int dir);
void moveStepperMotor(int dir, int steps);

void moveStepperMotorSingleStep(){
	digitalWrite(CLK,HIGH);
	delayMicrosecondsHard(DEL);
	digitalWrite(CLK,LOW);
	delayMicrosecondsHard(DEL);
}

void moveStepperMotorSingleStepWithDirection(int dir){
	digitalWrite(DIR,dir);

	moveStepperMotorSingleStep();
}

void moveStepperMotor(int dir, int steps){
	digitalWrite(DIR,dir);
	
	int i;
	for (i=0;i<steps;i++){
		moveStepperMotorSingleStep();
	}
}

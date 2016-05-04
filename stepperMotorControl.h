#include <stdlib.h>
#include <unistd.h>

#define DEL 2000	// Whenever a delay is needed, use this value

void moveMotor(int motor,int dir, int steps);
void moveStepperMotor(int p_clock, int p_dir, int p_home, int dir, int steps);

void moveMotor(int motor,int dir, int steps)
{
	int p_clock, p_dir, p_home;
	switch(motor){
		case(0):
			p_clock=3;
			p_dir=4;
			p_home=5;
		break;
		case(1):
			p_clock=21;
			p_dir=26;
			p_home=22;
		break;
		case(2):
			p_clock=1;
			p_dir=0;
			p_home=2;
		break;
	}
	moveStepperMotor(p_clock, p_dir, p_home, dir, steps);
}

void moveStepperMotor(int p_clock, int p_dir, int p_home, int dir, int steps){
	wiringPiSetup();
	pinMode(p_dir,OUTPUT);
	pinMode(p_clock,OUTPUT);
	digitalWrite(p_dir,dir);
	digitalWrite(p_clock,LOW);
	printf("Well Something is working like it should");
	
	int i;
	for (i=0;i<steps;i++){
		digitalWrite(p_clock,HIGH);
		delayMicrosecondsHard(DEL);
		digitalWrite(p_clock,LOW);
		delayMicrosecondsHard(DEL);
	}
}

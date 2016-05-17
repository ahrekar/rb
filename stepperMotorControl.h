#include <stdlib.h>
#include <unistd.h>

#define DEL 1500	// Whenever a delay is needed, use this value

// Motor 0 -> Polarimeter
// Motor 1 -> Absorption Analyzer
// Motor 2 -> QWP

void homeMotor(int motor);
void moveMotor(int motor,int dir, int steps);
void moveStepperMotor(int p_clock, int p_dir, int p_home, int dir, int steps);

void homeMotor(int motor)
{
	wiringPiSetup();
	int p_home;
	if(motor==1){
		printf("This motor not setup for home detection\n");
		return;
	}
	else if(motor==0){
		p_home=5;
	} else if(motor==2){
		p_home=2;
	} else {
		return;
	}
	pinMode(p_home,INPUT);
	if(digitalRead(p_home)){ // Already in home
		// Then move away from home and allow it 
		// to re-find it.
		printf("Already in home, reversing 100 steps...\n");
		fflush(stdout);
		moveMotor(motor,0,100);
	}
	while(!digitalRead(p_home)){
		moveMotor(motor,1,1);
	}
	printf("Found home!\n");
}

void moveMotor(int motor, int dir, int steps)
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
	
	int i;
	for (i=0;i<steps;i++){
		digitalWrite(p_clock,HIGH);
		delayMicrosecondsHard(DEL);
		digitalWrite(p_clock,LOW);
		delayMicrosecondsHard(DEL);
	}
}

#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>

#define DEL 1500	// Whenever a delay is needed, use this value

// Motor 0 -> Polarimeter
// Motor 1 -> Absorption Analyzer
// Motor 2 -> QWP
void setupMotorVariables(int motor);

void homeMotor(int motor);
void moveMotor(int motor,int dir, int steps);
void setMotor(int motor, int location);

void stepMotor(int dir, int steps);
void writeMotorPosition();

int p_clock, p_dir, p_home, p_stepsPerRevolution, p_motorPosition;
char motorPositionFileName[80];
char baseDir[80]="/home/pi/RbControl";

void setupMotorVariables(int motor){
    if(motor==0){
        p_dir=4;
        p_clock=3;
        p_home=5;
        p_stepsPerRevolution=1200;
    } else if(motor==1){
        p_dir=26;
        p_clock=21;
        p_home=22;
        p_stepsPerRevolution=350;
    } else if(motor==2){
        p_dir=0;
        p_clock=1;
        p_home=2;
        p_stepsPerRevolution=350;
    } else {
        printf("Invalid Motor Number\m");
        exit(1);
    }

    // Read in the current position of the stepperMotor
    FILE *motorPositionFile;
    sprintf(fileName,"%s/.motor%dposition",baseDir,motor);
    motorPositionFile = fopen(motorPositionFileName,"w");

    if(!motorPositionFile) {
        printf("Error: Unable to open position file.\n");
        exit(1);
    }
    fscanf(motorPositionFile,"%d", &p_motorPosition);
    fclose(motorPositionFile);
}

void homeMotor(int motor)
{
    int i;
	wiringPiSetup();
    setupMotorVariables(motor);

	if(motor==1){
		printf("This motor not setup for home detection\n");
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
        i++;
	}
    p_motorPosition=0;
    writeMotorPosition();
	printf("Found home in %d steps\n",i);
}

void moveMotor(int motor, int dir, int steps)
{
    if(!(dir==0 || dir==1)){ // Input error checking
        printf("Invalid direction provided\n");
        exit(1);
    }
    setupMotorVariables(motor);

	stepMotor(dir, steps);

    // Calculate new position.
    steps=steps%p_stepsPerRevolution; // We're only concerned
                                    // in the net change in 
                                    // position, if a full 
                                    // revolution happens,
                                    // it won't affect our
                                    // calculation of the new
                                    // position.
    if (dir==1) // Convert from number to be subtracted to a 
        steps=350-steps // number to be added.
        
    p_motorPosition+=steps;
    p_motorPosition%p_stepsPerRevolution; 
    writeMotorPosition();
}

void setMotor(int motor, int location){
    setupMotorVariables(motor);
    if(location>p_stepsPerRevolution){
        printf("Error: Location should be less than steps per revolution of motor\n");
        exit(1);
    }
    int direction,steps;
    if (location >= p_motorPosition){
        direction = 0;
        steps=location-p_motorPosition;
    } else {
        direction = 1;
        steps=p_motorPosition-location;
    }

    moveMotor(motor,direction,steps);
}

void stepMotor(int dir, int steps){
	wiringPiSetup();
       
    if(p_dir==0 && p_clock==0 && p_home==0){
        printf("Error: Motor variables not initialized\n");
        exit(1);
    }

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

void writeMotorPosition(){
    FILE *motorPositionFile;
    sprintf(fileName,"%s/.motor%dposition",baseDir,motor);
    motorPositionFile = fopen(motorPositionFileName,"w");

    fprintf(motorPositionFile,"%d",p_motorPosition);
    fclose(motorPositionFile);
}

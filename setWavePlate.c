/*
 * stepmotor.c
********************************************************************
GPIO 4 output -> clock source for stepper motor.  one clock transistion = 0->1 == one step
GPIO 2 output -> binary direction (1=up, 0=down)

compile
~ $ gcc -o setPlaneAngle setPlaneAngle.c -l wiringPi

execute
~ $ sudo ./setPlaneAngle angleinmrad

where steps is integer number of steps and dir is 0 or 1

 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>

#define DIR0 0
#define CLK0 1
#define DELAY 1500


int main (int argc, char *argv[]) {

	int i,steps,dir,limits,k,globalpos,newpos;
	FILE *fp;
	steps=10;
	dir = 1;

	if (argc ==2) {
		newpos = atoi(argv[1]);
	} else {
		return 1;
	}

if ((newpos < 0 )||( newpos > 350)){
	printf("invalid position \n");
	return 1;
}


  wiringPiSetup();  // must call this to setup  
  pinMode (DIR0, OUTPUT) ; // clock drive
  pinMode (CLK0, OUTPUT); // direction

 fp=fopen("/home/pi/globalHVsteps.dat","r");
 if (!fp){
	printf("Unable to open position file \n");
 }

fscanf(fp,"%d", &globalpos);
printf("globalpos %d \n",globalpos);
printf("newpos %d \n",newpos);
fclose(fp);
if (newpos >=globalpos){
	dir = 0;
	steps=newpos-globalpos;
	} else {
	dir = 1;
	steps=globalpos-newpos;
	}

	 digitalWrite (DIR0, dir) ;	// 1: up, 0: down
	for (i=0;i<steps;i++){
  		digitalWrite (CLK0,HIGH);
		delayMicrosecondsHard (2*DELAY);  // this delay will not allow other linux threads to execute.
	        digitalWrite (CLK0, LOW) ;	// Off
		delayMicrosecondsHard (2*DELAY);
	}


fp=fopen("/home/pi/globalHVsteps.dat","w");
	fprintf(fp,"%d",newpos);
fclose(fp);
return 0;
}

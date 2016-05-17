/*
 * stepmotor.c
********************************************************************

 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>

#define DIR0 0
#define CLK0 1
#define INP0 2
#define DELAY 1500 //move slower


int main (int argc, char *argv[]) {

	int i,steps,dir,limits,k;
	FILE *fp;
	steps=10;
	dir = 1;


	fp = fopen("/home/pi/globalHVsteps.dat","w");
	if(!fp) {
		printf("unable to open position file\n");
		return 1;
	}


  wiringPiSetup();  // must call this to setup  


  pinMode (DIR0, OUTPUT) ; // clock drive
  pinMode (CLK0, OUTPUT); // direction
	pinMode(INP0,INPUT);


	if (digitalRead(INP0)==1){  // already in  home move it +10
		digitalWrite(DIR0,0);
		printf("started in home\n");
		for (i=0;i<100;i++){
  			digitalWrite (CLK0,HIGH);
			delayMicrosecondsHard (DELAY);  // this delay will not allow other linux threads to execute.
		        digitalWrite (CLK0, LOW) ;	// Off
			delayMicrosecondsHard (DELAY);
		}
	}
	i=0;
	digitalWrite(DIR0,1);
	while ((i<350) & (digitalRead(INP0)==0)){
		digitalWrite (CLK0,HIGH);
		delayMicrosecondsHard (DELAY);  // this delay will not allow other linux threads to execute.
	        digitalWrite (CLK0, LOW) ;	// Off
		delayMicrosecondsHard (DELAY);
		i++;
	}
printf("moved %d steps \n",i);
fprintf(fp,"%d",0);
fclose(fp);
return 0;
}

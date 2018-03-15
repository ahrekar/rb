/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "kenBoard.h"
#include "vortexLaser.h"
#include "USB1208.h"

#define K617 0
#define PROBELASER 2
#define REFLASER 3
#define HOURSINDAY 24
#define MINUTESINHOUR 60
#define BUFSIZE 1024

int main (int argc, char* argv[]){

	float myTemp;
	float start,stop;
	char buffer[BUFSIZE];
	float i,volts, amps;

		// We're okay to continue.
		// variables for recording time

if (argc=3){
		initializeBoard();
		initializeUSB1208();
		initializeVortex();

start=atof(argv[1]);
stop=atof(argv[2]);

for (i=start;i<stop;i+=.1){
		setVortexPiezo(i);


	// read actual PZ volts.  This slows things down for some reason.
		//	getVortexPiezo(&volts);
		//	printf("%3.2f\t",volts);
	// jsut print i/10
		printf("%2.1f\t",i);
		//	delayMicrosecondsHard(300);
		getUSB1208AnalogIn(REFLASER,&myTemp);
		printf("ReferenceLaser: %.4f\t",myTemp);

		getUSB1208AnalogIn(PROBELASER,&myTemp);
		printf("Probe Laser%.4f\n",myTemp);


}

		closeUSB1208();

}
		return 0 ;
}

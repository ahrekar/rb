#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "probeLaserControl.h"
#include "interfacing/kenBoard.h"
#include "interfacing/USB1208.h"
//#include "interfacing/vortexLaser.h" // 2019-07-11 Moved to sacher as probe.
#include "interfacing/sacherLaser.h" // 2019-07-11 Moved to sacher as probe.
#include "interfacing/waveMeter.h"

int setProbeDetuning(float desiredDetuning){
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec
	float lineCenter=377107.463; //GHz
	float minAdjust=50; // Whatever parameter adjusts the laser frequency, indicate what the bounds on this 
						// parameter are.
	float maxAdjust=170;
	float tolerance;
	float deltaDetuning,detuningChangeRequired,returnedDetuning,adjustSetting;
	int correctlyDetuned=1;
	int maxAttempts=32;
	int attempts;


	initializeSacher();
	adjustSetting=getSacherCurrent();
	printf("Sacher Current: %d\n",adjustSetting);
	/*
	getProbeFrequency(&wavemeterReturn);
	returnedDetuning=wavemeterReturn-LINECENTER;

	printf("The probe laser is at: %6.2f (delta=%2.2f GHz)\n",wavemeterReturn,wavemeterReturn-LINECENTER);
	printf("Please wait while laser tunes to: %6.2f (delta=%2.2f)\n",desiredDetuning+LINECENTER,desiredDetuning);
	attempts=0;
	while(correctlyDetuned==1 && attempts<maxAttempts ){
		attempts++; 
		getProbeFrequency(&wavemeterReturn);
		returnedDetuning=wavemeterReturn-LINECENTER;
		detuningChangeRequired=fabs(returnedDetuning-desiredDetuning);
		if(detuningChangeRequired>12){
			deltaDetuning=10;
		}else if(detuningChangeRequired>1.2){
			deltaDetuning=1;
		}else if(detuningChangeRequired>.12){
			deltaDetuning=.1;
		}else{
			deltaDetuning=.1;
		}
		
		// Handle problem if the change in detuning results in a value
		// outside the range capable of adjusting.
		adjustSetting=getSacherCurrent();

		tolerance=.1;
		if(desiredDetuning < returnedDetuning + .1 && desiredDetuning > returnedDetuning - .1){//STOP
			//printf("The piezo is correctly tuned at %f3.1V\n",adjustSetting);
			correctlyDetuned=0;
		}else if(desiredDetuning>returnedDetuning){//Increase the detuning.
			printf("**%2.1f GHz**",returnedDetuning);
			printf(":%3.1fV -> %3.1fV  ",adjustSetting,adjustSetting+deltaDetuning);
			setVortexPiezo(adjustSetting+deltaDetuning>maxAdjust?maxAdjust:adjustSetting+deltaDetuning);
			printf(".");
		}
		else{//Decrease the detuning
			printf("**%2.1f GHz**",returnedDetuning);
			printf(":%3.1fV -> %3.1fV",adjustSetting,adjustSetting-deltaDetuning);
			setVortexPiezo(adjustSetting-deltaDetuning<minAdjust?minAdjust:adjustSetting-deltaDetuning);
			printf(".");
		}
		fflush(stdout);
	}
	*/
	printf("\n");
	return 0;
}

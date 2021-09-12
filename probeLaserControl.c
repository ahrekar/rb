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
	float minAdjust=27; // Whatever parameter adjusts the laser frequency, indicate what the bounds on this 
						// parameter are.
	float maxAdjust=36;
	float tolerance;
	float deltaDetuning,detuningChangeRequired,returnedDetuning,adjustSetting;
	int correctlyDetuned=1;
	int maxAttempts=32;
	int attempts;


	initializeSacher();
	adjustSetting=getSacherTemperature();
	
	getProbeFrequency(&wavemeterReturn);
	returnedDetuning=wavemeterReturn-LINECENTER;

	printf("PrL @ d=%+2.1f",wavemeterReturn-LINECENTER);
	printf(" -> d=%+2.1f ",desiredDetuning);
	attempts=0;
	while(correctlyDetuned==1 && attempts<maxAttempts ){
		attempts++; 
		getProbeFrequency(&wavemeterReturn);
		returnedDetuning=wavemeterReturn-LINECENTER;
		detuningChangeRequired=fabs(returnedDetuning-desiredDetuning);

		if(detuningChangeRequired>12){
			deltaDetuning=.7;
		}else if(detuningChangeRequired>1.2){
			deltaDetuning=.07;
		}else if(detuningChangeRequired>.12){
			deltaDetuning=.007;
		}else{
			deltaDetuning=.001;
		}
		
		adjustSetting=getSacherTemperature();

		tolerance=.1;
		if(desiredDetuning < returnedDetuning + tolerance && desiredDetuning > returnedDetuning - tolerance){//STOP
			//printf("The piezo is correctly tuned at %f3.1V\n",adjustSetting);
			correctlyDetuned=0;
		}else if(desiredDetuning<returnedDetuning){//Increase the detuning.
			//printf("**%2.1f GHz**",returnedDetuning);
			//printf(":%2.3fV -> %2.3fV  ",adjustSetting,adjustSetting+deltaDetuning);
			setSacherTemperature( (adjustSetting + deltaDetuning) > maxAdjust ? maxAdjust : (adjustSetting + deltaDetuning));
			printf(".");
		}
		else{//Decrease the detuning
			//printf("**%2.1f GHz**",returnedDetuning);
			//printf(":%2.3fV -> %2.3fV",adjustSetting,adjustSetting-deltaDetuning);
			setSacherTemperature( (adjustSetting - deltaDetuning) < minAdjust ? minAdjust : (adjustSetting - deltaDetuning));
			printf(".");
		}
		fflush(stdout);
	}
	
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "probeLaserControl.h"
#include "interfacing/kenBoard.h"
#include "interfacing/USB1208.h"
#include "interfacing/vortexLaser.h"
#include "interfacing/waveMeter.h"

int setProbeDetuning(float desiredDetuning){
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec
	float lineCenter=377107.463; //GHz
	float minPiezo=0;
	float maxPiezo=117.5;
	float deltaDetuning,detuningChangeRequired,returnedDetuning,piezoSetting;
	int correctlyDetuned=1;

	getProbeFrequency(&wavemeterReturn);
	returnedDetuning=wavemeterReturn-LINECENTER;

	printf("The probe laser is at: %6.2f (delta=%2.2f GHz)\n",wavemeterReturn,wavemeterReturn-LINECENTER);
	printf("Please wait while laser tunes to: %6.2f (delta=%2.2f)\n",desiredDetuning+LINECENTER,desiredDetuning);
	while(correctlyDetuned==1){
		getProbeFrequency(&wavemeterReturn);
		returnedDetuning=wavemeterReturn-LINECENTER;
		detuningChangeRequired=fabs(returnedDetuning-desiredDetuning);
		if(detuningChangeRequired>12){
			deltaDetuning=20;
		}else if(detuningChangeRequired>1.2){
			deltaDetuning=2;
		}else if(detuningChangeRequired>.12){
			deltaDetuning=.2;
		}else{
			deltaDetuning=.1;
		}
		
		// Handle problem if the change in detuning results in a value
		// outside the range capable of adjusting.
		getVortexPiezo(&piezoSetting);

		if(desiredDetuning < returnedDetuning + .05 && desiredDetuning > returnedDetuning - .05){//STOP
			//printf("The piezo is correctly tuned at %f3.1V\n",piezoSetting);
			correctlyDetuned=0;
		}else if(desiredDetuning>returnedDetuning){//Increase the detuning.
			printf("The piezo is currently at %2.1f GHz\n",returnedDetuning);
			printf("The piezo is currently at %3.1fV, increasing to %3.1fV\n",piezoSetting,piezoSetting+deltaDetuning);
			setVortexPiezo(piezoSetting+deltaDetuning>maxPiezo?maxPiezo:piezoSetting+deltaDetuning);
			printf(".");
		}
		else{//Decrease the detuning.
			printf("The piezo is currently at %2.1f GHz\n",returnedDetuning);
			printf("The piezo is currently at %3.1fV, reducing to %3.1fV\n",piezoSetting,piezoSetting-deltaDetuning);
			setVortexPiezo(piezoSetting-deltaDetuning<minPiezo?minPiezo:piezoSetting-deltaDetuning);
			printf(".");
		}
		fflush(stdout);
	}
	printf("\n");
	return 0;
}

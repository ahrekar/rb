#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "interfacing/interfacing.h"

int setProbeDetuning(float desiredDetuning);
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec
	float lineCenter=377107.463; //GHz
	float deltaDetuning,detuningChangeRequired,returnedDetuning,desiredWavelength,piezoSetting;
	int correctlyDetuned=1;

	initializeBoard();
	initializeUSB1208();

	wavemeterReturn=getProbeFreq();
	returnedDetuning=speedOfLight/(wavemeterReturn)-lineCenter;
	desiredWavelength=speedOfLight/(desiredDetuning+lineCenter);


	printf("The probe laser is at: %3.4f (delta=%2.1f GHz)\n",wavemeterReturn,returnedDetuning);
	printf("Please wait while laser tunes to: %3.4f (delta=%2.1f)\n",desiredWavelength,desiredDetuning);
	while(correctlyDetuned==1){
		wavemeterReturn=getProbeFreq();
		returnedDetuning=speedOfLight/(wavemeterReturn)-lineCenter;
		detuningChangeRequired=fabs(returnedDetuning-desiredDetuning);
		if(detuningChangeRequired>24){
			deltaDetuning=40;
		}else if(detuningChangeRequired>2.4){
			deltaDetuning=4;
		}else if(detuningChangeRequired>.24){
			deltaDetuning=.4;
		}else{
			deltaDetuning=.1;
		}

		if(desiredDetuning < returnedDetuning + .05 && desiredDetuning > returnedDetuning - .05){//STOP
			getVortexPiezo(&piezoSetting);
			//printf("The piezo is correctly tuned at %f3.1V\n",piezoSetting);
			correctlyDetuned=0;
		}else if(desiredDetuning>returnedDetuning){//Increase the detuning.
			getVortexPiezo(&piezoSetting);
			//printf("The piezo is currently at %2.1f GHz\n",returnedDetuning);
			//printf("The piezo is currently at %3.1fV, increasing to %3.1fV\n",piezoSetting,piezoSetting+.1);
			setVortexPiezo(piezoSetting+deltaDetuning);
			printf(".");
		}
		else{//Decrease the detuning.
			getVortexPiezo(&piezoSetting);
			//printf("The piezo is currently at %2.1f GHz\n",returnedDetuning);
			//printf("The piezo is currently at %3.1fV, reducing to %3.1fV\n",piezoSetting,piezoSetting-.1);
			setVortexPiezo(piezoSetting-deltaDetuning);
			printf(".");
		}
		fflush(stdout);
	}
	printf("\n");
	return 0;
}

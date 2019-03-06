#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "probeLaserControl.h"
#include "interfacing/kenBoard.h"
#include "interfacing/USB1208.h"
#include "interfacing/vortexLaser.h"
#include "interfacing/topticaLaser.h"
#include "interfacing/waveMeter.h"


int setProbeDetuning(float desiredDetuning){
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec
	float lineCenter=377107.463; //GHz
	float deltaPiezo,detuningChangeRequired,returnedDetuning,piezoSetting=45;
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
			deltaPiezo=20;
		}else if(detuningChangeRequired>1.2){
			deltaPiezo=2;
		}else if(detuningChangeRequired>.12){
			deltaPiezo=.2;
		}else{
			deltaPiezo=.1;
		}

		if(desiredDetuning < returnedDetuning + .05 && desiredDetuning > returnedDetuning - .05){//STOP
			getVortexPiezo(&piezoSetting);
			//printf("The piezo is correctly tuned at %f3.1V\n",piezoSetting);
			correctlyDetuned=0;
		}else if(desiredDetuning>returnedDetuning){//Increase the detuning.
			getVortexPiezo(&piezoSetting);
			//printf("The laser is currently at %2.1f GHz\n",returnedDetuning);
			//printf("The piezo is currently at %3.1fV, increasing to %3.1fV\n",piezoSetting,piezoSetting+.1);
			setVortexPiezo(piezoSetting+deltaPiezo);
			printf(".");
		}
		else{//Decrease the detuning.
			getVortexPiezo(&piezoSetting);
			//printf("The laser is currently at %2.1f GHz\n",returnedDetuning);
			//printf("The piezo is currently at %3.1fV, reducing to %3.1fV\n",piezoSetting,piezoSetting-.1);
			setVortexPiezo(piezoSetting-deltaPiezo);
			printf(".");
		}
		fflush(stdout);
	}
	printf("\n");
	return 0;
}

int setPumpDetuning(int laserSock, float desiredDetuning){
	float wavemeterReturn;
	float speedOfLight=299792458; //meters/sec
	float lineCenter=377107.463; //GHz
	float deltaPiezo,detuningChangeRequired,returnedDetuning,piezoSetting=55;
	float returnedWavemeter;
	float maxPiezo=70, minPiezo=41.5;
	int correctlyDetuned=1;

	getPumpFrequency(&wavemeterReturn);
	returnedDetuning=wavemeterReturn-LINECENTER;

	printf("The pump laser is at: %6.2f (delta=%2.2f GHz)\n",wavemeterReturn,wavemeterReturn-LINECENTER);
	printf("Please wait while laser tunes to: %6.2f (delta=%2.2f)\n",desiredDetuning+LINECENTER,desiredDetuning);
	while(correctlyDetuned==1){
		getPumpFrequency(&wavemeterReturn);
		returnedDetuning=wavemeterReturn-LINECENTER;
		detuningChangeRequired=fabs(returnedDetuning-desiredDetuning);
		if(detuningChangeRequired>1.2){
			deltaPiezo=1.35;
		}else if(detuningChangeRequired>.12){
			deltaPiezo=.135;
		}else{
			deltaPiezo=.0135;
		}

		if(desiredDetuning < returnedDetuning + .05 && desiredDetuning > returnedDetuning - .05){//STOP
			piezoSetting=getScanOffset(laserSock);
			//printf("The laser is correctly tuned at %f3.1V\n",piezoSetting);
			correctlyDetuned=0;
		}else if(desiredDetuning>returnedDetuning){//Increase the detuning.
			if((piezoSetting+deltaPiezo)>maxPiezo){
				deltaPiezo=fabs(maxPiezo-piezoSetting);
			}
			piezoSetting=getScanOffset(laserSock);
			printf("The piezo is currently at %2.1f GHz\n",returnedDetuning);
			printf("The piezo is currently at %3.1fV, increasing to %3.1fV\n",piezoSetting,piezoSetting+deltaPiezo);
			//setScanOffset(piezoSetting+deltaPiezo);
			printf(".");
		}
		else{//Decrease the detuning.
			if((piezoSetting-deltaPiezo)<minPiezo){
				deltaPiezo=fabs(piezoSetting-minPiezo);
			}
			piezoSetting=getScanOffset(laserSock);
			printf("The laser is currently at %2.1f GHz\n",returnedDetuning);
			printf("The piezo is currently at %3.1fV, reducing to %3.1fV\n",piezoSetting,piezoSetting-deltaPiezo);
			//setScanOffset(piezoSetting-deltaPiezo);
			printf(".");
		}
		fflush(stdout);
	}
	printf("\n");
	return 0;
}

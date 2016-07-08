/*
   Program to record polarization.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include <wiringPi.h>
#include "pmd.h"
#include "usb-1208LS.h"
#include "mathTools.h"
#include "fileTools.h"
#include "tempControl.h"
#include "stepperMotorControl.h"

#define NDAT 8
#define NREV 4
#define REVOLUTIONS 2
#define STEPSPERREV 1200
#define DATAPOINTSPERREV 75 //Options: 2,4,6,8,10,12,15,16,20,24,25,30,40,75,120,200,600
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979
#define HPCAL 28.1/960.0
#define POLMOTOR 0	// The integer that represents the polarization stepper motor is zero.
#define CCLOCKWISE 0	// 0 spins the motor counterclockwise
#define CLOCKWISE 1	// 1 spins it clockwise
#define NORMCURR 0 	// Set this to 1 to normalize the intensity with the current
#define DWELL 2		// The number of seconds to pause, letting electronics settle
#define ALPHA 0		// The constant Alpha (location of transmission axis), measured in degrees.
#define DALPHA 0 	// The uncertainty in ALPHA
#define BETA 0		// The constant Beta_0 (beginning position of QWP relative to LP) measured in degrees.
#define DBETA 0		// The uncertainty in BETA
#define DELTA 90	// The constant Delta (wave plate retardance) in degrees.
#define DDELTA 0	// The uncertainty in DELTA
#define DSTEP 0	// The uncertainty in the step size 
#define NUMSTOKES 4
#define COS 0			// Used for my fourier coefficient array. Cos stored first.
#define SIN (DATAPOINTS/2)	// Sin stored second
#define POS 0			// Used for my error array positive values are stored first.
#define NEG (DATAPOINTS)// Then negative values. 

int getPolarizationData(int numDataPoints, int numRevolutions);

int main (int argc, char **argv)
{
	int aout;
	int flag;
	int nDat, nRev;
	
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	char comments[1024];

	wiringPiSetup();


	// Variables for getting time information to identify
	// when we recorded the data
	time_t rawtime;
	struct tm * timeinfo;
	float bias, offset,energy, current;
	char* extension;
	

	// Get parameters.
	if (argc==3){
		nDat = atoi(argv[1]);
		nRev = atoi(argv[2]);
	}else {
		printf("usage '~$ sudo ./stepperMotorDiagnose <number of data points> <number of revolutions>'\n");
		return 1;
	}

	// Indicate that data is being collected.
	FILE* dataCollectionFlagFile;
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file \n");
		exit(1);
	}

	char* rawDataFileName="/home/pi/RbData/2016-07-07/stepperMotorDiagnose.dat";
	FILE* rawData;
	// Write the header for the raw data file.
	rawData=fopen(rawDataFileName,"a");
	if (!rawData) {
		printf("Unable to open file: %s\n", rawDataFileName);
		exit(1);
	}

	homeMotor(POLMOTOR);
	getPolarizationData(nDat, nRev); 
	//fprintf(rawData,"NumDataPoints\tNumRevolutions\tNumStepsToHome\n",rawDataFileName);
	fprintf(rawData,"%d\t%d\t%d\n",nDat,nRev,homeMotor(POLMOTOR));


	// Collect raw data

	fclose(rawData);
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

// INCLUDE
int getPolarizationData(int numDatapoints, int numRevolutions){

	// Variables for stepper motor control.
	int nsteps,steps,ninc,i;

	// Variables for data collections.
	int counts;
	float current;
	float currentErr;
	int nSamples = 8;
	float* measurement = calloc(nSamples,sizeof(float));


	nsteps=STEPSPERREV*numRevolutions;
	ninc=STEPSPERREV/numDatapoints; // The number of steps to take between readings.

	for (steps=0;steps<nsteps;steps+=ninc){

		//1200 steps per revoluion
		moveMotor(POLMOTOR,CLOCKWISE,ninc);

		counts=0;
		for (i=0;i<DWELL;i++){
			delay(1000);
		}

		current=0.0;
		for (i=0;i<nSamples;i++){
		}

		current = current/(float)nSamples;
		currentErr = stdDeviation(measurement,nSamples);

		printf("Steps %d\tCounts %d\tCurrent %f\n",steps,counts,current);
	}

	return 0;
}

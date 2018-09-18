/*
   Program to record excitation function. This is accomplished by 
   stepping up the voltage at the target in increments and recording
   the number of counts at each of those voltages.

   RasPi connected to USB 1204LS.


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
#include "mathTools.h"
#include "fileTools.h"
#include "interfacing/interfacing.h"
#include "probeLaserControl.h"

#define BUFSIZE 1024
#define WAITTIME 2
#define BOTLOCKIN 5
#define TOPLOCKIN 6

int main (int argc, char **argv)
{
	// Variables for finding the max and min.
	int numChannels=3;
	float involts[numChannels];
	float angle,angleErrPlus,angleErrMinus;
	float frequency;
	int channels[] = {BOTLOCKIN,TOPLOCKIN};
	int motor=PROBE_MOTOR;
	float* maxes = calloc(numChannels,sizeof(float));
	int* maxLoc = calloc(numChannels,sizeof(int));
	float* mins = calloc(numChannels,sizeof(float));
	int* minLoc = calloc(numChannels,sizeof(int));
	int j,k,nSamples=4,nAngleReadings=7,i;
	float* measurement = calloc(nSamples,sizeof(float));
	float* angleReadings = calloc(nAngleReadings,sizeof(float));

	if (argc==5) {
		mins[0]=atof(argv[1]);
		mins[1]=atof(argv[2]);
		maxes[0]=atof(argv[3]);
		maxes[1]=atof(argv[4]);
	} else {
		printf("Usage:\n");
		printf("$ sudo ./faradayScanBPD min0 min1 max0 max1\n");
		return 0;
	}
	

    // Variables for recording the time. 
	time_t rawtime;
	struct tm * timeinfo;
    int err;

	FILE *dataCollectionFlagFile, *fp;

	initializeBoard();
	initializeUSB1208();


	frequency = getProbeFrequency();// Getting the wavelength invokes a significant delay
									// So we no longer need the previous delay statement. 
	//kensWaveLength = -1;
	printf("%03.4f\n",frequency);

	angle=0;
	// grab several readings and average
	for(j=0;j<nAngleReadings;j++){
		for(k=0;k<numChannels;k++){
			involts[k]=0.0;	
			for (i=0;i<nSamples;i++){
				getMCPAnalogIn(channels[k],&measurement[i]);
				involts[k]=involts[k]+measurement[i];
				delay(10);
			}
			involts[k]=fabs(involts[k])/(float)(nSamples);
		}
		angleReadings[j]=atan(sqrt((involts[0]-mins[0])/(maxes[0]-mins[0])/(involts[1]-mins[1])*(maxes[1]-mins[1])));
		angle+=angleReadings[j];
	}
	for(k=0;k<numChannels;k++){
		involts[k]=fabs(involts[k]);
		printf("%0.4f\t",involts[k]);
	}
	printf("\n");
	angle=angle/nAngleReadings;
	printf("%02.3f\t%02.3f\n",angle,stdDeviation(angleReadings,nAngleReadings));

	closeUSB1208();

	return 0;
}

void findMaxMinIntensity(float* maxes,int* maxLoc, float* mins, int* minLoc, int* channels, int numChannels, int stepRange, int motor){
	int steps;
	int i,j;
    int stepSize=1;
    float involts=0;
    float measurement=0;
    int nSamples=8;

	homeMotor(motor);

	for(j=0;j<numChannels;j++){ // numPhotoDet1
		maxes[j]=0;
		mins[j]=100;
	}

    for (steps=0;steps < stepRange;steps+=stepSize){ // steps
        // (STEPSPERREV) in increments of STEPSIZE
        delay(150); // watching the o-scope, it looks like it takes ~100ms for the ammeter to settle after a change in LP

        //get samples and average
        for(j=0;j<numChannels;j++){ // numPhotoDet1
            involts=0;
            for (i=0;i<nSamples;i++){ // nSamples
                getMCPAnalogIn(channels[j],&measurement);
                involts=involts+fabs(measurement);
                delay(WAITTIME);
            } // nSamples
            involts=involts/(float)nSamples;
            if(maxes[j]<involts){
                maxes[j]=involts;
				maxLoc[j]=steps;
            }
            if(mins[j]>involts){
                mins[j]=involts;
				minLoc[j]=steps;
            }
        } // numPhotoDet1


        stepMotor(motor,CLK,stepSize);
    } // steps
}

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

int main (int argc, char **argv)
{
	// Variables for finding the max and min.
	int numChannels=2;
	float normInt; /*normalization of probe intensity factor*/
	float involts[numChannels];
	float angle,angleErrPlus,angleErrMinus;
	float frequency=-12;
	int channels[] = {BOTLOCKIN,TOPLOCKIN};
	int motor=PROBE_MOTOR;
	float* maxes = calloc(numChannels,sizeof(float));
	int* maxLoc = calloc(numChannels,sizeof(int));
	float* mins = calloc(numChannels,sizeof(float));
	int* minLoc = calloc(numChannels,sizeof(int));
	int j,k,nSamples=4,nAngleReadings=14,i;
	float* measurement = calloc(nSamples,sizeof(float));
	float* angleReadings = calloc(nAngleReadings,sizeof(float));
	char dailyFilename[BUFSIZE];
	char hrMinSec[BUFSIZE];
	FILE *fp;

	if (argc==1) {
		printf("DON'T FORGET TO RUN THE BALANCE PROGRAM FIRST!\n");
	} else {
		printf("Usage:\n");
		printf("$ sudo ./faradayScanBPD\n");
		return 0;
	}
	

    // Variables for recording the time. 
	time_t rawtime;
	struct tm * timeinfo;
    struct stat st = {0};

	FILE *dataCollectionFlagFile;

	//Read in the maxes and mins
	char* mmFilename="/home/pi/RbControl/.minMax";
	FILE* mmFp = fopen(mmFilename,"r");
	if (!mmFp) {
		printf("Unable to open file %s\n",mmFilename);
		exit(1);
	}
	fscanf(mmFp,"%f\n%f\n%f\n%f",&mins[0],&maxes[0],&mins[1],&maxes[1]);

	initializeBoard();
	initializeUSB1208();

    // Get file name.  Use format "FDayScan"+$DATE+$TIME+".dat"
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    strftime(dailyFilename,BUFSIZE,"/home/pi/RbData/%F",timeinfo); //INCLUDE
    strftime(hrMinSec,BUFSIZE,"%H\t%M\t%S",timeinfo); //INCLUDE
    if (stat(dailyFilename, &st) == -1){
        mkdir(dailyFilename,S_IRWXU | S_IRWXG | S_IRWXO );
    }

    strftime(dailyFilename,BUFSIZE,"/home/pi/RbData/%F/PBSCAngles%F.dat",timeinfo); //INCLUDE
    fp=fopen(dailyFilename,"a");
    if (!fp) {
        printf("Unable to open file: %s\n",dailyFilename);
        exit(1);
    }

	getUSB1208AnalogIn(REF_LASER,&normInt);
	normInt=fabs(normInt);

	//getProbeFrequency(&frequency);// Getting the wavelength invokes a significant delay
									// So we no longer need the previous delay statement. 
	//kensWaveLength = -1;
	//printf("Frequency: %03.4f\n",frequency);
	//printf("Detuning: %03.4f\n",frequency-LINECENTER);

	printf("Sig. 1\tSig. 2\n",frequency);
	angle=0;
	// grab several readings and average
	for(j=0;j<nAngleReadings;j++){
		for(k=0;k<numChannels;k++){involts[k]=0.0;}
		for(k=0;k<numChannels;k++){
			for (i=0;i<nSamples;i++){
				getMCPAnalogIn(channels[k],&measurement[i]);
				involts[k]=involts[k]+measurement[i];
				delay(30);
			}
			involts[k]=fabs(involts[k])/(float)(nSamples);
		}
		angleReadings[j]=atan(sqrt((involts[0]/normInt-mins[0])/(maxes[0]-mins[0])/(involts[1]/normInt-mins[1])*(maxes[1]-mins[1])));
		angle+=angleReadings[j];
	}
	for(k=0;k<numChannels;k++){
		involts[k]=fabs(involts[k]);
		printf("%0.4f\t",involts[k]);
	}
	printf("\n");
	angle=angle/nAngleReadings;
	printf("Angle: %02.4f\tError: %02.4f\n",angle,stdDeviation(angleReadings,nAngleReadings));
    fprintf(fp,"%s\t%f\t%f\t%f\n",hrMinSec,angle,stdDeviation(angleReadings,nAngleReadings),frequency);

	closeUSB1208();

	return 0;
}


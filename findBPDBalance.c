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
#define WAITTIME 10

void findMaxMinIntensity(char* fileName, float* maxes,int* maxLoc, float* mins, int* minLoc, int* channels, int numChannels, int stepRange, int motor, float normInt);

int main (int argc, char **argv)
{
	// Variables for finding the max and min.
	int numChannels=2;
	float involts[numChannels];
	float angle;
	float frequency;
	float returnFloat;
	float normInt; /*Normalizing intensity */
	int channels[] = {BOTLOCKIN,TOPLOCKIN};
	//int channels[] = {PUMP_LASER,PROBE_LASER,REF_LASER};
	int stepRange=87;
	int motor=PROBE_MOTOR;
	float* maxes = calloc(numChannels,sizeof(float));
	int* maxLoc = calloc(numChannels,sizeof(int));
	float* mins = calloc(numChannels,sizeof(float));
	int* minLoc = calloc(numChannels,sizeof(int));
	int k,nSamples=8,i;
	float *measurement = calloc(nSamples,sizeof(float));
	char fileName[BUFSIZE];
	char *minMaxFileName=".minMax";
	FILE *mmFp,*fp;
	

    // Variables for recording the time. 


	initializeBoard();
	initializeUSB1208();

    // Variables for recording the time. 
	time_t rawtime;
	struct tm * timeinfo;
	// get file name.  use format "FSCanBPD"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F",timeinfo);
	if (stat(fileName, &st) == -1){ // Create the directory for the Day's data 
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/FindBPDBalance%F_%H%M%S.dat",timeinfo);

	printf("\n%s\n",fileName);

	
	mmFp=fopen(minMaxFileName,"w");
	if (!mmFp) {
		printf("unable to open file: %s\n",minMaxFileName);
		exit(1);
	}

	getProbeFrequency(&frequency);// Getting the wavelength invokes a significant delay
									// So we no longer need the previous delay statement. 
									//
	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	fprintf(fp,"#Filename:\t%s",fileName);
	fprintf(fp,"\n#ProbeFrequency:\t%6.2f",frequency);
	fprintf(fp,"\n");

	fclose(fp);

	getUSB1208AnalogIn(REF_LASER,&normInt);
	normInt=fabs(normInt);

	findMaxMinIntensity(fileName, maxes, maxLoc, mins, minLoc, channels, numChannels, stepRange, motor, normInt);
	printf("Max Intensity PD1: %f\nMin Intensity PD1: %f\nMax Loc PD1: %d\nMin Loc PD1: %d\n",maxes[0]/normInt,mins[0]/normInt,maxLoc[0],minLoc[0]);
	int equalSignalLoc=(maxLoc[0]+minLoc[0])/2;
	fprintf(mmFp,"%f\n%f\n%f\n%f\n%f\n%d\n%d\n%d",mins[0]/normInt,maxes[0]/normInt,mins[1]/normInt,maxes[1]/normInt,normInt,minLoc[0],maxLoc[0],equalSignalLoc);

	homeMotor(motor);

	//stepMotor(motor,CLK,minLoc[0]);
	printf("Setting to balanced location at %d steps\n",equalSignalLoc);
	stepMotor(motor,CLK,equalSignalLoc);

	//kensWaveLength = -1;
	printf("%03.4f\n",frequency);

	for(k=0;k<numChannels;k++){
		involts[k]=0.0;	
	}

	// grab several readings and average
	for(k=0;k<numChannels;k++){
		for (i=0;i<nSamples;i++){
			getMCPAnalogIn(channels[k],&measurement[i]);
			involts[k]=involts[k]+measurement[i];
			delay(WAITTIME);
		}
		involts[k]=fabs(involts[k])/(float)nSamples;

		//printf("  %0.4f %0.4f  ",involts[k],stdDeviation(measurement,nSamples));
	}
	printf("Min0: %f, Min1: %f\n",mins[0]/normInt,mins[1]/normInt);
	printf("Max0: %f, Max1: %f\n",maxes[0]/normInt,maxes[1]/normInt);
	angle=atan(sqrt((involts[0]/normInt-mins[0])/(maxes[0]-mins[0])/(involts[1]/normInt-mins[1])*(maxes[1]-mins[1])));
	printf("%02.3f(%02.2f)\n",angle,angle*180/3.14159265);

	fclose(mmFp);
	closeUSB1208();

	return 0;
}

void findMaxMinIntensity(char* fileName, float* maxes,int* maxLoc, float* mins, int* minLoc, int* channels, int numChannels, int stepRange, int motor, float normInt){
	int steps;
	int i,j;
    int stepSize=1;
    float involts=0;
    int nSamples=8;
	float* measurement = calloc(nSamples,sizeof(float));
	FILE* fp;

	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	homeMotor(motor);

	for(j=0;j<numChannels;j++){ // numPhotoDet1
		maxes[j]=0;
		mins[j]=100;
	}
	/* I found that the first measurement was returning a much to low value,
	 * so I just make a measurement before the important ones to make sure that
	 * all the values are valid.
	 */
	for(j=0;j<numChannels;j++){ // numPhotoDet1
		for (i=0;i<nSamples;i++){ // nSamples
			getMCPAnalogIn(channels[j],&measurement[i]);
			involts=involts+fabs(measurement[i]);
			delay(WAITTIME);
		} // nSamples
	}

	fprintf(fp,"STEPS\tHORIZ\tHORIZnorm\tVERT\tVERTnorm\t");

    for (steps=0;steps < stepRange;steps+=stepSize){ // steps
		fprintf(fp,"\n");
        // (STEPSPERREV) in increments of STEPSIZE
        delay(150); // watching the o-scope, it looks like it takes ~100ms for the ammeter to settle after a change in LP

		fprintf(fp,"%03d",steps);

        //get samples and average
        for(j=0;j<numChannels;j++){ // numPhotoDet1
            involts=0;
			fprintf(fp,"\t");
            for (i=0;i<nSamples;i++){ // nSamples
                getMCPAnalogIn(channels[j],&measurement[i]);
				//if(steps==0)printf("Measurement 0: %f\n",measurement[i]);
				//fflush(stdout);
                involts=involts+fabs(measurement[i]);
                delay(WAITTIME);
            } // nSamples
            involts=involts/(float)nSamples;
			fprintf(fp,"%2.4f\t%2.4f",involts,involts/normInt);

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
	fclose(fp);
}

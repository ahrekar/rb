/*
   Program to record an absorption profile. This is accomplished by 
   scanning the frequency of the laser over the transition range 
   (roughly 377103.463 to 377113.463 GHz or -4 to +6 GHz detuning)
   while monitoring the intensity of the laser passing through a cell
   containing alkali metal.

   RasPi connected to USB 1208LS.
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
#include "interfacing/kenBoard.h" // For controlling stepper motors. 
#include "interfacing/USB1208.h" // For accessing the photodiode signals
#include "interfacing/vortexLaser.h" // For changing the detuning of the laser.
#include "interfacing/RS485Devices.h" // For talking to wavemeter, Omega, etc. 
#include "interfacing/grandvillePhillips.h" // For getting pressures. 
#include "interfacing/interfacing.h"

#define BUFSIZE 1024
#define NUMCHANNELS 3
#define WAITTIME 2

void graphData(char* fileName);
void writeFileHeader(char* fileName, char* comments);
void writeTextToFile(char* fileName, char* line);
void collectAndRecordData(char* fileName, float startvalue, float endvalue, float stepsize);
float stdDeviation(float* values, int numValues);
void findAndSetProbeMaxTransmission();
void findMaxMinIntensity(float* maxes, float* mins,int* channels, int numChannels, int stepRange);

int main (int argc, char **argv)
{
    // Variables for recording the time. 
	time_t rawtime;
	struct tm * timeinfo;
    // Variables for describing the scan.
	float startvalue,endvalue,stepsize;
    // Variables for storing text
	char fileName[BUFSIZE],comments[BUFSIZE];
    // A file used to indicate that we are collecting data.
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
    // Used to store error codes
    int err;

	FILE *dataCollectionFlagFile, *fp;

	if (argc==5) {
		startvalue=atof(argv[1]);
		endvalue=atof(argv[2]);
		stepsize=atof(argv[3]);
		strcpy(comments,argv[4]);
	} else {
		printf("Usage:\n");
		printf("$ sudo ./RbAbsorbScan <begin> <end> <step>  <comments>\n");
		printf("                      (0.0 - 117.5)                   \n");
		return 0;
	}

	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("Unable to open file: %s\n",dataCollectionFileName);
		exit(1);
	}

	initializeBoard();
	initializeUSB1208();

	if (endvalue>117.5) endvalue=117.5;
	if (startvalue>117.5) endvalue=117.5;
	if (startvalue<0) startvalue=0;
	if (endvalue<0) endvalue=0;
	if (startvalue>endvalue) {
		printf("error: startvalue > endvalue.\nYeah, i could just swap them in code.. or you could just enter them in correctly. :-)\n");
		return 1;
	}

	// get file name.  use format "RbAbs"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F",timeinfo);
	if (stat(fileName, &st) == -1){ // Create the directory for the Day's data 
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/RbAbs%F_%H%M%S.dat",timeinfo);

	printf("\n%s\n",fileName);

	writeFileHeader(fileName, comments);
	fp=fopen(fileName,"a");

	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

    err=setMirror(0);
    if(err>0) printf("Error Occured While setting Flip Mirror: %d\n",err);

	collectAndRecordData(fileName, startvalue, endvalue, stepsize);

	setVortexPiezo(45.0); // Return Piezo to 45.0 V

	closeUSB1208();

	graphData(fileName);

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

void collectAndRecordData(char* fileName, float startvalue, float endvalue, float stepsize){
	float value;
	FILE* fp;
	int k=0,i;
	int nSamples;
    int count=0;
	float involts[NUMCHANNELS];
	float kensWaveLength;

	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}
	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 16;
	float* measurement = malloc(nSamples*sizeof(float));

	value=startvalue;
	setVortexPiezo(value);
	delay(10000);

	for (value=startvalue;value < endvalue && value >= startvalue;value+=stepsize){
        if(count%15==0) printf("          \t       \t\t\tVERTICAL      |        HORIZONTAL      |        REFERENCE\n");
		setVortexPiezo(value);
		printf("VOLT %3.1f \t",value);
		fprintf(fp,"%f\t",value);

		// delay to allow transients to settle
		delay(200);
		kensWaveLength = getWaveMeter();// Getting the wavelength invokes a significant delay
                                        // So we no longer need the previous delay statement. 
		//kensWaveLength = -1;
		fprintf(fp,"%03.4f\t",kensWaveLength);
		printf("%03.4f\t",kensWaveLength);
		for(k=0;k<NUMCHANNELS;k++){
			involts[k]=0.0;	
		}


		// grab several readings and average
		for(k=1;k<NUMCHANNELS+1;k++){
			for (i=0;i<nSamples;i++){
				getUSB1208AnalogIn(k,&measurement[i]);
				involts[k-1]=involts[k-1]+measurement[i];
				delay(10);
			}
			involts[k-1]=fabs(involts[k-1])/(float)nSamples;
			fprintf(fp,"%0.4f\t%0.4f\t",involts[k-1],stdDeviation(measurement,nSamples));
			printf("  %0.4f %0.4f  ",involts[k-1],stdDeviation(measurement,nSamples));
            if(k<NUMCHANNELS) printf(" | ");
		}
		fprintf(fp,"\n");
		printf("\n");
        count++;
	}
	fprintf(fp,"\n");
	fclose(fp);
	free(measurement);
}

void writeFileHeader(char* fileName, char* comments){
	FILE* fp;
	float returnFloat;
	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	fprintf(fp,"#Filename:\t%s\n",fileName);
	fprintf(fp,"#Comments:\t%s\n",comments);

    /** Record System Stats to File **/
    /** Pressure Gauges **/
	getIonGauge(&returnFloat);
	printf("IonGauge %2.2E Torr \n",returnFloat);
	fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

	getConvectron(GP_N2_CHAN,&returnFloat);
	printf("CVGauge(N2) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(N2)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_HE_CHAN,&returnFloat);
	printf("CVGauge(He) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);

    /** Temperature Controllers **/
	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#CurrTemp(Res):\t%f\n",returnFloat);
	getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#SetTemp(Res):\t%f\n",returnFloat);

	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#CurrTemp(Targ):\t%f\n",returnFloat);
	getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#SetTemp(Targ):\t%f\n",returnFloat);

	getPVCN7500(CN_CHAMWALL,&returnFloat);
	fprintf(fp,"#CurrTemp(Res2):\t%f\n",returnFloat);
	getSVCN7500(CN_CHAMWALL,&returnFloat);
	fprintf(fp,"#SetTemp(Res2):\t%f\n",returnFloat);
    /** End System Stats Recording **/

	//fprintf(fp,"VOLT\tPUMP\tStdDev\tPROBE\tStdDev\tREF\tStdDev\n");
	fprintf(fp,"VOLT\tWAV\tVERT\tVERTsd\tHORIZ\tHORIZsd\tREF\tREFsd\n");
	fclose(fp);
}

void writeTextToFile(char* fileName, char* line){
	FILE* fp;
	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}
	fprintf(fp,"%s",line);
	fclose(fp);
}


void graphData(char* fileName){
	char fileNameBase[1024];
	char buffer[BUFSIZE];
	char* extension;
	FILE* gnuplot;
	// Create graphs for data see gnutest.c for an explanation of 
	// how this process works.
	gnuplot = popen("gnuplot","w"); 

	strcpy(fileNameBase,fileName);
	extension = strstr(fileNameBase,".dat");
	strcpy(extension,"");

	//getCommentLineFromFile(fileName,"#ProbeOffset:",buffer);
	//probeOffset=atof(buffer);
	//aoutConv=9e-6*pow(probeOffset,2)+.0012*probeOffset-.0651;
	//aoutInt=.6516*probeOffset-22.851;

	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 80,32\n");
		fprintf(gnuplot, "set output\n");			
		
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Voltage (Detuning)'\n");			
		fprintf(gnuplot, "set ylabel 'Transmitted Current'\n");			
		fprintf(gnuplot, "set yrange [-.1:*]\n");			
		fprintf(gnuplot, "set xrange [*:*]\n");			
		//fprintf(gnuplot, "set x2range [*:*]\n");			
		fprintf(gnuplot, "set x2tics nomirror\n");
		//sprintf(buffer, "plot '%s' using 1:6:7 with errorbars, '%s' using ($1*%f+%f):6:7 axes x2y1\n",fileName,fileName,aoutConv,aoutInt);
		sprintf(buffer, "plot '%s' using 1:7:8 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:5:6 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:3:4 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileNameBase);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:7:8 with errorbars,\
						 	  '%s' using 1:5:6 with errorbars,\
						 	  '%s' using 1:3:4 with errorbars\n", fileName,fileName,fileName);
		fprintf(gnuplot, buffer);
	}
	pclose(gnuplot);
}

void findAndSetProbeMaxTransmission(){
	int i;
	int numMoves=0;
	int stepsPerRevolution=350;
	int moveSize=stepsPerRevolution/4;
	int foundMax=0;
	float returnFloat;
	float maxIntensity=0;
	int numMovesBackToMax=0;
	do{
		getUSB1208AnalogIn(PROBE_LASER,&returnFloat);
		if(fabs(returnFloat)>maxIntensity){
			numMovesBackToMax=4-numMoves;
			maxIntensity=fabs(returnFloat);
		}
		stepMotor(PROBE_MOTOR,CLK,moveSize);
		numMoves++;
		if(numMoves==4){
			// Go back to the maximum
			for(i=0;i<numMovesBackToMax+1;i++){
				stepMotor(PROBE_MOTOR,CCLK,moveSize);
			}
			if(moveSize==1){
				stepMotor(PROBE_MOTOR,CLK,moveSize);
			}
			moveSize=moveSize/2;
			numMoves=0;
		}
		if(moveSize==0){
			foundMax=1;
		}
	}while(!foundMax);
}

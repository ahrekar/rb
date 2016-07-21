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
#include "interfacing/interfacing.h"
#include "mathTools.h"

#define BUFSIZE 1024
#define NUMCHANNELS 3

void graphData(char* fileName);
float stdDeviation(float* values, int numValues);

int main (int argc, char **argv)
{
	int i,startvalue,endvalue,stepsize,nSamples;
	time_t rawtime;
	struct tm * timeinfo;
	char fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	__u16 value;

	float involts[3];
	float returnFloat;
	int k=0;

	FILE *fp, *dataCollectionFlagFile;

	if (argc==5) {
		startvalue=atoi(argv[1]);
		endvalue=atoi(argv[2]);
		stepsize=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else {
		printf("Usage:\n$ sudo ./RbAbsorbScan <begin> <end> <step> <comments>\n");
		printf("                              ( 0 - 1023 )                   \n");
		return 0;
	}
	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file \n");
		exit(1);
	}

	initializeBoard();
	initializeUSB1208();

	if (endvalue>1024) endvalue=1024;
	if (startvalue>1024) endvalue=1024;
	if (startvalue<1) startvalue=0;
	if (endvalue<1) endvalue=0;
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

	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	fprintf(fp,"#Filename:\t%s\n",fileName);
	fprintf(fp,"#Comments:\t%s\n",comments);
	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#CellTemp(Res):\t%f\n",returnFloat);
	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#CellTemp(Targ):\t%f\n",returnFloat);
	fprintf(fp,"Aout\tPUMP\tStdDev\tPROBE\tStdDev\tREF\tStdDev\n");

	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 32;
	float* measurement = malloc(nSamples*sizeof(float));

	for (value=endvalue;value > startvalue && value <= endvalue;value-=stepsize){
		setUSB1208AnalogOut(PROBEOFFSET,value);
		printf("Aout %d \t",value);
		fprintf(fp,"%d\t",value);

		// delay to allow transients to settle
		delay(100);
		for(k=0;k<NUMCHANNELS;k++){
			involts[k]=0.0;	
		}

		// grab several readings and average
		for(k=0;k<NUMCHANNELS;k++){
			for (i=0;i<nSamples;i++){
				getUSB1208AnalogIn(k,&measurement[i]);
				involts[k]=involts[k]+measurement[i];
				delay(1);
			}
			involts[k]=fabs(involts[k])/(float)nSamples;
			fprintf(fp,"%f\t%f\t",involts[k],stdDeviation(measurement,nSamples));
			printf("%f\t%f\t",involts[k],stdDeviation(measurement,nSamples));
		}
		fprintf(fp,"\n");
		printf("\n");
	}
	free(measurement);

	value=(int)(1.325*617.0);
	setUSB1208AnalogOut(PROBEOFFSET,value);//sets vout such that 0 v at the probe laser

	fclose(fp);

	closeUSB1208();

	graphData(fileName);

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

void graphData(char* fileName){
	char buffer[BUFSIZE];
	FILE* gnuplot;
	// Create graphs for data see gnutest.c for an explanation of 
	// how this process works.
	gnuplot = popen("gnuplot","w"); 

	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 158,32\n");
		fprintf(gnuplot, "set output\n");			
		
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Aout (Detuning)'\n");			
		fprintf(gnuplot, "set ylabel 'Transmitted Current'\n");			
		fprintf(gnuplot, "set yrange [-.1:*]\n");			
		fprintf(gnuplot, "set xrange [*:*] reverse\n");			
		sprintf(buffer, "plot '%s' using 1:6:7 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:4:5 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:2:3 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:6:7 with errorbars,\
						 	  '%s' using 1:4:5 with errorbars,\
						 	  '%s' using 1:2:3 with errorbars\n", fileName,fileName,fileName);
		fprintf(gnuplot, buffer);
	}
	pclose(gnuplot);
}

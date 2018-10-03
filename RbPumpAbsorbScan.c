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
#include "interfacing/topticaLaser.h"
#include "interfacing/USB1208.h"
#include "interfacing/kenBoard.h"
#include "interfacing/keithley.h"

#define BUFSIZE 1024
#define NUMCHANNELS 2

void graphData(char* fileName);
void writeFileHeader(char* fileName, char* comments);
void writeTextToFile(char* fileName, char* line);
void collectAndRecordData(char* fileName, int laserSock, float startvalue, float endvalue, float stepsize);
float stdDeviation(float* values, int numValues);
void findAndSetProbeMaxTransmission();

int main (int argc, char **argv)
{
    // Variables for recording the time. 
	time_t rawtime;
	struct tm * timeinfo;
	float startvalue,endvalue,stepsize;
	char fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
    int err,laserSock;

	FILE *dataCollectionFlagFile, *fp;

	if (argc==5) {
		startvalue=atof(argv[1]);
		endvalue=atof(argv[2]);
		stepsize=atof(argv[3]);

		strcpy(comments,argv[4]);
	} else {
		printf("Usage:\n");
		printf("$ sudo ./RbPumpAbsorbScan <begin>  <end>   <step> 					<comments>\n");
		printf("                 @170 mA  (27.52 - 27.85)  (.02 for a rough scan			  \n");
		printf("                 @130 mA  (29.60 - 30.00)   .005 for a fine scan)			  \n");
		printf("                 @ 90 mA  (31.50 - 31.95)                 					  \n");
		printf("                 @ 60 mA  (33.94 - 34.50)                 					  \n");
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
	laserSock=initializeLaser();

	if (endvalue>40) endvalue=40;
	if (startvalue>40) startvalue=40;
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

    err=setMirror(8);
    if(err>0) printf("Error Occured While setting Flip Mirror: %d\n",err);

	collectAndRecordData(fileName, laserSock, startvalue, endvalue, stepsize);

	//homeMotor(PROBE_MOTOR);


	closeUSB1208();
	close(laserSock);

	graphData(fileName);

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
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
		fprintf(gnuplot, "set yrange [*:*]\n");			
		fprintf(gnuplot, "set xrange [*:*]\n");			
		//fprintf(gnuplot, "set x2range [*:*]\n");			
		fprintf(gnuplot, "set x2tics nomirror\n");
		//sprintf(buffer, "plot '%s' using 1:6:7 with errorbars, '%s' using ($1*%f+%f):6:7 axes x2y1\n",fileName,fileName,aoutConv,aoutInt);
		sprintf(buffer, "plot '%s' using 1:2:3 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileNameBase);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:2:3 with errorbars\n", fileName);
		fprintf(gnuplot, buffer);
	}
	pclose(gnuplot);
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

	/*
	getConvectron(GP_HE_CHAN,&returnFloat);
	printf("CVGauge(He) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);
	*/

    /** Temperature Controllers **/
	// Write File Header(header).
	//fprintf(fp,"VOLT\tDET\tVERT\tVERTsd\tHORIZ\tHORIZsd\n");
	fprintf(fp,"VOLT\tDET\tVERT\tVERTsd\tHORIZ\tHORIZsd\n");
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

void collectAndRecordData(char* fileName, int laserSock, float startvalue, float endvalue, float stepsize){
	float value;
	FILE* fp;
	int k=0,i;
	int nSamples;
    int count=0;
	float involts[NUMCHANNELS];
	float detuning;
	int startChannel=1;

	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}
	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 8;
	float* measurement = malloc(nSamples*sizeof(float));

	value=startvalue;
	setScanOffset(laserSock, value);
	delay(5000);

	for (value=startvalue;value < endvalue && value >= startvalue;value+=stepsize){
		setScanOffset(laserSock, value);
		printf("VOLT %2.3f \t",value);
		fprintf(fp,"%f\t",value);

		getDetuning(&detuning);
		printf("DET %2.3f \t",detuning);
		fprintf(fp,"%f\t",detuning);

		for(k=0;k<NUMCHANNELS;k++){
			involts[k]=0.0;	
		}


		// grab several readings and average
		for(k=startChannel;k<startChannel+NUMCHANNELS;k++){
			for (i=0;i<nSamples;i++){
				//getUSB1208AnalogIn(k,&measurement[i]);
				getMCPAnalogIn(k,&measurement[i]);
				involts[k-1]=involts[k-1]+measurement[i];
				delay(50);
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
	fclose(fp);
	free(measurement);
}

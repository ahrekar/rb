/*
   Program to record polarization.
   RasPi connected to USB 1208LS.

   FARADAY SCAN


   use Aout 0 to set laser wavelength. see page 98-100
   usage
   $ sudo ./faradayscan <aoutstart> <aoutstop> <deltaaout> <comments_no_spaces>


   2015-12-31
   added error calculations. see page 5 and 6 of "FALL15" lab book
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
#include "mathTools.h" //includes stdDeviation
#include "faradayScanAnalysisTools.h"

#define PI 3.14159265358979
#define NUMSTEPS 350	
#define WAITTIME 2

#define BUFSIZE 1024

int main (int argc, char **argv)
{
    int runs, revolutions, dataPointsPerRev;
	int leftDataExclude=0, rightDataExclude=0;
	int interactive;
	char fileName[1024];
	if (argc==3){
		strcpy(fileName,argv[1]);
		interactive = atoi(argv[2]);
	} else { 
		printf("usage '~$ sudo ./faradayscanAnalysis <fileName> <0 or 1 (1 to remove datapoints)>'\n");
		return 1;
	}

    runs=getRuns(fileName);
    revolutions=getRevPerRun(fileName);
    dataPointsPerRev=getStepsPerRev(fileName);
  

	printf("Processing Data...\n");
	analyzeData(fileName, runs,revolutions,dataPointsPerRev);

	printf("Plotting Data...\n");
	char* extensionStart=strstr(fileName,".dat");
	strcpy(extensionStart,"RotationAnalysis.dat");
	plotData(fileName);

	if (interactive){
		printf("How many data points on left to exclude?\n");
		scanf("%d",&leftDataExclude);
		printf("How many data points on right to exclude?\n");
		scanf("%d",&rightDataExclude);
	}

    //printf("Calculating number density...\n");
	//calculateNumberDensity(fileName,leftDataExclude,rightDataExclude);

	//printf("Recording number density to file...\n");
	//recordNumberDensity(fileName);

	return 0;
}

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
#define STEPSIZE 25
#define STEPSPERREV 350.0
#define WAITTIME 2

#define BUFSIZE 1024

int main (int argc, char **argv)
{
	char fileName[1024];
	if (argc==2){
		strcpy(fileName,argv[1]);
	} else { 
		printf("usage '~$ sudo ./faradayscan <fileName>'\n");
		return 1;
	}

	int dataPointsPerRev=14;
	int revolutions=1;
	printf("Processing Data...\n");
	float probeOffset = 52.0;
	analyzeData(fileName, probeOffset, dataPointsPerRev,revolutions);

	printf("Plotting Data...\n");
	char* extensionStart=strstr(fileName,".dat");
	strcpy(extensionStart,"Analysis.dat");
	plotData(fileName);

	printf("Calculating number density...\n");
	calculateNumberDensity(fileName);

	printf("Recording number density to file...\n");
	recordNumberDensity(fileName);

	return 0;
}

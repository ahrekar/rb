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
#include "fileTools.h" //includes stdDeviation
#include "faradayScanAnalysisTools.h"

#define PI 3.14159265358979
#define NUMSTEPS 350	
#define WAITTIME 2

#define BUFSIZE 1024

int main (int argc, char **argv)
{
    int runs, revolutions, dataPointsPerRev;
	char fileName[1024],buffer[1024];
	if (argc==2){
		strcpy(fileName,argv[1]);
	} else { 
		printf("usage '~$ sudo ./faradayscanAnalysis <fileName>'\n");
		return 1;
	}

    getCommentLineFromFile(fileName,"#NumVolts:",buffer);
    runs=atoi(buffer);
    getCommentLineFromFile(fileName,"#Revolutions:",buffer);
    revolutions=atoi(buffer);
    getCommentLineFromFile(fileName,"#DataPointsPerRev:",buffer);
    dataPointsPerRev=atoi(buffer);
    printf("COMMENT LINES: NUMVOLT=%d, REV=%d, DPPR=%d",runs,revolutions,dataPointsPerRev);
  

	printf("Processing Data...");
	analyzeData(fileName, runs,revolutions,dataPointsPerRev,FOI);

	printf("Plotting Data...\n");
	char* extensionStart=strstr(fileName,".dat");
	strcpy(extensionStart,"RotationAnalysis.dat");
	plotData(fileName);

	return 0;
}

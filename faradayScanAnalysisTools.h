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

#define FOI 4

int plotData(char* fileName);
int plotRawData(char* fileName);
int calculateNumberDensity(char* fileName,int leftDataExclude, int rightDataExclude);
int recordNumberDensity(char* fileName);
int analyzeData(char* fileName, int runs, int revolutions, int dataPointsPerRev,int frequencyOfInterest);
int getNumberOfAouts(char* fileName);
int getRevPerRun(char* filename);
int getRuns(char* filename);
int getStepsPerRev(char* filename);

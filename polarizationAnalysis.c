/*
   Program to record polarization.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
*/
#include "fileTools.h"
#include "polarizationAnalysisTools.h"
#ifndef DEFINITIONS_H
#define DEFINITIONS_H
	#include "mathTools.h"
#endif

int main (int argc, char **argv)
{
	char analysisFileName[1024],backgroundFileName[1024],rawDataFileName[1024]; 
	char* extensionStart;
	int normalizeWithCurrent;

	// Get parameters.
	if (argc==3){
		strcpy(rawDataFileName,argv[1]);
		strcpy(analysisFileName,argv[1]);
		normalizeWithCurrent=atoi(argv[2]);
		strcpy(backgroundFileName,"NONE");
	} else if(argc==4){
		strcpy(backgroundFileName,argv[1]);
		strcpy(rawDataFileName,argv[2]);
		strcpy(analysisFileName,argv[2]);
		normalizeWithCurrent=atoi(argv[3]);
	}else {
		printf("You put in %d argument(s) \n",argc);
		printf("There are two options for using this program: \n\n");
		printf("usage '~$ ./polarizationAnalysis <data file> <normalize with current (0=no, 1=yes)>'\n");
		printf("usage '~$ ./polarizationAnalysis <background file> <data file> <normalize with current (0=no, 1=yes)>\n");
		return 1;
	}
	extensionStart=strstr(analysisFileName,".dat");
	strcpy(extensionStart,"analysis.dat");


	processFileWithBackground(analysisFileName, backgroundFileName, rawDataFileName, DATAPOINTSPERREV, REVOLUTIONS, normalizeWithCurrent); 

	return 0;
}

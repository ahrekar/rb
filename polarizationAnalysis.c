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
	char analysisFileName[80],backgroundFileName[80],rawDataFileName[80]; 
	char* extensionStart;

	// Get parameters.
	if (argc==2){
		strcpy(rawDataFileName,argv[1]);
		strcpy(analysisFileName,argv[1]);
		strcpy(backgroundFileName,"NONE");
	} else if(argc==3){
		strcpy(backgroundFileName,argv[1]);
		strcpy(rawDataFileName,argv[2]);
		strcpy(analysisFileName,argv[2]);
	}else {
		printf("You put in %d argument(s) \n",argc);
		printf("There are two options for using this program: \n\n");
		printf("usage '~$ ./polarizationAnalysis <data file>'\n");
		printf("usage '~$ ./polarizationAnalysis <background file> <data file>\n");
		return 1;
	}
	extensionStart=strstr(analysisFileName,".dat");
	strcpy(extensionStart,"analysis.dat");

	processFileWithBackground(analysisFileName, backgroundFileName, rawDataFileName, DATAPOINTSPERREV, REVOLUTIONS, "No_longer_supports_comments"); 

	return 0;
}

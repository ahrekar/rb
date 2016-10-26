/*
   Program to record polarization.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
*/
#include "fileTools.h"
#include "polarizationAnalysisTools.h"
#include <math.h>
#ifndef DEFINITIONS_H
#define DEFINITIONS_H
	#include "mathTools.h"
#endif

int main (int argc, char **argv)
{
	char analysisFileName[1024]; 
	char filesFileName[1024];
	char* extensionStart;
	int normalizeWithCurrent=1;

	// Get parameters.
	if (argc==2){
		strcpy(filesFileName,argv[1]);
	} else {
		printf("You put in %d argument(s) \n",argc);
		printf("There is one option for using this program: \n\n");
		printf("usage '~$ ./polarizationAnalysis <File containing list of files>'\n");
		return 1;
	}

	int numRuns=5;
	int typesOfRun=4;
	int backgroundFile=0;
	int piFile=1;
	//int sPlusFile=2;
	//int sMinusFile=3;
	char runTypes[4][80];
	strcpy(runTypes[0],"Background");
	strcpy(runTypes[1],"Pi Pump");
	strcpy(runTypes[2],"S+ Pump");
	strcpy(runTypes[3],"S- Pump");

	int totalDatapoints = DATAPOINTSPERREV * REVOLUTIONS;
	
	float* avgFcCos = calloc(totalDatapoints/2,sizeof(float));
	float* avgFcSin = calloc(totalDatapoints/2,sizeof(float));
	float* avgFcCosErr = calloc(totalDatapoints,sizeof(float));
	float* avgFcSinErr = calloc(totalDatapoints,sizeof(float)); 

	float* fcCos = calloc(totalDatapoints/2,sizeof(float));
	float* fcSin = calloc(totalDatapoints/2,sizeof(float));
	float* fcCosErr = calloc(totalDatapoints,sizeof(float)); 	
	float* fcSinErr = calloc(totalDatapoints,sizeof(float)); 

	float* bkFcCos = calloc(totalDatapoints/2,sizeof(float));
	float* bkFcSin = calloc(totalDatapoints/2,sizeof(float));
	float* bkFcCosErr = calloc(totalDatapoints,sizeof(float)); 	
	float* bkFcSinErr = calloc(totalDatapoints,sizeof(float)); 
	float avgCurrent, avgCurrentStdDev;

	float* stokesParameters = calloc(NUMSTOKES,sizeof(float));
	float* spErr = calloc(NUMSTOKES*2,sizeof(float));

	int totalNumberOfFiles = numRuns * typesOfRun;
	printf("Total number of files %d \n",totalNumberOfFiles);
	FILE* filesFile = fopen(filesFileName,"r");
	if (!filesFile) {
		printf("Unable to open file %s\n",filesFileName);
		exit(1);
	}
	// Create an array of files from the filesFile.
	char fileNames[20][1024];
	char tempAnalysis[1024];
	//char** fileNames = calloc(totalNumberOfFiles,sizeof(char)*1024);
	int i,j,k;
	for(i=0;i<totalNumberOfFiles;i++){ 
		fscanf(filesFile,"%1023[^\n]\n",fileNames[i]);
	}

	strcpy(analysisFileName,fileNames[0]);
	extensionStart=strstr(analysisFileName,".dat");
	strcpy(extensionStart,"ScriptAnalysis.dat");

	writeDataSummaryFileHeader(analysisFileName, "PolarizationScriptAnalysis", fileNames[0], "Data file is first file in series");
	writeDataSummaryFileColumnHeadings(analysisFileName);
	
	for (i=piFile; i < typesOfRun; i++){ 
		for (j=0; j < numRuns; j++){
			strcpy(tempAnalysis,fileNames[i+j*typesOfRun]);
			extensionStart=strstr(tempAnalysis,".dat");
			strcpy(extensionStart,"analysis.dat");
			
			processFileWithBackground(tempAnalysis, fileNames[backgroundFile+j*typesOfRun], fileNames[i+j*typesOfRun], DATAPOINTSPERREV, REVOLUTIONS, normalizeWithCurrent); 
			calculateFourierCoefficients(fileNames[backgroundFile+j*typesOfRun], DATAPOINTSPERREV, REVOLUTIONS, normalizeWithCurrent,bkFcCos,bkFcCosErr,bkFcSin,bkFcSinErr,&avgCurrent,&avgCurrentStdDev);
			calculateFourierCoefficients(fileNames[i+j*typesOfRun], DATAPOINTSPERREV, REVOLUTIONS, normalizeWithCurrent,fcCos,fcCosErr,fcSin,fcSinErr,&avgCurrent,&avgCurrentStdDev);
			for (k=0; k < totalDatapoints/2; k++){
				fcCos[k]-=bkFcCos[k];
				fcSin[k]-=bkFcSin[k];
				fcCosErr[POS+k]=sqrt(pow(fcCosErr[POS+k],2)+pow(bkFcCosErr[POS+k],2));
				fcCosErr[NEG+k]=sqrt(pow(fcCosErr[NEG+k],2)+pow(bkFcCosErr[NEG+k],2));
				fcSinErr[POS+k]=sqrt(pow(fcSinErr[POS+k],2)+pow(bkFcSinErr[POS+k],2));
				fcSinErr[NEG+k]=sqrt(pow(fcSinErr[NEG+k],2)+pow(bkFcSinErr[NEG+k],2));

				avgFcCos[k]+=fcCos[k]/numRuns;
				avgFcSin[k]+=fcSin[k]/numRuns;
				avgFcCosErr[POS+k]+=fcCosErr[POS+k]/numRuns;
				avgFcCosErr[NEG+k]+=fcCosErr[NEG+k]/numRuns;
				avgFcSinErr[POS+k]+=fcSinErr[POS+k]/numRuns;
				avgFcSinErr[NEG+k]+=fcSinErr[NEG+k]/numRuns;
			}
			//?Record the individual run results to a file?
		}
		//Calculate the Stokes Parameters from the FC
		printf("Calculating Stokes: %s\n",runTypes[i]);
		calculateStokesFromFC(fcCos,fcCosErr,fcSin,fcSinErr,stokesParameters,spErr);
			
		printf("Writing Data to file: %s\n",runTypes[i]);
		//Record the average values to file.
		writeDataSummaryFileEntry(analysisFileName,"Group Polarization Run",fileNames[0],
								normalizeWithCurrent,runTypes[i],
								avgFcCos,avgFcCosErr,avgFcSin,avgFcSinErr,
								stokesParameters,spErr,
								0,0);

		
		//Zero the average values.
		for (k=0; k < totalDatapoints/2; k++){
			avgFcCos[k]=0;
			avgFcSin[k]=0;
			avgFcCosErr[POS+k]=0;
			avgFcCosErr[NEG+k]=0;
			avgFcSinErr[POS+k]=0;
			avgFcSinErr[NEG+k]=0;
		}
	}

	free(avgFcCos);
	free(avgFcSin);
	free(avgFcCosErr);
	free(avgFcSinErr);

	free(fcCos);
	free(fcSin);
	free(fcCosErr);
	free(fcSinErr);

	free(bkFcCos);
	free(bkFcSin);
	free(bkFcCosErr);
	free(bkFcSinErr);
	return 0;
}

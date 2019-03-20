/*
   Program to record polarization.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
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
#include <wiringPi.h>
#include "fileTools.h"
#include "polarizationAnalysisTools.h"
#include "interfacing/interfacing.h"
#include "interfacing/RS485Devices.h"
#include "interfacing/Sorensen120.h"
#ifndef DEFINITIONS_H
#define DEFINITIONS_H
	#include "mathTools.h"
#endif
#define UNDENIABLESHIFT 10 // Sometimes one stepper motor position can be the same as another, we know for 
							// sure that we won't mistake two angles that are 10 steps from each other
							// for each other though.
#define VALLEY1 120
#define PEAK1 	270
#define VALLEY2 420
#define PEAK2 	570
#define VALLEY3 720
#define PEAK3 	870
#define VALLEY4 1020
#define PEAK4 	1170
#define STEPSPERREV 1200
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979

int getPolarizationData(char* fileName, int VHe, int dwell, int nMeasurements, float leakageCurrent);
void plotCommand(FILE* gnuplot, char* fileName, char* buffer);
void plotData(char* fileName);

int main (int argc, char **argv)
{
	int i,dwell;
	float VHe;
	float leakageCurrent;
    int ammeterScale,nMeasurements;
	
	char analysisFileName[80],backgroundFileName[80],rawDataFileName[80],comments[1024]; 
	char buffer[1024];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 


	// Variables for getting time information to identify
	// when we recorded the data
	time_t rawtime;
	struct tm * timeinfo;
	float returnFloat;
	char* extension;
	
	// Setup time variables
	time(&rawtime); 
	timeinfo=localtime(&rawtime); 
	struct stat st = {0};

	// Get parameters.
	if (argc==7){
		VHe=atoi(argv[1]);
		dwell=atoi(argv[2]);
		nMeasurements=atoi(argv[3]);
		ammeterScale=atof(argv[4]);
		leakageCurrent=atof(argv[5]);
		strcpy(comments,argv[6]);
		strcpy(backgroundFileName,"NONE");
	} else {
		printf("There is one option for using this program: \n\n");
		printf("usage '~$ sudo ./quickPolarization <VHe> (0-1023)\n");
		printf("                                   <dwell>             (1-5)s  \n");
		printf("                                   <nMeasurements>             \n");
		printf("                                   <ammeterScale>      (assumed neg.)\n");
		printf("                                   <leakageCurrent>    (just mantissa, assumed neg.)\n");
		printf("                                   <comments_in_double_quotes>' \n");
		return 1;
	}

	// Indicate that data is being collected.
	FILE* dataCollectionFlagFile;
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file \n");
		exit(1);
	}

	initializeBoard();
	initializeUSB1208();
	
	// Set up GPIB devices.
	i=resetGPIBBridge(GPIBBRIDGE1);
	if(i!=0){
		printf("Error resetting GPIB Bridge. Code: %d\n",i);
	}
	delay(200);
	i=initSorensen120(SORENSEN120,GPIBBRIDGE1);
	if(i!=0){
		printf("Error Initializing Sorensen. Code: %d\n",i);
	}


	// RUDAMENTARIY ERROR CHECKING
	if (VHe<0) VHe=0;
	if (VHe>120) VHe=120;

	// Create Directory for the day
	strftime(analysisFileName,80,"/home/pi/RbData/%F",timeinfo); 
	if (stat(analysisFileName, &st) == -1){
		mkdir(analysisFileName,S_IRWXU | S_IRWXG | S_IRWXO );
		sprintf(buffer,"%s/img",analysisFileName); 
		mkdir(analysisFileName,S_IRWXU | S_IRWXG | S_IRWXO );
		extension = strstr(buffer,"/img");
		strcpy(extension,"/anl");
		mkdir(analysisFileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	// Create file name.  Use format "EX"+$DATE+$TIME+".dat"
	strftime(rawDataFileName,80,"/home/pi/RbData/%F/QPOL%F_%H%M%S.dat",timeinfo); 
	strcpy(analysisFileName,rawDataFileName);
	extension = strstr(analysisFileName,".dat");
	strcpy(extension,"analysis.dat");

	printf("\n%s\n",analysisFileName);
	FILE* rawData;
	// Write the header for the raw data file.
	rawData=fopen(rawDataFileName,"w");
	if (!rawData) {
		printf("Unable to open file: %s\n", rawDataFileName);
		exit(1);
	}

	fprintf(rawData,"#File\t%s\n",rawDataFileName);
	fprintf(rawData,"#Comments\t%s\n",comments);
	printf("Comments:\t%s\n",comments);

	getIonGauge(&returnFloat);
	printf("IonGauge %2.2E Torr \n",returnFloat);
	fprintf(rawData,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

	getConvectron(GP_N2_CHAN,&returnFloat);
	printf("CVGauge(N2) %2.2E Torr\n", returnFloat);
	fprintf(rawData,"#CVGauge(N2)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_HE_CHAN,&returnFloat);
	printf("CVGauge(He) %2.2E Torr\n", returnFloat);
	fprintf(rawData,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);


	returnFloat=-1.0;
	//getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(rawData,"#CURTEMP_R(degC):\t%f\n",returnFloat);
	//getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(rawData,"#SETTEMP_R(degC):\t%f\n",returnFloat);
	//getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(rawData,"#CURTEMP_T(degC):\t%f\n",returnFloat);
	//getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(rawData,"#SETTEMP_T(degC):\t%f\n",returnFloat);

	fprintf(rawData,"#V_He:\t%f\n",VHe);
	fprintf(rawData,"#LEAKCURR:\t%f\n",leakageCurrent);
	fprintf(rawData,"#SCALE:\t%d\n",ammeterScale);
	fprintf(rawData,"#REV:\t%d\n",REVOLUTIONS);
	fprintf(rawData,"#DATAPPR:\t%d\n",DATAPOINTSPERREV);
	fprintf(rawData,"#STPPERREV:\t%d\n",STEPSPERREV);
	fprintf(rawData,"#DATPTS:\t%d\n",DATAPOINTS);
	fprintf(rawData,"#DWELL(s):\t%d\n",dwell);

	fclose(rawData);

	// Collect raw data
	getPolarizationData(rawDataFileName, VHe, dwell, nMeasurements, leakageCurrent); 

	closeUSB1208();

	// Remove the file indicating that we are taking data.
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

int getCountsAndCurrent(int dwell,long *sumCounts,float *avgCurrent){
	long returnCounts;
	int i;
	float returnCurrent;

	*avgCurrent=0.0;
	*sumCounts=0;
	for(i=0;i<dwell;i++){
		//writeRS485to232Bridge("READ?",echoData,0xCA);
		//current += atof(echoData);

		getUSB1208AnalogIn(0,&returnCurrent);
		*avgCurrent += returnCurrent/dwell;

		getUSB1208Counter(10,&returnCounts);
		*sumCounts += returnCounts;
	}

	return 0;
}

int getPolarizationData(char* fileName, int VHe, int dwell, int nMeasurements, float leakageCurrent){
	// Variables for stepper motor control.
	int j,i;

	// Variables for data collections.
	long sumCounts;
	float current;

	// Write Aout for He traget here
    i = setSorensen120Volts(VHe,SORENSEN120,GPIBBRIDGE1);
	if(i!=0){
		printf("Error setting Sorensen. Code: %d\n",i);
	}
	// NOTE THAT THIS SETS THE FINAL ELECTRON ENERGY. THIS ALSO DEPENDS ON BIAS AND TARGET OFFSET.  AN EXCIATION FN WILL TELL THE
	// USER WHAT OUT TO USE, OR JUST MANUALLY SET THE TARGET OFFSET FOR THE DESIRED ENERGY

	// Begin File setup
	FILE* rawData=fopen(fileName,"a");
	if (!rawData) {
		printf("Unable to open file: %s\n",fileName);
		exit(1);
	}
	// End File setup

	fprintf(rawData,"MEASUREMENT\tCOUNT+45\tCURRENT+45\tCOUNT-45\tCURRENT-45\n");// This line withough a comment is vital for being able to quickly process data. DON'T REMOVE
	printf("Steps\tCounts\tCurrent\n");

	for (j=0;j<nMeasurements;j++){
		homeMotor(POL_MOTOR); 
		stepMotor(POL_MOTOR,CCLK,VALLEY1); 
		getCountsAndCurrent(dwell,&sumCounts,&current);
		printf("%d\t%ld\t%e\t",j,sumCounts,current+leakageCurrent);
		fprintf(rawData,"%d\t%ld\t%e\t",j,sumCounts,current+leakageCurrent);

		homeMotor(POL_MOTOR); 
		stepMotor(POL_MOTOR,CLK,STEPSPERREV-VALLEY4); 
		getCountsAndCurrent(dwell,&sumCounts,&current);
		printf("%ld\t%e\n",sumCounts,current+leakageCurrent);
		fprintf(rawData,"%ld\t%e\n",sumCounts,current+leakageCurrent);
		stepMotor(POL_MOTOR,CCLK,STEPSPERREV-VALLEY4+UNDENIABLESHIFT); 
	}
	fclose(rawData);

	// Reset Helium Target Offset back to zero
    i = setSorensen120Volts(VHe,SORENSEN120,GPIBBRIDGE1);
	if(i!=0){
		printf("Error setting Sorensen. Code: %d\n",i);
	}

	return 0;
}

void plotData(char* fileName){
	FILE* gnuplot;
	char buffer[1024];
	char fileNameBase[1024];
	char* extension;
	// Create rough graphs of data.
	gnuplot = popen("gnuplot","w"); 

	strcpy(fileNameBase,fileName);
	extension = strstr(fileNameBase,".dat");
	strcpy(extension,"");

	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 100,32\n");
		fprintf(gnuplot, "set output\n");			
		sprintf(buffer, "set title '%s'\n", fileNameBase);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Step'\n");			
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		fprintf(gnuplot, "set yrange [*:*]\n");			
		plotCommand(gnuplot, fileName,buffer);
	}
	pclose(gnuplot);
}

void plotCommand(FILE* gnuplot, char* fileName, char* buffer){
	sprintf(buffer, "plot '%s' using 1:2\n",fileName);
	fprintf(gnuplot, buffer);
}

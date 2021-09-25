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

#include "interfacing/K617meter.h"
#include "interfacing/K6485meter.h"

#define GPIBBRIDGE1 0XC9 // the gpib bridge can have many gpib devices attached to it, so will also need the GPIB address of each
// this is the GPIB addresses of each respective instrument attached to this bridge
#define SORENSEN120 0x0C
#ifndef DEFINITIONS_H
#define DEFINITIONS_H
	#include "mathTools.h"
#endif
#define UNDENIABLESHIFT 10 // Sometimes one stepper motor position can be the same as another, we know for 
							// sure that we won't mistake two angles that are 10 steps from each other
							// for each other though.
#define VALLEY1 75
#define PEAK1 	225
#define VALLEY2 375
#define PEAK2 	525
#define VALLEY3 675
#define PEAK3 	825
#define VALLEY4 975
#define PEAK4 	1125
#define STEPSPERREV 1200
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979

int getPolarizationData(char* fileName, float VHe, int dwell, int nMeasurements, float leakageCurrent, int scale);
void plotCommand(FILE* gnuplot, char* fileName, char* buffer);
void plotData(char* fileName);

int main (int argc, char **argv)
{
	int dwell;
	float VHe;
	float leakageCurrent;
    int ammeterScale,nMeasurements;
	
	char rawDataFileName[80],comments[1024]; 
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
	
	// RUDAMENTARIY ERROR CHECKING
	if (VHe<0) VHe=0;
	if (VHe>180) VHe=180;

	// Create Directory for the day
	strftime(rawDataFileName,80,"/home/pi/RbData/%F",timeinfo); 
	if (stat(rawDataFileName, &st) == -1){
		mkdir(rawDataFileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	// Create file name.  Use format "QPOL"+$DATE+$TIME+".dat"
	strftime(rawDataFileName,80,"/home/pi/RbData/%F/QPOL%F_%H%M%S.dat",timeinfo); 

	printf("\n");
	printf("-------------------------\n");
	printf("|%s|\n", rawDataFileName);
	printf("-------------------------\n");
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

	getConvectron(GP_TOP2,&returnFloat);
	printf("CVGauge(Source Foreline): %2.2E Torr\n", returnFloat);
	fprintf(rawData,"#CVGauge(Source Foreline)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_TOP1,&returnFloat);
	printf("CVGauge(Target Foreline): %2.2E Torr\n", returnFloat);
	fprintf(rawData,"#CVGauge(Target Foreline)(Torr):\t%2.2E\n", returnFloat);


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
	getPolarizationData(rawDataFileName, VHe, dwell, nMeasurements, leakageCurrent, ammeterScale); 

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

int getPolarizationData(char* fileName, float VHe, int dwell, int nMeasurements, float leakageCurrent, int scale){
	// Variables for stepper motor control.
	int j, i;
	int retryCounter;

	// Variables for data collections.
	long sumCounts;
	long allCountsPlus=0;
	long allCountsMinus=0;
	float current=0;
	float countRatePlus=0;
	float countRateMinus=0;
	float sorensenValue, hpValue;

	if (VHe<0)
    {
        VHe=-1;
        sorensenValue=0;
    }
	else if (VHe < 60)
	{
        sorensenValue=0;
	}
	else if (VHe < 180)
	{
		sorensenValue=VHe-60;
	}
	else 
    {
        VHe=180;
        sorensenValue=120;
    };
    hpValue=VHe-sorensenValue;

    // If VHe is negative, we don't change the voltage on the polarimeter.
    if (VHe >= 0){
        setUSB1208AnalogOut(HETARGET,(int)hpValue/HPCAL);

        i=resetGPIBBridge(GPIBBRIDGE1);
        delay(200);
        i=initSorensen120(SORENSEN120,GPIBBRIDGE1);

        i = setSorensen120Volts(sorensenValue,SORENSEN120,GPIBBRIDGE1);

        retryCounter=0;
        if(i!=0 && retryCounter < 5){
            retryCounter++;
            printf("Error setting Sorensen Code: %d\n",i);
            printf("Trying to set again after .5 s\n");
            delay(500);
            i = setSorensen120Volts(sorensenValue,SORENSEN120,GPIBBRIDGE1);
        }

        // The supply can take some time to get to he desired voltage, pause for 2 seconds to allow for this process.
        delay(2000);
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
	printf("MEASUREMENT\tCOUNT+45\tCURRENT+45\tCOUNT-45\tCURRENT-45\n");// This line withough a comment is vital for being able to quickly process data. DON'T REMOVE

	for (j=0;j<nMeasurements;j++){
		homeMotor(POL_MOTOR); 
		stepMotor(POL_MOTOR,CCLK,VALLEY1); 
		getCountsAndCurrent(dwell,&sumCounts,&current);
		allCountsPlus=allCountsPlus+sumCounts;
		countRatePlus=countRatePlus+(float)sumCounts /(current*pow(10,9-scale));
		printf("%d\t%ld\t%1.2e\t",j,sumCounts,current+leakageCurrent);
		fprintf(rawData,"%d\t%ld\t%1.3e\t",j,sumCounts,current+leakageCurrent);

		homeMotor(POL_MOTOR); 
		stepMotor(POL_MOTOR,CLK,STEPSPERREV-VALLEY4); 
		getCountsAndCurrent(dwell,&sumCounts,&current);
		allCountsMinus=allCountsMinus+sumCounts;
		countRateMinus=countRateMinus+(float)sumCounts /(current*pow(10,9-scale));
		printf("%ld\t%1.2e\n",sumCounts,current+leakageCurrent);
		fprintf(rawData,"%ld\t%1.3e\n",sumCounts,current+leakageCurrent);
		stepMotor(POL_MOTOR,CCLK,STEPSPERREV-VALLEY4+UNDENIABLESHIFT); 
	}
	printf("Total Cts.:\t%ld\t%ld\n",allCountsPlus,allCountsMinus);
	printf("Rate:\t%2.2e\t%2.2e\n",(float)countRatePlus /nMeasurements,countRateMinus/nMeasurements);
	fclose(rawData);

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

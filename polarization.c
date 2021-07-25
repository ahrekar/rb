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
#ifndef DEFINITIONS_H
#define DEFINITIONS_H
	#include "mathTools.h"
#endif
#define STEPSPERREV 1200
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979

#include "interfacing/RS485Devices.h"
#include "interfacing/Sorensen120.h"
#include "interfacing/K617meter.h"
#include "interfacing/K6485meter.h"

#define GPIBBRIDGE1 0XC9 // the gpib bridge can have many gpib devices attached to it, so will also need the GPIB address of each
// this is the GPIB addresses of each respective instrument attached to this bridge
#define SORENSEN120 0x0C

int getPolarizationData(char* fileName, float VHe, int dwell, float leakageCurrent);
void plotCommand(FILE* gnuplot, char* fileName, char* buffer);
void plotData(char* fileName);

int main (int argc, char **argv)
{
	int dwell;
    float VHe; /* (V)oltage of (He)lium Target */
	float leakageCurrent;
    int ammeterScale;
	
	char analysisFileName[80],backgroundFileName[80],rawDataFileName[80],comments[1024]; 
    char configFileName[]="/home/pi/RbControl/system.cfg";
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
	if (argc==6){
		VHe=atof(argv[1]);
		dwell=atoi(argv[2]);
		ammeterScale=atof(argv[3]);
		leakageCurrent=atof(argv[4]);
		strcpy(comments,argv[5]);
		strcpy(backgroundFileName,"NONE");
	} else {
		printf("There is one option for using this program: \n\n");
		printf("usage '~$ sudo ./polarization <voltage for He target> \\  (0-63, -1 to leave unchanged)\n");
        printf("                              <dwell> \\                  (1-5)s\n");
        printf("                              <ammeterScale> \\           (assumed neg.)\n");
        printf("                              <leakageCurrent> \\               \n");
        printf("                              <comments_in_double_quotes>'\n");
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
	strftime(rawDataFileName,80,"/home/pi/RbData/%F/POL%F_%H%M%S.dat",timeinfo); 
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

    // BEGIN record file header information
	fprintf(rawData,"#File\t%s\n",rawDataFileName);
	fprintf(rawData,"#Comments\t%s\n",comments);
	printf("File:\t%s\n",rawDataFileName);
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
	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(rawData,"#T_res:\t%f\n",returnFloat);
	getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(rawData,"#T_res_set:\t%f\n",returnFloat);
	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(rawData,"#T_trg:\t%f\n",returnFloat);
	getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(rawData,"#T_trg_set:\t%f\n",returnFloat);

	fprintf(rawData,"#V_he:\t%f\n",VHe);
	fprintf(rawData,"#LEAKCURR:\t%f\n",leakageCurrent);
	fprintf(rawData,"#SCALE:\t%d\n",ammeterScale);
	fprintf(rawData,"#AOUTCONV:\t%2.6f\n",HPCAL);
	fprintf(rawData,"#REV:\t%d\n",REVOLUTIONS);
	fprintf(rawData,"#DATAPPR:\t%d\n",DATAPOINTSPERREV);
	fprintf(rawData,"#STPPERREV:\t%d\n",STEPSPERREV);
	fprintf(rawData,"#DATPTS:\t%d\n",DATAPOINTS);
	fprintf(rawData,"#DWELL(s):\t%d\n",dwell);
	getCommentLineFromFile(configFileName,"#PumpQWPAngle(step):",buffer);
	fprintf(rawData,"#QWP(STEP):\t%s\n",buffer);

	fclose(rawData);
    // END record file header information

	// Collect raw data
	getPolarizationData(rawDataFileName, VHe, dwell, leakageCurrent); 

	plotData(rawDataFileName);

	//processFileWithBackground(analysisFileName,backgroundFileName,rawDataFileName,DATAPOINTSPERREV,REVOLUTIONS,1);

	closeUSB1208();

	// Remove the file indicating that we are taking data.
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

int getPolarizationData(char* fileName, float VHe, int dwell, float leakageCurrent){
	//char echoData[128];
	// Variables for stepper motor control.
	int nsteps,steps,ninc,i;
    int retryCounter;
    float sorensenValue, hpValue;

	// Variables for data collections.
	long returnCounts;
	long sumCounts;
	float current,angle;
	float currentErr;
	float* measurement = calloc(dwell+1,sizeof(float));


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

	homeMotor(POL_MOTOR); 

	nsteps=STEPSPERREV*REVOLUTIONS;
	ninc=STEPSPERREV/DATAPOINTSPERREV; // The number of steps to take between readings.

	fprintf(rawData,"STEP\tCOUNT\tCURRENT\tCURRENTsd\tANGLE\n");// This line withough a comment is vital for being able to quickly process data. DON'T REMOVE
	printf("Steps\tCounts\tCurrent\n");

	for (steps=0;steps<nsteps;steps+=ninc){
		stepMotor(POL_MOTOR,CCLK,ninc); 

		current=0.0;
		sumCounts=0;
		for(i=0;i<dwell;i++){
            //writeRS485to232Bridge("READ?",echoData,0xCA);
			//current += atof(echoData);
            do{
            // This is the usual line for the polarization
			getUSB1208AnalogIn(0,&measurement[i]);
            
            // This is the ad-hoc line for determining the Beta position, using the Ref photodiode channel.
			//getUSB1208AnalogIn(BROWN_KEITHLEY,&measurement[i]);
            //j = getReadingK6485(&measurement[i], K6485METERVERT, GPIBBRIDGE2);
            }while(measurement[i] == 0);
            current=(measurement[i]/(float)(dwell+1))+current;

			getUSB1208Counter(10,&returnCounts);
			sumCounts += returnCounts;

		}
        do{
            // This is the usual line for the polarization
			getUSB1208AnalogIn(0,&measurement[i]);
            
            // This is the ad-hoc line for determining the Beta position, using the Ref photodiode channel.
			// getUSB1208AnalogIn(BROWN_KEITHLEY,&measurement[i]);
            //j = getReadingK6485(&measurement[i], K6485METERVERT, GPIBBRIDGE2);
        }while(measurement[i] == 0);
        current=(measurement[i]/(float)(dwell+1))+current;


        currentErr=stdDeviation(measurement,dwell+1)/pow(dwell+1,.5);
        //printf("Err: %f\n",currentErr);

		angle = (float)steps/STEPSPERREV*2.0*PI;

		printf("%03d\t%06ld\t%1.2e\n",steps,sumCounts,current+leakageCurrent);
		fprintf(rawData,"%d\t%ld\t%e\t%f\t%f\n",steps,sumCounts,current+leakageCurrent,currentErr,angle);
	}
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
		fprintf(gnuplot, "set terminal dumb size 54,24\n");
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

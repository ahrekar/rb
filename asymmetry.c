/*
 *
 * This program will be used to quickly record the amount of 
 * contamination present in the probe detector from the
 * pump laser. 
 *
 * 
*/

#include "interfacing/interfacing.h"
#include "interfacing/kenBoard.h"
#include "mathTools.h"
#include "interfacing/RS485Devices.h"
#include "interfacing/K6485meter.h"
#include "interfacing/K485meter.h"
#include "interfacing/K617meter.h"
#include "interfacing/K617meter.h"

#define NUMCHANNELS 4

void collectAndRecordData(char* fileName, int cycles, int measurementsPerCycle);
void recordSystemStats(char* fileName);
void recordInputsAndComments(char* fileName, int cycles, int measurementsPerCycle, char* comments);

int main (int argc, char **argv)
{
	time_t rawtime;
	struct tm * timeinfo;
	char fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	int i;
	int cycles, measurementsPerCycle;

	FILE *dataCollectionFlagFile;

	if (argc==4) {
		cycles = atoi(argv[1]);
		measurementsPerCycle = atoi(argv[2]);
		strcpy(comments,argv[3]);
	} else {
		printf("Usage:\n$ sudo ./asymmetry <cycles> <measurementsPerCycle> <comments>\n");
		return 0;
	}
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("Unable to open file: %s\n",dataCollectionFileName);
		exit(1);
	}

	wiringPiSetup();
	initializeBoard();
	initializeUSB1208();
	i = resetGPIBBridge(GPIBBRIDGE1);
	i = resetGPIBBridge(GPIBBRIDGE2);
	if(i != 0) printf("ERROR RESETTING GPIB BRIDGE\n");

	// get file name. Use format "deflectorTransmission"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F",timeinfo);
	if (stat(fileName, &st) == -1){ // Create the directory for the Day's data 
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/asymmetry%F_%H%M%S.dat",timeinfo);

	printf("\n%s\n",fileName);

	recordSystemStats(fileName);

	recordInputsAndComments(fileName, cycles, measurementsPerCycle, comments);
	
	collectAndRecordData(fileName, cycles, measurementsPerCycle);

	printf("---\n%s\n---\n",fileName);

	closeUSB1208();

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

void recordSystemStats(char* fileName){
	FILE* fp;
	float returnFloat;
	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	fprintf(fp,"#Filename:\t%s\n",fileName);

    /** Record System Stats to File **/
    /** Pressure Gauges **/
	getIonGauge(&returnFloat);
	printf("IonGauge %2.2E Torr \n",returnFloat);
	fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

	getConvectron(GP_TOP2,&returnFloat);
	printf("CVGauge(Source Foreline): %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(Source Foreline)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_TOP1,&returnFloat);
	printf("CVGauge(Target Foreline): %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(Target Foreline)(Torr):\t%2.2E\n", returnFloat);

    /** Temperature Controllers **/
	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#T_res:\t%f\n",returnFloat);
	getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#T_res_set:\t%f\n",returnFloat);

	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#T_trg:\t%f\n",returnFloat);
	getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#T_trg_set:\t%f\n",returnFloat);

    /** End System Stats Recording **/
	fclose(fp);
}

void recordInputsAndComments(char* fileName, int cycles, int measurementsPerCycle, char* comments){
	FILE* fp;
	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	fprintf(fp,"#Cycles:\t%d\n", cycles);
	fprintf(fp,"#MeasurementsPerCycle:\t%d\n", measurementsPerCycle);
	fprintf(fp,"#Comments:\t%s\n",comments);

	fprintf(fp,"qwppos\tfd\tstddev\tct\tstddev\tcb\tstddev\the\tstddev\n");
	printf("qwppos\tfd\tstddev\tct\tstddev\tcb\tstddev\the\tstddev\n");
	fclose(fp);
}

void collectAndRecordData(char* fileName, int cycles, int measurementsPerCycle){
	FILE* fp;
	int k=0,i,m,p, cyc;
	int nSamples = 16; // The number of data points to collect
	float involts[NUMCHANNELS];


	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	// Allocate some memory to store measurements for calculating
	// error bars.
	float* measurement = malloc(nSamples*sizeof(float));

	i=initializeK6485(K6485METERVERT,GPIBBRIDGE2);
	if(i != 0) printf("ERROR INITIALIZING K6485 VERT\n");
	i=initializeK6485(K6485METERHORIZ,GPIBBRIDGE2);
	if(i != 0) printf("ERROR INITIALIZING K6485 HORIZ\n");
	i=initializeK617(K617METER,GPIBBRIDGE1);
	if(i != 0) printf("ERROR INITIALIZING K617\n");
	i=initializeK485(K485METER,GPIBBRIDGE1);
	if(i != 0) printf("ERROR INITIALIZING K485\n");

	int numMotorPositions=2;
	int motorPositions[]={160,72};
	

	for(cyc=0; cyc < cycles; cyc++)
	{
		for(p=0; p < numMotorPositions; p++)
		{
			setMotor(PUMP_MOTOR, motorPositions[p]);
			delay(2000);
			for(m=0; m < measurementsPerCycle; m++)
			{
				for(k=0;k<NUMCHANNELS;k++){
					involts[k]=0.0;	
				}

				i = getReadingK617(&involts[0], K617METER, GPIBBRIDGE1);
				i = getReadingK6485(&involts[1], K6485METERVERT, GPIBBRIDGE2);
				i = getReadingK6485(&involts[2], K6485METERHORIZ, GPIBBRIDGE2);
				//i = getReadingK485(&involts[3], K485METER, GPIBBRIDGE1);
				involts[3] = 0;

				fprintf(fp,"%d\t", motorPositions[p]);
				printf("%d\t", motorPositions[p]);
				for(k=0;k<NUMCHANNELS;k++){
					fprintf(fp,"%+0.5e\t%+0.5e", involts[k], 0.);
					printf("%+0.5e   ", involts[k]);
					if(k<NUMCHANNELS-1){
						printf(" | ");
						fprintf(fp,"\t");
					}
				}
				printf("\n");
				fprintf(fp,"\n");
				fflush(stdout);
			}
		}
	}

	fclose(fp);
	free(measurement);
}

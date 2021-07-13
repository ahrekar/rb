/*
 *
 * This program will be used to quickly record the amount of 
 * contamination present in the probe detector from the
 * pump laser. 
 *
 * 
*/

#include "interfacing/interfacing.h"
#include "mathTools.h"
#include "interfacing/RS485Devices.h"
#include "interfacing/K6485meter.h"
#include "interfacing/K485meter.h"
#include "interfacing/K617meter.h"

#define NUMCHANNELS 4

void collectAndRecordData(char* fileName);
void writeFileHeader(char* fileName, char* comments);

int main (int argc, char **argv)
{
	time_t rawtime;
	struct tm * timeinfo;
	char fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
    int i;

	FILE *dataCollectionFlagFile, *fp;

	if (argc==2) {
		strcpy(comments,argv[1]);
	} else {
		printf("Usage:\n$ sudo ./monitorPhotodiodes <comments>\n");
		return 0;
	}
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("Unable to open file: %s\n",dataCollectionFileName);
		exit(1);
	}

	initializeBoard();
	initializeUSB1208();
	i = resetGPIBBridge(GPIBBRIDGE1);
	i = resetGPIBBridge(GPIBBRIDGE2);
	if(i != 0) printf("ERROR RESETTING GPIB BRIDGE\n");

	// get file name. Use format "MonitorPhotodiodes"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F",timeinfo);
	if (stat(fileName, &st) == -1){ // Create the directory for the Day's data 
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/MonitorPhotodiodes%F_%H%M%S.dat",timeinfo);

	printf("\n%s\n",fileName);

	writeFileHeader(fileName, comments);
	fp=fopen(fileName,"a");

	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}
	
	collectAndRecordData(fileName);

	closeUSB1208();

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	printf("\n%s\n",fileName);

	return 0;
}

void writeFileHeader(char* fileName, char* comments){
	FILE* fp;
	float returnFloat;
	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	fprintf(fp,"#Filename:\t%s\n",fileName);
	fprintf(fp,"#Comments:\t%s\n",comments);

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

	//fprintf(fp,"TEMP\tFREQ\tPUMP\tStdDev\tPROBE\tStdDev\tREF\tStdDev\n");
	fprintf(fp,"K617\tStdDev\tPUMP\tStdDev\tPROBE\tStdDev\tREF\tStdDev\n");
	fclose(fp);
}

void collectAndRecordData(char* fileName){
	FILE* fp;
	int k=0,i;
    int timeCounter, evaluationTime=30;
	int nSamples = 16; // The number of data points to collect
    float temperature=0.0;
    float wavelength=0.0;
    int delayTime=1000; // Time in ms
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

    //while(temperature!=-1.0){
    for (timeCounter=0;timeCounter < evaluationTime; timeCounter+=1){
        //scanf("%f,%f",&temperature,&wavelength);
        //fprintf(fp,"%f\t%f\t",temperature,wavelength);

        for(k=0;k<NUMCHANNELS;k++){
            involts[k]=0.0;	
        }
        //if(timeCounter%15==0) printf("       PUMP       |        PROBE      |        REFERENCE\n"); // Channels 1-3
        if(timeCounter%15==0) printf("       Faraday C    |       VERT        |        HORIZ      |        REFERENCE\n"); // Channels 0-3

        /* THE GPIB WAY ************/
		i = getReadingK617(&involts[0], K617METER, GPIBBRIDGE1);
		i = getReadingK6485(&involts[1], K6485METERVERT, GPIBBRIDGE2);
		i = getReadingK6485(&involts[2], K6485METERHORIZ, GPIBBRIDGE2);
		// Channel 3
		i = getReadingK485(&involts[3], K485METER, GPIBBRIDGE1);
		//getUSB1208AnalogIn(BROWN_KEITHLEY,&involts[3]);

		printf("%02d  | ", timeCounter);

        for(k=0;k<NUMCHANNELS;k++){
            fprintf(fp,"%+0.5e\t%+0.5e\t", involts[k], 0.);
            printf("%+0.5e   ", involts[k]);
            if(k<NUMCHANNELS) printf(" | ");
            delay(delayTime/NUMCHANNELS);
        }
        /******* END GPIB Way */

        /* THE ANALOG WAY *********
        // grab several readings and average
        //for(k=1;k<NUMCHANNELS+1;k++){
        for(k=0;k<NUMCHANNELS;k++){
            for (i=0;i<nSamples;i++){
                getUSB1208AnalogIn(k,&measurement[i]);
                involts[k]=involts[k]+measurement[i];
                delay(delayTime/NUMCHANNELS/nSamples);
            }
            involts[k]=fabs(involts[k])/(float)nSamples;
            fprintf(fp,"%0.4f\t%0.4f\t",involts[k],stdDeviation(measurement,nSamples));
            printf("  %0.4f %0.4f  ",involts[k],stdDeviation(measurement,nSamples));
            if(k<NUMCHANNELS) printf(" | ");
        }
        ******** THE ANALOG WAY */

        fprintf(fp,"\n");
        printf("\n");
        
    }

	fprintf(fp,"\n");
	fclose(fp);
	free(measurement);
}

/*
   Program to record polarization.
   RasPi connected to USB 1208LS.

   FARADAY ROTATION


   use Aout 0 to set laser wavelength. see page 98-100
   usage
   $ sudo ./faradayRotation <aoutstart> <aoutstop> <deltaaout> <comments_no_spaces>


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
#include <wiringPi.h>
#include "mathTools.h" //includes stdDeviation
#include "interfacing/interfacing.h"
#include "interfacing/waveMeter.h"

#define PI 3.14159265358979
#define STEPSIZE 7.0
#define STEPSPERREV 350.0
#define WAITTIME 2

#define BUFSIZE 1024

int recordNumberDensity(char* fileName);
void collectLaserProfile(FILE* fp, int* photoDetector, int numPhotoDetectors,int motor, int totalSteps, int stepSize, int dir,int measurementsPerStep);

int main (int argc, char **argv)
{
    int i;
	int totalSteps, stepSize, measurementsPerStep, dir;
    time_t rawtime;
    float returnFloat;
    //float probeOffset,mag1Voltage,mag2Voltage;
    struct tm * timeinfo;
    char fileName[BUFSIZE], comments[BUFSIZE];
	char folderName[BUFSIZE], timeStamp[BUFSIZE], dateStamp[BUFSIZE];
    char dailyFileName[BUFSIZE];
	char dataTypeFilePrefix[BUFSIZE];
    char dataCollectionFileName[] = "/home/pi/.takingData"; 
    FILE *fp,*dataCollectionFlagFile,*configFile;


    if (argc==6){
		// 1. Total number of steps.
		// 2. Step increment per measurement.
		// 3. Stepping direction
		// 4. Number of measurements at each step.
		// 5. Comments
		totalSteps=atoi(argv[1]);
		stepSize=atoi(argv[2]);
		dir=atoi(argv[3]);
		measurementsPerStep=atoi(argv[4]);
        strcpy(comments,argv[5]);
    } else { 
        printf("usage '~$ sudo ./razorBladeLaserProfiling <totalSteps> <stepIncrement> <step direction (0=CCLK, 1=CLK)> <measurementsPerStep> <comments in quotes>'\n");
        printf("Note: 1000 steps = .125 in.\n");
        return 1;
    }

    // Indicate that data is being collected.
    dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
    if (!dataCollectionFlagFile) {
        printf("unable to open file:\t%s\n",dataCollectionFileName);
        exit(1);
    }

    // Set up interfacing devices
    initializeBoard();
    initializeUSB1208();

    // Get file name.  Use format "FDayScan"+$DATE+$TIME+".dat"
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    struct stat st = {0};
    strftime(fileName,BUFSIZE,"/home/pi/RbData/%F",timeinfo); //INCLUDE
    if (stat(fileName, &st) == -1){
        mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
    }
	strcpy(dataTypeFilePrefix,"LaserProfile");
    strftime(folderName,BUFSIZE,"/home/pi/RbData/%F",timeinfo); //INCLUDE
	strftime(timeStamp,BUFSIZE,"%H%M%S",timeinfo);
	strftime(dateStamp,BUFSIZE,"%F",timeinfo);

	sprintf(fileName,"%s/%s%s_%s.dat",folderName,dataTypeFilePrefix,dateStamp,timeStamp);
	sprintf(dailyFileName,"%s/%s%s.dat",folderName,dataTypeFilePrefix,dateStamp);

    printf("%s\n",fileName);
    printf("%s\n",comments);

    fp=fopen(fileName,"w");
    if (!fp) {
        printf("Unable to open file: %s\n",fileName);
        exit(1);
    }

    configFile=fopen("/home/pi/RbControl/system.cfg","r");
    if (!configFile) {
        printf("Unable to open config file\n");
        exit(1);
    }

    fprintf(fp,"#File:\t%s\n#Comments:\t%s\n",fileName,comments);

    getIonGauge(&returnFloat);
    //printf("IonGauge %2.2E Torr \n",returnFloat);
    fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

    getConvectron(GP_N2_CHAN,&returnFloat);
    //printf("CVGauge(N2) %2.2E Torr\n", returnFloat);
    fprintf(fp,"#CVGauge(N2)(Torr):\t%2.2E\n", returnFloat);

    getConvectron(GP_HE_CHAN,&returnFloat);
    //printf("CVGauge(He) %2.2E Torr\n", returnFloat);
    fprintf(fp,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);

    getPVCN7500(CN_RESERVE,&returnFloat);
    fprintf(fp,"#CurrTemp(Res):\t%f\n",returnFloat);
    getSVCN7500(CN_RESERVE,&returnFloat);
    fprintf(fp,"#SetTemp(Res):\t%f\n",returnFloat);

    getPVCN7500(CN_TARGET,&returnFloat);
    fprintf(fp,"#CurrTemp(Targ):\t%f\n",returnFloat);
    getSVCN7500(CN_TARGET,&returnFloat);
    fprintf(fp,"#SetTemp(Targ):\t%f\n",returnFloat);

    char line[1024];
	fgets(line,1024,configFile);
	while(line[0]=='#'){
		fprintf(fp,"%s",line);
		fgets(line,1024,configFile);
	}

	fclose(configFile);

	fprintf(fp,"#NumAouts:\t%d\n",1);

    char* names[]={"PMP","PRB","REF"};
    int photoDetectors[] = {PUMP_LASER,PROBE_LASER,REF_LASER};
    int numPhotoDetectors = 3;
    int motor = POL_MOTOR;

    // Write the header for the data to the file.
    fprintf(fp,"STEP");
    for(i=0;i<numPhotoDetectors;i++){
        fprintf(fp,"\t%s\t%ssd",names[i],names[i]);
    }
    fprintf(fp,"\n");
    fclose(fp);

    fp=fopen(fileName,"a");

    collectLaserProfile(fp, photoDetectors, numPhotoDetectors, motor, totalSteps, stepSize, dir, measurementsPerStep);

    fclose(fp);

    closeUSB1208();

    // Remove the file indicating that we are taking data.
    fclose(dataCollectionFlagFile);
    remove(dataCollectionFileName);

    return 0;
}

void collectLaserProfile(FILE* fp, int* photoDetector, int numPhotoDetectors,int motor, int totalSteps, int stepSize,int dir, int measurementsPerStep)
{
    int count=0;
    int steps,i,j;


    int nSamples=measurementsPerStep;
	float* measurement = malloc(nSamples*sizeof(float));

    
    float* involts = calloc(numPhotoDetectors,sizeof(float));
    float* stdDev = calloc(numPhotoDetectors,sizeof(float));

	for (steps=0;steps < totalSteps;steps+=(int)stepSize){ // steps
		// (STEPSPERREV) in increments of STEPSIZE
		delay(150);	// Time to wait for motor to move and things to settle.
		//get samples and average
		for(j=0;j<numPhotoDetectors;j++){ // numPhotoDet1
			involts[j]=0.0;	
			for (i=0;i<nSamples;i++){ // nSamples
					getUSB1208AnalogIn(photoDetector[j],&measurement[i]);
					involts[j]=involts[j]+fabs(measurement[i]);
					delay(WAITTIME);
			} // nSamples
			involts[j]=involts[j]/(float)nSamples; 
			stdDev[j]=stdDeviation(measurement,nSamples);
		} // numPhotoDet1

		fprintf(fp,"%d\t",steps);
		for(j=0;j<numPhotoDetectors;j++){
			if(j!=numPhotoDetectors-1)
				fprintf(fp,"%f\t%f\t",involts[j],stdDev[j]);
			else
				fprintf(fp,"%f\t%f\n",involts[j],stdDev[j]);
		}

		count++;
		stepMotor(motor,dir,(int)stepSize);
	} // steps
    free(measurement);
    free(stdDev);
    free(involts);
}

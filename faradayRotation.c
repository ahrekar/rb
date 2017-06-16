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
#include <wiringPi.h>
#include "mathTools.h" //includes stdDeviation
#include "tempControl.h"
#include "interfacing/interfacing.h"
#include "faradayScanAnalysisTools.h"
#include "interfacing/waveMeter.h"

#define PI 3.14159265358979
#define NUMSTEPS 350
#define STEPSIZE 2
#define STEPSPERREV 350.0
#define WAITTIME 2

#define BUFSIZE 1024

int plotData(char* fileName);
int recordNumberDensity(char* fileName);

int main (int argc, char **argv)
{
    int i,steps,nsamples;
    int revolutions,dataPointsPerRevolution;
    int homeFlag;
    time_t rawtime;
    float returnFloat, angle;
    float sumSin, sumCos;
    float f3, f4;
    float probeOffset,mag1Voltage,mag2Voltage;
    struct tm * timeinfo;
    char fileName[BUFSIZE], comments[BUFSIZE];
    char dailyFileName[BUFSIZE];
    char dataCollectionFileName[] = "/home/pi/.takingData"; 

    float involts,involts2; 	// The amount of light that is entering into the sensor. 
    FILE *fp,*dataCollectionFlagFile;


    if (argc==5){
        probeOffset=atof(argv[1]);
        mag1Voltage=atof(argv[2]);
        mag2Voltage=atof(argv[3]);
        strcpy(comments,argv[4]);
    } else { 
        printf("usage '~$ sudo ./faradayscan <probeOffset> <mag. 1 volt> <mag. 2 volt> <comments in quotes>'\n");
        return 1;
    }

    // Indicate that data is being collected.
    dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
    if (!dataCollectionFlagFile) {
        printf("unable to open file:\t%s\n",dataCollectionFileName);
        exit(1);
    }

    revolutions=1;
    dataPointsPerRevolution=NUMSTEPS/STEPSIZE;
    nsamples=32;
    float* measurement = malloc(nsamples*sizeof(float));
    float* measurement2 = malloc(nsamples*sizeof(float));

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
    strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/FDayRotation%F_%H%M%S.dat",timeinfo); //INCLUDE
    strftime(dailyFileName,BUFSIZE,"/home/pi/RbData/%F/FDayRotation%F.dat",timeinfo); //INCLUDE

    printf("%s\n",fileName);
    printf("%s\n",comments);

    fp=fopen(fileName,"w");
    if (!fp) {
        printf("Unable to open file: %s\n",fileName);
        exit(1);
    }

    fprintf(fp,"#File:\t%s\n#Comments:\t%s\n",fileName,comments);

    getIonGauge(&returnFloat);
    printf("IonGauge %2.2E Torr \n",returnFloat);
    fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

    getConvectron(GP_N2_CHAN,&returnFloat);
    printf("CVGauge(N2) %2.2E Torr\n", returnFloat);
    fprintf(fp,"#CVGauge(N2)(Torr):\t%2.2E\n", returnFloat);

    getConvectron(GP_HE_CHAN,&returnFloat);
    printf("CVGauge(He) %2.2E Torr\n", returnFloat);
    fprintf(fp,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);

    getPVCN7500(CN_RESERVE,&returnFloat);
    fprintf(fp,"#CurrTemp(Res):\t%f\n",returnFloat);
    getSVCN7500(CN_RESERVE,&returnFloat);
    fprintf(fp,"#SetTemp(Res):\t%f\n",returnFloat);

    getPVCN7500(CN_TARGET,&returnFloat);
    fprintf(fp,"#CurrTemp(Targ):\t%f\n",returnFloat);
    getSVCN7500(CN_TARGET,&returnFloat);
    fprintf(fp,"#SetTemp(Targ):\t%f\n",returnFloat);

    fprintf(fp,"#ProbeOffset:\t%f\n",probeOffset);
    fprintf(fp,"#Mag1Voltage:\t%f\n",mag1Voltage);
    fprintf(fp,"#Mag2Voltage:\t%f\n",mag2Voltage);

    fprintf(fp,"#Revolutions:\t%d\n",revolutions);
    fprintf(fp,"#DataPointsPerRev:\t%d\n",dataPointsPerRevolution);

    // Write the header for the data to the file.
    fprintf(fp,"STEP\tINT\tINTsd\tHadToHomeFlag\n");

    int j=0;
    int count=0;
    float wavelength=-1;
    homeFlag=0;
    sumSin=0;
    sumCos=0;
    count=0;

    if(quickHomeMotor(PROBE_MOTOR)>0)
        homeFlag=1;
    else
        homeFlag=0;

    //wavelength=getWaveMeter();
    wavelength=-1;

    for (j=0;j<revolutions;j++){
        for (steps=0;steps < NUMSTEPS;steps+=STEPSIZE){ // We want to go through a full revolution of the linear polarizer
            // (NUMSTEPS) in increments of STEPSIZE
            delay(150); // watching the o-scope, it looks like it takes ~100ms for the ammeter to settle after a change in LP
            //get samples and average
            involts=0.0;	
            involts2=0.0;	
            for (i=0;i<nsamples;i++){ // Take several samples of the voltage and average them.
                getUSB1208AnalogIn(PUMP_LASER,&measurement[i]);
                getUSB1208AnalogIn(PROBE_LASER,&measurement2[i]);
                involts=involts+measurement[i];
                involts2=involts2+measurement2[i];
                delay(WAITTIME);
            }
            involts=fabs(involts/(float)nsamples); 
            involts2=fabs(involts2/(float)nsamples); 


            fprintf(fp,"%d\t%f\t%d\t%f\t%f\t%d\n",512,wavelength,steps,involts,involts2,homeFlag);
            angle=2.0*PI*(steps)/STEPSPERREV;
            sumSin+=involts*sin(2*angle);
            sumCos+=involts*cos(2*angle);

            count++;
            stepMotor(PROBE_MOTOR,CLK,STEPSIZE);
        }
    }
    f3=sumSin/count;
    f4=sumCos/count;
    angle = 0.5*atan2(f4,f3);
    printf("Angle: %3.2f\n",angle*180/PI);

    fclose(fp);

    printf("Processing Data...\n");
    analyzeData(fileName, dataPointsPerRevolution, revolutions);

    closeUSB1208();

    // Remove the file indicating that we are taking data.
    fclose(dataCollectionFlagFile);
    remove(dataCollectionFileName);

    return 0;
}

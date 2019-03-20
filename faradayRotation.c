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
#include "faradayScanAnalysisTools.h"
#include "interfacing/waveMeter.h"

#define PI 3.14159265358979
#define STEPSIZE 7.0
#define STEPSPERREV 350.0
#define WAITTIME 2

#define BUFSIZE 1024

int recordNumberDensity(char* fileName);
void collectDiscreteFourierData(FILE* fp, int* photoDetector, int numPhotoDetectors,int motor, int revolutions);

int main (int argc, char **argv)
{
    int i;
    int revolutions,dataPointsPerRevolution;
    time_t rawtime;
    float returnFloat;
    //float probeOffset,mag1Voltage,mag2Voltage;
    struct tm * timeinfo;
    char fileName[BUFSIZE], comments[BUFSIZE];
    char dailyFileName[BUFSIZE];
    char dataCollectionFileName[] = "/home/pi/.takingData"; 
    FILE *fp,*dataCollectionFlagFile,*configFile;


    if (argc==2){
        strcpy(comments,argv[1]);
    } else { 
        printf("usage '~$ sudo ./faradayRotation <comments in quotes>'\n");
        printf("    Don't forget to edit the config file!             \n");
        return 1;
    }

    // Indicate that data is being collected.
    dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
    if (!dataCollectionFlagFile) {
        printf("unable to open file:\t%s\n",dataCollectionFileName);
        exit(1);
    }

    //printf("Data collection flag file created!\n");  //DEBUG

    revolutions=1;
    dataPointsPerRevolution=(int)STEPSPERREV/STEPSIZE;

    // Set up interfacing devices
    initializeBoard();
    initializeUSB1208();

    //printf("Initialized Board\n"); //DEBUG

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

    configFile=fopen("/home/pi/RbControl/system.cfg","r");
    if (!configFile) {
        printf("Unable to open config file\n");
        exit(1);
    }

    fprintf(fp,"#Filename:\t%s\n#Comments:\t%s\n",fileName,comments);

    getIonGauge(&returnFloat);
    //printf("IonGauge %2.2E Torr \n",returnFloat);
    fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

    getConvectron(GP_N2_CHAN,&returnFloat);
    //printf("CVGauge(N2) %2.2E Torr\n", returnFloat);
    fprintf(fp,"#CVGauge(N2)(Torr):\t%2.2E\n", returnFloat);

    getConvectron(GP_HE_CHAN,&returnFloat);
    //printf("CVGauge(He) %2.2E Torr\n", returnFloat);
    fprintf(fp,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);

    //getPVCN7500(CN_RESERVE,&returnFloat);
    fprintf(fp,"#T_res:\t%f\n",returnFloat);
    //getSVCN7500(CN_RESERVE,&returnFloat);
    fprintf(fp,"#T_res_set:\t%f\n",returnFloat);

    //getPVCN7500(CN_TARGET,&returnFloat);
    fprintf(fp,"#T_trg:\t%f\n",returnFloat);
    //getSVCN7500(CN_TARGET,&returnFloat);
    fprintf(fp,"#T_trg_set:\t%f\n",returnFloat);

    char line[1024];
	fgets(line,1024,configFile);
	while(line[0]=='#'){
		fprintf(fp,"%s",line);
		fgets(line,1024,configFile);
	}

	fclose(configFile);

    fprintf(fp,"#Revolutions:\t%d\n",revolutions);
    fprintf(fp,"#DataPointsPerRev:\t%d\n",dataPointsPerRevolution);
	fprintf(fp,"#NumVoltages:\t%d\n",1);
    fprintf(fp,"#PumpWavelength:\t%f\n",getPumpFrequency(&returnFloat));
    //fprintf(fp,"#ProbeWavelength:\t%f\n",getProbeFrequency(&returnFloat));

    // UNCOMMENT THE FOLLOWING LINES WHEN COLLECTING STANDARD DATA
    int numPhotoDetectors = 2;
    int photoDetectors[] = {BOTLOCKIN,TOPLOCKIN};
    char* names[]={"HORIZ","VERT"};
    // UNCOMMENT THE FOLLOWING LINES WHEN USING THE FLOATING PD
    //int photoDetectors[] = {PROBE_LASER,PUMP_LASER,REF_LASER};
    //char* names[]={"PRB","PMP","REF"};
    int motor = PROBE_MOTOR;
    //int motor = PUMP_MOTOR;

    // Write the header for the data to the file.
    fprintf(fp,"STEP");
    for(i=0;i<numPhotoDetectors;i++){
        fprintf(fp,"\t%s\t%ssd",names[i],names[i]);
    }
    fprintf(fp,"\n");
    fclose(fp);

    fp=fopen(fileName,"a");

	homeMotor(motor);
    collectDiscreteFourierData(fp, photoDetectors, numPhotoDetectors, motor, revolutions);

    fclose(fp);

    //printf("Processing Data...\n");

    //plotRawData(fileName);

    //analyzeData(fileName, 1, revolutions, dataPointsPerRevolution, FOI);

    closeUSB1208();

    // Remove the file indicating that we are taking data.
    fclose(dataCollectionFlagFile);
    remove(dataCollectionFileName);

    return 0;
}

void collectDiscreteFourierData(FILE* fp, int* photoDetector, int numPhotoDetectors,int motor, int revolutions)
{
    float sumSin=0;
    float sumCos=0;
    int count=0;
    int steps,i,j,k;
    float angle;


    int nSamples=16;
	float* measurement = malloc(nSamples*sizeof(float));

    
    float* involts = calloc(numPhotoDetectors,sizeof(float));
    float* stdDev = calloc(numPhotoDetectors,sizeof(float));

    for (k=0;k<revolutions;k++){ //revolutions
        for (steps=0;steps < STEPSPERREV;steps+=(int)STEPSIZE){ // steps
            // (STEPSPERREV) in increments of STEPSIZE
            delay(300); // watching the o-scope, it looks like it takes ~100ms for the ammeter to settle after a change in LP. 
            // UPDATE: With the Lock-in at a time scale of 100 ms, it takes 500 ms to settle. 
            // UPDATE: With time scale of 30 ms, takes 300 ms to settle.

            //get samples and average
            for(j=0;j<numPhotoDetectors;j++){ // numPhotoDet1
                involts[j]=0.0;	
                for (i=0;i<nSamples;i++){ // nSamples
                        getMCPAnalogIn(photoDetector[j],&measurement[i]);
                        involts[j]=involts[j]+fabs(measurement[i]);
                        delay(WAITTIME);
                } // nSamples
                involts[j]=involts[j]/(float)nSamples; 
                stdDev[j]=stdDeviation(measurement,nSamples);
            } // numPhotoDet1

            fprintf(fp,"%d\t",steps+(int)STEPSPERREV*k);
            for(j=0;j<numPhotoDetectors;j++){
                if(j!=numPhotoDetectors-1)
                    fprintf(fp,"%f\t%f\t",involts[j],stdDev[j]);
                else
                    fprintf(fp,"%f\t%f\n",involts[j],stdDev[j]);
            }
            angle=2.0*PI*(steps)/STEPSPERREV;
            sumSin+=involts[0]*sin(2*angle);
            sumCos+=involts[0]*cos(2*angle);

            count++;
            stepMotor(motor,CLK,(int)STEPSIZE);
        } // steps
    } // revolutions
    free(measurement);
    free(stdDev);
    free(involts);
}

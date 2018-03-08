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
#define STEPSIZE 7
#define STEPSPERREV 350.0
#define WAITTIME 2

#define BUFSIZE 1024

int plotData(char* fileName);
int recordNumberDensity(char* fileName);
void collectDiscreteFourierData(FILE* fp, int* photoDetector, int numPhotoDetectors,int motor, int revolutions);

int main (int argc, char **argv)
{
	int AoutStart1,AoutStop1,deltaAout,Aout;
	int AoutStart2,AoutStop2;
    int revolutions,dataPointsPerRevolution;
	time_t rawtime;
	float returnFloat;
	float wavelength;
	float probeOffset;
	struct tm * timeinfo;
	char fileName[BUFSIZE], comments[BUFSIZE];
	char dailyFileName[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 

	FILE *fp,*dataCollectionFlagFile,*configFile;


	if (argc==2){
		strcpy(comments,argv[1]);
	} else { 
		printf("usage '~$ sudo ./faradayscan <comments in quotes>'\n");
		return 1;
	}

	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file:\t%s\n",dataCollectionFileName);
		exit(1);
	}

    revolutions=1;
    dataPointsPerRevolution=(int)STEPSPERREV/STEPSIZE;

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
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/FDayScan%F_%H%M%S.dat",timeinfo); //INCLUDE
	strftime(dailyFileName,BUFSIZE,"/home/pi/RbData/%F/FDayScan%F.dat",timeinfo); //INCLUDE

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

    int totalAouts=0;
    deltaAout=42;
    probeOffset=44.9;
    if(probeOffset<45){
        AoutStart1=0;
        AoutStop1=1023;
        totalAouts+=(AoutStop1-AoutStart1)/deltaAout+1;
        AoutStart2=625;
        AoutStop2=1000;
        totalAouts+=(AoutStop2-AoutStart2)/deltaAout+1;
    }else{
        AoutStart1=0;
        AoutStop1=125;
        totalAouts+=(AoutStop1-AoutStart1)/deltaAout+1;
        AoutStart2=625;
        AoutStop2=1000;
        totalAouts+=(AoutStop2-AoutStart2)/deltaAout+1;
    }

	fprintf(fp,"#File:\t%s\n#Comments:\t%s\n",fileName,comments);

    /** Record System Stats to File **/
    /** Pressure Gauges **/
	getIonGauge(&returnFloat);
	printf("IonGauge %2.2E Torr \n",returnFloat);
	fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

	getConvectron(GP_N2_CHAN,&returnFloat);
	printf("CVGauge(N2) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(N2)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_HE_CHAN,&returnFloat);
	printf("CVGauge(He) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);

    /** Temperature Controllers **/
	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#CurrTemp(Res):\t%f\n",returnFloat);
	getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#SetTemp(Res):\t%f\n",returnFloat);

	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#CurrTemp(Targ):\t%f\n",returnFloat);
	getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#SetTemp(Targ):\t%f\n",returnFloat);

    /** End System Stats Recording **/

    char line[1024];
	fgets(line,1024,configFile);
	while(line[0]=='#'){
		fprintf(fp,"%s",line);
		fgets(line,1024,configFile);
	}

	fclose(configFile);

	fprintf(fp,"#Revolutions:\t%d\n",revolutions);
	fprintf(fp,"#DataPointsPerRev:\t%d\n",dataPointsPerRevolution);
	fprintf(fp,"#NumAouts:\t%d\n",totalAouts);
	fprintf(fp,"#StepSize:\t%d\n",STEPSIZE);

	// Write the header for the data to the file.
	fprintf(fp,"STEP\tPUMP\tPUMPsd\tPRB\tPRBsd\n");
    fclose(fp);

	printf("Homing motor...\n");
	homeMotor(PROBE_MOTOR);

    int numPd=3;
    int pd[] = {PUMP_LASER,PROBE_LASER,REF_LASER};

	fp=fopen(fileName,"a");
	setUSB1208AnalogOut(PROBEOFFSET,AoutStart1);
    delay(500);
	setUSB1208AnalogOut(PROBEOFFSET,AoutStop2);
    delay(500);

	for(Aout=AoutStart1;Aout<=AoutStop1;Aout+=deltaAout){
		setUSB1208AnalogOut(PROBEOFFSET,Aout);
		delay(1000); 

	    wavelength=getWaveMeter();

        fprintf(fp,"\n\n#AOUT:%d(%f)\n",Aout,wavelength);
        printf("AOUT:%d(%f)\t",Aout,wavelength);

        collectDiscreteFourierData(fp,pd,numPd /*numPhotoDet*/,PROBE_MOTOR,revolutions);
	}//end for Aout

	for(Aout=AoutStart2;Aout<=AoutStop2;Aout+=deltaAout){
		setUSB1208AnalogOut(PROBEOFFSET,Aout);
		delay(1000); 

		wavelength=getWaveMeter();

        fprintf(fp,"\n\n#AOUT:%d(%f)\n",Aout,wavelength);
        printf("AOUT:%d(%f)\t",Aout,wavelength);

        collectDiscreteFourierData(fp,pd,numPd /*numPhotoDet*/,PROBE_MOTOR,revolutions);
	}//end for Aout

	setUSB1208AnalogOut(PROBEOFFSET,512);
	fclose(fp);


	printf("Processing Data...\n");
	analyzeData(fileName, totalAouts, revolutions, dataPointsPerRevolution);

	char* extensionStart;
	extensionStart=strstr(fileName,".dat");
	strcpy(extensionStart,"RotationAnalysis.dat");

	printf("Plotting Data...\n");
	plotData(fileName);
	//printf("Calculating number density...\n");
	//calculateNumberDensity(fileName, 0, 0);
	//printf("Recording number density to file...\n");
	//recordNumberDensity(fileName);

	closeUSB1208();

	// Remove the file indicating that we are taking data.
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

void collectDiscreteFourierData(FILE* fp, int* photoDetector, int numPhotoDetectors,int motor, int revolutions)
{
    int count=0;
    int steps,i,j,k;

    int nSamples=16;
	float* measurement = malloc(nSamples*sizeof(float));

    
    float* involts = calloc(numPhotoDetectors,sizeof(float));
    float* stdDev = calloc(numPhotoDetectors,sizeof(float));

    for (k=0;k<revolutions;k++){ //revolutions
        for (steps=0;steps < NUMSTEPS;steps+=STEPSIZE){ // steps
            // (NUMSTEPS) in increments of STEPSIZE
            delay(150); // watching the o-scope, it looks like it takes ~100ms for the ammeter to settle after a change in LP
            //get samples and average
            for(j=0;j<numPhotoDetectors;j++){ // numPhotoDet1
                involts[j]=0.0;	
                for (i=0;i<nSamples;i++){ // nSamples
                        getUSB1208AnalogIn(photoDetector[j],&measurement[i]);
                        involts[j]=involts[j]+measurement[i];
                        delay(WAITTIME);
                } // nSamples
                involts[j]=involts[j]/(float)nSamples; 
                stdDev[j]=stdDeviation(measurement,nSamples);
            } // numPhotoDet1

            fprintf(fp,"%d\t",steps+NUMSTEPS*k);
            for(j=0;j<numPhotoDetectors;j++){
                if(j!=numPhotoDetectors-1)
                    fprintf(fp,"%f\t%f\t",involts[j],stdDev[j]);
                else
                    fprintf(fp,"%f\t%f\n",involts[j],stdDev[j]);
            }

            count++;
            stepMotor(motor,CLK,STEPSIZE);
        } // steps
    } // revolutions
    free(measurement);
    free(stdDev);
    free(involts);
}

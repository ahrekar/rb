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
#include "pmd.h"
#include "usb-1208LS.h"
#include "mathTools.h"
#include "fileTools.h"
#include "tempControl.h"
#include "stepperMotorControl.h"

#define REVOLUTIONS 2
#define STEPSPERREV 1200
#define DATAPOINTSPERREV 75 //Options: 2,4,6,8,10,12,15,16,20,24,25,30,40,75,120,200,600
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979
#define HPCAL 28.1/960.0
#define POLMOTOR 0	// The integer that represents the polarization stepper motor is zero.
#define CCLOCKWISE 0	// 0 spins the motor counterclockwise
#define CLOCKWISE 1	// 1 spins it clockwise
#define NORMCURR 0 	// Set this to 1 to normalize the intensity with the current
#define DWELL 2		// The number of seconds to pause, letting electronics settle
#define ALPHA 0		// The constant Alpha (location of transmission axis), measured in degrees.
#define DALPHA 0 	// The uncertainty in ALPHA
#define BETA 0		// The constant Beta_0 (beginning position of QWP relative to LP) measured in degrees.
#define DBETA 0		// The uncertainty in BETA
#define DELTA 90	// The constant Delta (wave plate retardance) in degrees.
#define DDELTA 0	// The uncertainty in DELTA
#define DSTEP 0	// The uncertainty in the step size 
#define NUMSTOKES 4
#define COS 0			// Used for my fourier coefficient array. Cos stored first.
#define SIN (DATAPOINTS/2)	// Sin stored second
#define POS 0			// Used for my error array positive values are stored first.
#define NEG (DATAPOINTS)// Then negative values. 

int processFile(char* backgroundFileName, char* comments);
int processFileWithBackground(char* analysisFileName, char* backgroundFileName, char* dataFile, char* comments);
int getPolarizationData(char* fileName, int aout);

int calculateFourierCoefficients(char* fileName, int dataPoints, float* fcReturn, float* fcErrReturn);
float calculateOneSumTerm(int trigFunc, float intensity, float i,int k);
float calculateOneSumTermError(int trigFunc, int posOrNeg, float intensity,float intensityErr, float i, float iErr, int k);

int calculateStokesParameters(float* fourierCoefficients, float* fcErr, float* stokesReturn, float* stokesErrReturn);
float calculateStokes(int i, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4);
float calculateStokesErr(int i, int signOfError, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4, float* fcErrors);

int writeDataSummaryToFile(char* rawDataFileName, char* analysisFileName, char* backgroundFileName, 
							char* comments,
							float* fourierCoefficients, float* fcError, 
							float* stokesParameters, float* spError);

int printOutFC(float* fourierCoefficients, float* fcErr);
int printOutSP(float* sp, float* spError);
int printOutFloatArray(float* array, int n);
int printOutFloatArrayWithError(float* array, float* error, int n);

int main (int argc, char **argv)
{
	int aout;
	int flag;
	
	char analysisFileName[80],backgroundFileName[80],rawDataFileName[80],comments[1024]; //INCLUDE
	char dataCollectionFileName[] = "/home/pi/.takingData"; 


	// Variables for getting time information to identify
	// when we recorded the data
	time_t rawtime;
	struct tm * timeinfo;
	float bias, offset,energy, current;
	char* extension;
	
	// Setup time variables
	time(&rawtime); //INCLUDE
	timeinfo=localtime(&rawtime); //INCLUDE
	struct stat st = {0};

	// Get parameters.
	if (argc==3){
		aout=atoi(argv[1]);
		strcpy(comments,argv[2]);
		*backgroundFileName=NULL;
	} else if(argc==4){
		aout=atoi(argv[1]);
		strcpy(backgroundFileName,argv[1]);
		if(aout==0 && strcmp(backgroundFileName,"0")!=0){//If the second argument is not "0" (is a fileName)
			// It should be a fileName, process the two files. TODO: Put analysis in a separate program.
			strcpy(rawDataFileName,argv[2]);
			extension = strstr(rawDataFileName,".dat");
			strcpy(extension,"analysis.dat");

			strcpy(comments,argv[3]);

			printf("\n%s\n",analysisFileName);
			processFileWithBackground(analysisFileName,backgroundFileName,rawDataFileName,comments);
			return 0;
		}else{
			strcpy(backgroundFileName,argv[2]);
			strcpy(comments,argv[3]);
		}
	}else {
		printf("There are three options for using this program: \n\n");
		printf("usage '~$ sudo ./polarization <aout_for_HeTarget> <comments_in_double_quotes>'\n");
		printf("usage '~$ sudo ./polarization <aout_for_HeTarget> <background file> <comments_in_double_quotes>'\n");
		printf("usage '~$ sudo ./polarization <background file> <data file> <comments_in_double_quotes'\n");
		return 1;
	}

	// Indicate that data is being collected.
	FILE* dataCollectionFlagFile;
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file \n");
		exit(1);
	}

	// RUDAMENTARIY ERROR CHECKING
	if (aout<0) aout=0;
	if (aout>1023) aout=1023;

	// Create file name.  Use format "EX"+$DATE+$TIME+".dat"
	strftime(analysisFileName,80,"/home/pi/RbData/%F",timeinfo); //INCLUDE
	if (stat(analysisFileName, &st) == -1){
		mkdir(analysisFileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(rawDataFileName,80,"/home/pi/RbData/%F/POL%F_%H%M%S.dat",timeinfo); //INCLUDE
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
	fprintf(rawData,"#Cell Target Temp 1:\t%f\n",getTargetTemperature(3));
	fprintf(rawData,"#Cell Target Temp 2:\t%f\n",getTargetTemperature(5));
	fprintf(rawData,"#Cell Temp 1:\t%f\n",getTemperature(3));
	fprintf(rawData,"#Cell Temp 2:\t%f\n",getTemperature(5));
	fprintf(rawData,"#Aout\t%d\n",aout);
	fprintf(rawData,"#Assumed USB1208->HP3617A conversion\t%2.6f\n",HPCAL);
	fprintf(rawData,"#REVOLUTIONS\t%d\n",REVOLUTIONS);
	fprintf(rawData,"#DataPointsPerRev\t%d\n",DATAPOINTSPERREV);
	fprintf(rawData,"#StepsPerRev\t%d\n",STEPSPERREV);
	fprintf(rawData,"#Datapoints\t%d\n",DATAPOINTS);
	fprintf(rawData,"#PMT dwell time (s)\t%d\n",DWELL);

	fclose(rawData);

	// Collect raw data
	getPolarizationData(rawDataFileName, aout); //INCLUDE

	processFileWithBackground(analysisFileName, backgroundFileName, rawDataFileName, comments); //TODO: Move all processesing of files
															//	    to find stokes Parameters to 
															//		a separate funtion.

	// Remove the file indicating that we are taking data.
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

// INCLUDE
int getPolarizationData(char* fileName, int aout){

	FILE* gnuplot;
	char buffer[1024];
	// Variables for interfacing with the USB1208
	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;
	signed short svalue;
	__u8 channel, gain;

	// Variables for stepper motor control.
	int nsteps,steps,ninc,i;

	// Variables for data collections.
	int counts;
	float current;
	float currentErr;
	int nSamples = 8;
	float* measurement = calloc(nSamples,sizeof(float));

	// TODO Can we relegate USB setup into its own function?
	// we don't change Aout at all during data collection, so 
	// we could just set it and then leave it alone, right?
	// 
	// Almost, we still need it to read in the current. I
	// think it would be reasonable to have a function to 
	// set up the usb stuff, set Aout, and then close 
	// all the usb stuff down again, assuming that it would
	// leave Aout undisturbed after closing. TODO: Try this out.
	//
	// Begin USB setup
	ret = hid_init();
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_init failed with return code %d\n", ret);
		exit(1);
	}

	if ((interface = PMD_Find_Interface(&hid, 0, USB1208LS_PID)) < 0) {
		fprintf(stderr, "USB 1208LS not found.\n");
		exit(1);
	} else {
		printf("USB 1208LS Device is found! interface = %d\n", interface);
	}


	// config mask 0x01 means all inputs
	usbDConfigPort_USB1208LS(hid, DIO_PORTB, DIO_DIR_IN);
	usbDConfigPort_USB1208LS(hid, DIO_PORTA, DIO_DIR_OUT);


	//	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);

	// set up for stepmotor
	wiringPiSetup();

	// Setup for AnalogUSB
	gain=BP_10_00V;
	channel = 0;
	// Write Aout for He traget here
	usbAOut_USB1208LS(hid,1,aout);
	// NOTE THAT THIS SETS THE FINAL ELECTRON ENERGY. THIS ALSO DEPENDS ON BIAS AND TARGET OFFSET.  AN EXCIATION FN WILL TELL THE
	// USER WHAT OUT TO USE, OR JUST MANUALLY SET THE TARGET OFFSET FOR THE DESIRED ENERGY

	// Begin File setup
	FILE* rawData=fopen(fileName,"a");
	if (!rawData) {
		printf("Unable to open file: %s\n",fileName);
		exit(1);
	}
	// End File setup

	homeMotor(POLMOTOR);

	nsteps=STEPSPERREV*REVOLUTIONS;
	ninc=STEPSPERREV/DATAPOINTSPERREV; // The number of steps to take between readings.

	fprintf(rawData,"Steps\tCount\tCurrent\tCurrent Error\n");// This line withough a comment is vital for being able to quickly process data. DON'T REMOVE

	for (steps=0;steps<nsteps;steps+=ninc){

		//200 steps per revoluion
		moveMotor(POLMOTOR,CLOCKWISE,ninc);

		counts=0;
		for (i=0;i<DWELL;i++){
			usbInitCounter_USB1208LS(hid);
			delayMicrosecondsHard(1000000);//WiringPi
			counts+=usbReadCounter_USB1208LS(hid);
		}

		current=0.0;
		for (i=0;i<nSamples;i++){
			measurement[i] = volts_LS(gain,usbAIn_USB1208LS(hid,channel,gain));
			current += measurement[i];
		}

		current = current/(float)nSamples;
		currentErr = stdDeviation(measurement,nSamples);

		printf("Steps %d\tCounts %d\tCurrent %f\n",steps,counts,current);
		fprintf(rawData,"%d\t%d\t%f\t%f\n",steps,counts,current,currentErr);
	}


	fclose(rawData);

	// Create rough graphs of data.
	gnuplot = popen("gnuplot","w"); 

	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 158,32\n");
		fprintf(gnuplot, "set output\n");			
		
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Step'\n");			
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			
		sprintf(buffer, "plot '%s' using 1:2\n",fileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:2\n",fileName);
		fprintf(gnuplot, buffer);
	}
	pclose(gnuplot);

	// Reset Aout back to zero
	usbAOut_USB1208LS(hid,1,0);

	//cleanly close USB
	ret = hid_close(hid);
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_close failed with return code %d\n", ret);
		exit(1);
	}

	hid_delete_HIDInterface(&hid);
	ret = hid_cleanup();
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_cleanup failed with return code %d\n", ret);
		exit(1);
	}
	return 0;
}


int calculateFourierCoefficients(char* fileName, int totalDataPoints, float* fcReturn,float* fcErrReturn){	
	// normalizeWithCurrent should be set to 1 if it is desired
	// Begin File setup
	FILE* data = fopen(fileName,"r");
	if (!data) {
		printf("Unable to open file %s\n",fileName);
		exit(1);
	}
	// End File setup
	int k;
	for (k=0; k < totalDataPoints/2; k++){
		fcReturn[COS+k]=0;
		fcReturn[SIN+k]=0;
		fcErrReturn[COS+POS+k]=0;
		fcErrReturn[COS+NEG+k]=0;
		fcErrReturn[SIN+POS+k]=0;
		fcErrReturn[SIN+NEG+k]=0;
	}
	// TODO: implement the FFT version of this. 
	int i;
	char trash[1024]; 	// We need to skip several lines in the file that aren't data
					 	// This is a buffer to accomplish that.
	trash[0]='#'; 	// This is a quickly thrown together hack to avoid having an fgets statement 
				 	// outside of the while loop.
	for (i=0; i< totalDataPoints; i++){
		int steps, counts;
		float current, currentErr;
		int j=0;
		while(trash[0]=='#'){ // Skip over all lines that have a # at the beginning. 
			
			fgets(trash,1024,data);
			j++;
		}
		//printf("Lines skipped=%d\n",j);
		trash[0]='a';
		fscanf(data,"%d\t%d\t%f\t%f\n",&steps,&counts,&current,&currentErr);
		//fscanf(data,"%d,%d,%f\n",&steps,&counts,&current); 	// Because I think I might want to revert 
																// to non-error calculations 
																// quickly at some point
																// I'm going to keep it hanging 
																// around commented out for a while.
		//printf("%d,%d,%f,%f\n",steps,counts,current,currentErr);
		float intensity;
		float intensityErr;
		for (k=0; k < totalDataPoints/2; k++){
			intensity=counts;
			intensityErr=sqrt((float)counts);
			if(NORMCURR==1){
				intensity=intensity/fabs(current);
				intensityErr=counts/fabs(current)*sqrt(pu2(counts,sqrt(counts))+pu2(current,currentErr));
			}

			fcReturn[COS+k] += calculateOneSumTerm(COS,intensity, (float)i, k);
			fcReturn[SIN+k] += calculateOneSumTerm(SIN,intensity, (float)i, k);

			fcErrReturn[COS+POS+k] += pow(calculateOneSumTermError(COS,POS,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[COS+NEG+k] += pow(calculateOneSumTermError(COS,NEG,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[SIN+POS+k] += pow(calculateOneSumTermError(SIN,POS,intensity,intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[SIN+NEG+k] += pow(calculateOneSumTermError(SIN,NEG,intensity,intensityErr,(float)i,DSTEP,k),2);
		}
		//printf("%f\t%f\n",fcErrReturn[COS+POS+0],fcErrReturn[COS+POS+1]);
	}
	for (k=0; k < totalDataPoints/2; k++){
		fcErrReturn[COS+POS+k]=sqrt(fcErrReturn[COS+POS+k]);
		fcErrReturn[COS+NEG+k]=sqrt(fcErrReturn[COS+NEG+k]);
		fcErrReturn[SIN+POS+k]=sqrt(fcErrReturn[SIN+POS+k]);
		fcErrReturn[SIN+NEG+k]=sqrt(fcErrReturn[SIN+NEG+k]);
	}
	//printf("Lines Read: %d\n",i);
	fclose(data);
	return 0;
}

float calculateOneSumTerm(int trigFunc, float intensity, float i,int k){
	int totalDataPoints=DATAPOINTS;
	int d0_L=0;	// d0_L represents two delta functions. See Berry for
				// more info.
	if(k==0 || k==totalDataPoints/2-1){
		d0_L=1;
	}else{
		d0_L=0;
	}
	if (trigFunc==COS)
		return 2 * intensity * cos(k*(2*PI*REVOLUTIONS)*i/totalDataPoints)/(totalDataPoints*(1+d0_L));
	else
		return 2 * intensity * sin(k*(2*PI*REVOLUTIONS)*i/totalDataPoints)/(totalDataPoints*(1+d0_L));
}

float calculateOneSumTermError(int trigFunc, int posOrNeg, float intensity,float intensityErr, float i, float iErr, int k){
	if (posOrNeg==NEG){
		intensityErr=-intensityErr;	
		iErr=-iErr;	
	}
	float sI=calculateOneSumTerm(trigFunc,intensity+intensityErr,i,k)-calculateOneSumTerm(trigFunc,intensity,i,k);
	float sStep=calculateOneSumTerm(trigFunc,intensity,i+iErr,k)-calculateOneSumTerm(trigFunc,intensity,i,k);

	return sqrt(pow(sI,2)+pow(sStep,2));
}

int calculateStokesParameters(float* fourierCoefficients, float* fcErrors, float* stokesReturn, float* stokesErrorReturn){
	float delta=2*PI*(DELTA)/360.0;
	float alpha=2*PI*(ALPHA)/360.0;
	float beta_0=2*PI*(BETA)/360.0;
	float c0=fourierCoefficients[COS+0];
	float c2=fourierCoefficients[COS+2];
	float c4=fourierCoefficients[COS+4];
	float s2=fourierCoefficients[SIN+2];
	float s4=fourierCoefficients[SIN+4];
	int pos=0;
	int neg=NUMSTOKES;
	int i;
	for(i=0;i<NUMSTOKES;i++){
		stokesReturn[i]=calculateStokes(i,alpha,beta_0,delta,c0,c2,c4,s2,s4);
		stokesErrorReturn[pos+i]=calculateStokesErr(i,pos,alpha,beta_0,delta,c0,c2,c4,s2,s4,fcErrors);
		stokesErrorReturn[neg+i]=calculateStokesErr(i,neg,alpha,beta_0,delta,c0,c2,c4,s2,s4,fcErrors);
	}	
	return 0;
}

float calculateStokes(int i, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4){
	if(i==0)
		return c0-(1+cos(delta))/(1-cos(delta))*(c4*cos(4*alpha+4*beta)+s4*sin(4*alpha+4*beta));
	else if(i==1)
		return 2.0/(1-cos(delta))*(c4*cos(2*alpha+4*beta)+s4*sin(2*alpha+4*beta)) / calculateStokes(0,alpha, beta, delta, c0, c2, c4, s2, s4);
	else if(i==2)
		return 2.0/(1-cos(delta))*(s4*cos(2*alpha+4*beta)-c4*sin(2*alpha+4*beta)) / \
			calculateStokes(0,alpha, beta, delta, c0, c2, c4, s2, s4);
	else
		return -s2/(sin(delta)*cos(2*alpha+2*beta)) / \
			calculateStokes(0,alpha, beta, delta, c0, c2, c4, s2, s4);
}

float calculateStokesErr(int i, int signOfError, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4, float* fcErrors){
	int numVars = 8;
	float* temp = calloc(numVars,sizeof(float));
	float totalError=0;
	int sgn=1;
	int sign=POS;

	if(signOfError == 4){
		sign=NEG;
		sgn=-1;
	}

	float k;
	      k=calculateStokes(i, alpha , beta, delta, c0, c2, c4, s2, s4);
	temp[0]=calculateStokes(i,   alpha+sgn*(DALPHA)     , beta, delta, c0, c2, c4, s2, s4)-k;
	temp[1]=calculateStokes(i,alpha, beta  +  sgn*DBETA       , delta, c0, c2, c4, s2, s4)-k;
	temp[2]=calculateStokes(i,alpha, beta,     delta+sgn*DDELTA      , c0, c2, c4, s2, s4)-k;
	temp[3]=calculateStokes(i,alpha, beta, delta, c0+fcErrors[COS+sign+0], c2, c4, s2, s4)-k;
	temp[4]=calculateStokes(i,alpha, beta, delta, c0, c2+fcErrors[COS+sign+2], c4, s2, s4)-k;
	temp[5]=calculateStokes(i,alpha, beta, delta, c0, c2, c4+fcErrors[COS+sign+4], s2, s4)-k;
	temp[6]=calculateStokes(i,alpha, beta, delta, c0, c2, c4, s2+fcErrors[SIN+sign+2], s4)-k;
	temp[7]=calculateStokes(i,alpha, beta, delta, c0, c2, c4, s2, s4+fcErrors[SIN+sign+4])-k;

	int j;
	for(j=0;j<numVars;j++)
		totalError+=pow(temp[j],2);
	
	free(temp);

	return sqrt(totalError);
}

int printOutFC(float* fourierCoefficients, float* fcError){
	printf("Cos Coefficients:\n");
	int numCoefficients = 10;
	int i;
	for(i=0;i<numCoefficients;i++){
		printf("%s %d:\t%10.3f\t%10.3f\t%10.3f\n","Cos",i,fourierCoefficients[COS+i],fcError[COS+POS+i],fcError[COS+NEG+i]);
	}
	printf("Sin Coefficients:\n");
	for(i=0;i<numCoefficients;i++){
		printf("%s %d:\t%10.3f\t%10.3f\t%10.3f\n","Sin",i,fourierCoefficients[SIN+i],fcError[SIN+POS+i],fcError[SIN+NEG+i]);
	}
	return 0;
}

int printOutSP(float* sp, float* spError){
	printf("Stokes Parameters:\n");
	printf("  I:\t%10.3f\t%10.3f\t%10.3f\n",sp[0],spError[0+0],spError[4+0]);
	printf("M/I:\t%10.3f\t%10.3f\t%10.3f\n",sp[1],spError[0+1],spError[4+1]);
	printf("C/I:\t%10.3f\t%10.3f\t%10.3f\n",sp[2],spError[0+2],spError[4+2]);
	printf("S/I:\t%10.3f\t%10.3f\t%10.3f\n",sp[3],spError[0+3],spError[4+3]);
	return 0;
}

int printOutFloatArray(float* array, int n){
	int i;
	for(i=0;i<n;i++){
		printf("Element[%d]:%f\n",i,array[i]);
	}
	return 0;
}

int printOutFloatArrayWithError(float* array, float* errorArray, int n){
	int i;
	for(i=0;i<n;i++){
		printf("Element[%d]:%f\tError:%f\n",i,array[i],errorArray[i]);
	}
	return 0;
}

int processFile(char* backgroundFileName, char* comments){
	return 0;
}

int processFileWithBackground(char* analysisFileName, char* backgroundFileName, char* dataFileName, char* comments){

	float* fourierCoefficients = malloc(DATAPOINTS*sizeof(float));
	float* fcErr = malloc(DATAPOINTS*2*sizeof(float)); 	// We need twice as many datapoints for the 
														// error because I'm doing the upper error and
														// lower error separately. I'll access the upper
														// and lower error in a similarly convoluted way
														// to how I access the fourier coefficients.

	// Find fourier coefficients from raw data.
	calculateFourierCoefficients(dataFileName,DATAPOINTS,fourierCoefficients,fcErr);

	printf("====Raw Data Fourier Coefficients====\n");
	printOutFC(fourierCoefficients,fcErr);
	printf("\n");

	// Calculate fourier coefficients from BG data, if provided, and
	// remove background from data
	if(*backgroundFileName != NULL){
		
		float* fcBg = malloc(DATAPOINTS*sizeof(float));
		float* fcBgErr = malloc(DATAPOINTS*2*sizeof(float));
		calculateFourierCoefficients(backgroundFileName,DATAPOINTS,fcBg,fcBgErr);

		printf("====Background Fourier Coefficients====\n");
		printOutFC(fcBg,fcBgErr);
		printf("\n");

		int k;
		for (k=0; k < DATAPOINTS; k++){
			fourierCoefficients[k]-=fcBg[k];
			fcErr[POS+k]=sqrt(pow(fcErr[POS+k],2)+pow(fcBgErr[POS+k],2));
			fcErr[NEG+k]=sqrt(pow(fcErr[NEG+k],2)+pow(fcBgErr[NEG+k],2));
		}

		printf("====Signal Fourier Coefficients====\n");
		printOutFC(fourierCoefficients,fcErr);
		printf("\n");

		free(fcBg);
		free(fcBgErr);
	}
	// Calculate Stokes Parameters from Fourier Coefficients.
	float* stokesParameters = malloc(NUMSTOKES*sizeof(float));
	float* spErr = malloc(NUMSTOKES*2*sizeof(float));
	calculateStokesParameters(fourierCoefficients,fcErr,stokesParameters,spErr);

	printf("====Stokes Parameters====\n");
	printOutFloatArray(stokesParameters,4);
	printf("\n");

	writeDataSummaryToFile(analysisFileName,backgroundFileName,dataFileName,
							comments,
							fourierCoefficients,fcErr,
							stokesParameters,spErr);

	free(fourierCoefficients);
	free(fcErr);
	free(stokesParameters);
	free(spErr);
	return 0;
}

int writeDataSummaryToFile(char* analysisFileName, char* backgroundFileName, char* dataFileName, 
							char* comments,
							float* fourierCoefficients, float* fcErr, 
							float* stokesParameters, float* spErr){
	FILE* dataSummary;
	// Record the results along with the raw data in a file.
	dataSummary=fopen(analysisFileName,"w");
	if (!dataSummary) {
		printf("Unable to open file: %s\n", analysisFileName);
		exit(1);
	}
	
	fprintf(dataSummary,"#File\t%s\n",analysisFileName);
	fprintf(dataSummary,"#DataFile\t%s\n",dataFileName);
	fprintf(dataSummary,"#BackgroundFile\t%s\n",(*backgroundFileName!=NULL)? backgroundFileName:"NONE");//This is the ternary operator ()?:
	fprintf(dataSummary,"#Comments\t%s\n",comments);
	fprintf(dataSummary,"#ALPHA\t%d\t%d\n",ALPHA,DALPHA);
	fprintf(dataSummary,"#BETA\t%d\t%d\n",BETA,DBETA);
	fprintf(dataSummary,"#DELTA\t%d\t%d\n",DELTA,DDELTA);
	fprintf(dataSummary,"#Current Adj. Intensity\t%s\n",(NORMCURR==1)?"Yes":"No");
	fprintf(dataSummary,"#f0\t%f\t%f\t%f\n",fourierCoefficients[COS+0],fcErr[POS+COS+0],fcErr[NEG+COS+0]);
	fprintf(dataSummary,"#f1\t%f\t%f\t%f\n",fourierCoefficients[COS+4],fcErr[POS+COS+4],fcErr[NEG+COS+4]);
	fprintf(dataSummary,"#f2\t%f\t%f\t%f\n",fourierCoefficients[SIN+4],fcErr[POS+COS+4],fcErr[NEG+COS+4]);
	fprintf(dataSummary,"#f3\t%f\t%f\t%f\n",fourierCoefficients[SIN+2],fcErr[POS+COS+2],fcErr[NEG+COS+2]);
	fprintf(dataSummary,"#f4\t%f\t%f\t%f\n",fourierCoefficients[COS+2],fcErr[POS+COS+2],fcErr[NEG+COS+2]);
	fprintf(dataSummary,"Stokes Parameter\tValue\tUpper Error\tLower Error\n");
	fprintf(dataSummary,"p0\t%f\t%f\t%f\n",stokesParameters[0],spErr[0+0],spErr[4+0]);
	fprintf(dataSummary,"p1\t%f\t%f\t%f\n",stokesParameters[1],spErr[0+1],spErr[4+1]);
	fprintf(dataSummary,"p2\t%f\t%f\t%f\n",stokesParameters[2],spErr[0+2],spErr[4+2]);
	fprintf(dataSummary,"p3\t%f\t%f\t%f\n",stokesParameters[3],spErr[0+3],spErr[4+3]);
	
	fflush(dataSummary);
	fclose(dataSummary);

	return 0;
}

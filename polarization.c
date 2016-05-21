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
#include <sys/types.h>
#include <asm/types.h>
#include <wiringPi.h>
#include "pmd.h"
#include "usb-1208LS.h"
#include "mathTools.h"
#include "fileTools.h"

#define CLK 3
#define DIR 4
#define REVOLUTIONS 2
#define STEPSPERREV 1200
#define DATAPOINTSPERREV 40
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979
#define DWELL 1
#define ALPHA 0		// The constant Alpha (location of transmission axis), measured in degrees.
#define DALPHA 3 	// The uncertainty in ALPHA
#define BETA 0		// The constant Beta_0 (beginning position of QWP relative to LP) measured in degrees.
#define DBETA 3		// The uncertainty in BETA
#define DELTA 90	// The constant Delta (wave plate retardance) in degrees.
#define DDELTA 3	// The uncertainty in DELTA
#define DSTEP 0	// The uncertainty in the step size 
#define NUMSTOKES 4
#define COS 0
#define SIN (DATAPOINTS/2)
#define POS 0
#define NEG (DATAPOINTS)

int processFiles(char* backgroundFile, char* dataFile);
int getPolarizationData(char* fileName, int aout);

int calculateFourierCoefficients(char* fileName, int dataPoints, float* fcReturn, float* fcErrReturn);
float calculateOneSumTerm(int trigFunc, float intensity, float i,int k);
float calculateOneSumTermError(int trigFunc, int posOrNeg, float intensity,float intensityErr, float i, float iErr, int k);

int calculateStokesParameters(float* fourierCoefficients, float* fcErr, float* stokesReturn, float* stokesErrReturn);
float calculateStokes(int i, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4);
float calculateStokesErr(int i, int signOfError, float alpha, float beta, float delta, float c0, float c2, float c4, float s2, float s4, float* fcErrors);

int printOutFC(float* fourierCoefficients, float* fcErr);
int printOutSP(float* sp, float* spError);
int printOutFloatArray(float* array, int n);
int printOutFloatArrayWithError(float* array, float* error, int n);

int main (int argc, char **argv)
{
	int aout;
	int flag;
	char* tmp="/home/pi/RbData/tmp.dat"; //INCLUDE
	
	char fileName[80], comments[80],backgroundFile[80]; //INCLUDE
	float HPcal;
	FILE* dataSummary;

	// Variables for getting time information to identify
	// when we recorded the data
	time_t rawtime;
	struct tm * timeinfo;
	float bias, offset,energy, current;

	// Get parameters.
	if (argc==4){
		aout=atoi(argv[1]);
		flag=atoi(argv[2]); 
		strcpy(comments,argv[3]);
	} else if(argc==5){
		aout=atoi(argv[1]);
		flag=atoi(argv[2]); 
		strcpy(backgroundFile,argv[3]);
		strcpy(comments,argv[4]);
	} else if(argc==3){
		strcpy(backgroundFile,argv[1]);
		strcpy(fileName,argv[2]);
		printf("%d\n",DATAPOINTS);
		processFiles(backgroundFile,fileName);
		return 0;
	}else {
		printf("usage '~$ sudo ./polarization <aout_for_target> <Pump Laser Flag> <comments_in_double_quotes>'\n");
		printf("usage '~$ sudo ./polarization <aout_for_target> <Pump Laser Flag> <background file> <comments_in_double_quotes>'\n");
		printf("usage '~$ sudo ./polarization <dataFile> <background file>'\n");
		return 1;
	}

	// RUDAMENTARIY ERROR CHECKING
	if (aout<0) aout=0;
	if (aout>1023) aout=1023;

	// Create file name.  Use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime); //INCLUDE
	timeinfo=localtime(&rawtime); //INCLUDE
	strftime(fileName,80,"/home/pi/RbData/POL%F_%H%M%S.dat",timeinfo); //INCLUDE
	printf("\n%s\n",fileName);

	// Collect raw data
	getPolarizationData(tmp, aout); //INCLUDE
	
					 	// TODO idea: don't have separate array for sin and 
						// cos, but just a single array twice the size of 
						// my current arrays. Then, cos values will be
						// assigned to the first half of the array, and 
						// sin values will be assigned to the second half of
						// the array. To aid in readability, I will  
						// define variables Cos and Sin somewhere
						// to be 0 and kmax. So to access the Cos coefficients
						// I would write:
						//     fourierCoefficients(Cos+i)
	float* fourierCoefficients = malloc(DATAPOINTS*sizeof(float));
	float* fcErr = malloc(DATAPOINTS*2*sizeof(float)); 	// We need twice as many datapoints for the 
														// error because I'm doing the upper error and
														// lower error separately. I'll access the upper
														// and lower error in a similarly convoluted way
														// to how I access the fourier coefficients.

	// Find fourier coefficients from raw data.
	calculateFourierCoefficients(tmp,DATAPOINTS,fourierCoefficients,fcErr);

	printf("====Raw Data Fourier Coefficients====\n");
	printOutFC(fourierCoefficients,fcErr);
	printf("\n");

	// Calculate fourier coefficients from BG data, if provided, and
	// remove background from data
	if(argc==5){
		
		float* fcBg = malloc(DATAPOINTS*sizeof(float));
		float* fcBgErr = malloc(DATAPOINTS*2*sizeof(float));
		calculateFourierCoefficients(backgroundFile,DATAPOINTS,fcBg,fcBgErr);

		printf("====Background Fourier Coefficients====\n");
		printOutFC(fcBg,fcBgErr);
		printf("\n");

		int k;
		for (k=0; k < DATAPOINTS; k++){
			fourierCoefficients[k]-=fcBg[k];
			fcErr[COS+POS+k]=sqrt(pow(fcErr[COS+POS+k],2)+pow(fcBgErr[COS+POS+k],2));
			fcErr[COS+NEG+k]=sqrt(pow(fcErr[COS+NEG+k],2)+pow(fcBgErr[COS+NEG+k],2));
			fcErr[SIN+POS+k]=sqrt(pow(fcErr[SIN+POS+k],2)+pow(fcBgErr[SIN+POS+k],2));
			fcErr[SIN+NEG+k]=sqrt(pow(fcErr[SIN+NEG+k],2)+pow(fcBgErr[SIN+NEG+k],2));
		}

		printf("====Signal Fourier Coefficients====\n");
		printOutFC(fourierCoefficients,fcErr);
		printf("\n");

		free(fcBg);
		free(fcBgErr);
	}
	
	// Calculate Stokes Parameters from Fourier Coefficients.
	float* stokesParameters = malloc(4*sizeof(float));
	float* spErr = malloc(8*sizeof(float));
	calculateStokesParameters(fourierCoefficients,fcErr,stokesParameters,spErr);

	printf("====Stokes Parameters====\n");
	printOutFloatArray(stokesParameters,4);
	printf("\n");

	// Record the results along with the raw data in a file.
	dataSummary=fopen(fileName,"w");
	if (!dataSummary) {
		printf("Unable to open file: %s\n", fileName);
		exit(1);
	}
	
	HPcal=28.1/960.0;
	fprintf(dataSummary,"#File,%s\n",fileName);
	if(argc==5){fprintf(dataSummary,"#BackgroundFile,%s\n",backgroundFile);}
	fprintf(dataSummary,"#Aout,%d\n",aout);
	fprintf(dataSummary,"#Assumed USB1208->HP3617A conversion,%2.6f\n",HPcal);
	fprintf(dataSummary,"#REVOLUTIONS,%d\n",REVOLUTIONS);
	fprintf(dataSummary,"#DataPointsPerRev,%d\n",DATAPOINTSPERREV);
	fprintf(dataSummary,"#StepsPerRev,%d\n",STEPSPERREV);
	fprintf(dataSummary,"#Datapoints,%d\n",DATAPOINTS);
	fprintf(dataSummary,"#PMT dwell time (s),%d\n",DWELL);
	fprintf(dataSummary,"#Comments,%s\n",comments);
	fprintf(dataSummary,"#ALPHA,%d\n",ALPHA);
	fprintf(dataSummary,"#BETA,%d\n",BETA);
	fprintf(dataSummary,"#DELTA,%d\n",DELTA);
	fprintf(dataSummary,"#f0,%f\n",fourierCoefficients[COS+0]);
	fprintf(dataSummary,"#f1,%f\n",fourierCoefficients[COS+4]);
	fprintf(dataSummary,"#f2,%f\n",fourierCoefficients[SIN+4]);
	fprintf(dataSummary,"#f3,%f\n",fourierCoefficients[SIN+2]);
	fprintf(dataSummary,"#f4,%f\n",fourierCoefficients[COS+2]);
	fprintf(dataSummary,"#p0,%f\n",stokesParameters[0]);
	fprintf(dataSummary,"#p1,%f\n",stokesParameters[1]);
	fprintf(dataSummary,"#p2,%f\n",stokesParameters[2]);
	fprintf(dataSummary,"#p3,%f\n",stokesParameters[3]);

	free(fourierCoefficients);
	free(fcErr);
	
	fclose(dataSummary);

	append(fileName,tmp);

	return 0;
}
// INCLUDE
int getPolarizationData(char* fileName, int aout){
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
	pinMode(CLK,OUTPUT);
	pinMode(DIR,OUTPUT);
	digitalWrite(DIR,1);

	// Setup for AnalogUSB
	gain=BP_10_00V;
	channel = 0;
	// Write Aout for He traget here
	usbAOut_USB1208LS(hid,1,aout);
	// NOTE THAT THIS SETS THE FINAL ELECTRON ENERGY. THIS ALSO DEPENDS ON BIAS AND TARGET OFFSET.  AN EXCIATION FN WILL TELL THE
	// USER WHAT OUT TO USE, OR JUST MANUALLY SET THE TARGET OFFSET FOR THE DESIRED ENERGY

	// Begin File setup
	FILE* rawData=fopen(fileName,"w");
	if (!rawData) {
		printf("Unable to open file: %s\n",fileName);
		exit(1);
	}
	// End File setup


	// Give a small delay so that we can be sure
	// the stepperMotor has settled into its state.
	delayMicrosecondsHard(2000); 

	nsteps=STEPSPERREV*REVOLUTIONS;
	ninc=STEPSPERREV/DATAPOINTSPERREV; // The number of steps to take between readings.

	fprintf(rawData,"#Steps,Count,Current,Current Error\n");
	fprintf(rawData,"\n"); // This extra newline is vital for being able to quickly process data. DON'T REMOVE

	for (steps=0;steps<nsteps;steps+=ninc){

		//200 steps per revoluion

		for (i=0;i<ninc;i++){
			// increment steppermotor by ninc steps
			digitalWrite(CLK,HIGH);
			delayMicrosecondsHard(2300); 
			digitalWrite(CLK,LOW);
			delayMicrosecondsHard(2300);
		}

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

		printf("Steps %d,Counts %d,Current %f\n",steps,counts,current);
		fprintf(rawData,"%d,%d,%f,%f\n",steps,counts,current,currentErr);
	}


	fclose(rawData);
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
	char trash[100]; 	// We need to skip several lines in the file that aren't data
					 	// This is a buffer to accomplish that.
	trash[0]='#'; 	// This is a quickly thrown together hack to avoid having an fgets statement 
				 	// outside of the while loop.
	for (i=0; i< totalDataPoints; i++){
		int steps, counts;
		float current, currentErr;
		int j=0;
		while(trash[0]=='#'){ // Skip over all lines that have a # at the beginning. 
			
			fgets(trash,100,data);
			j++;
		}
		trash[0]='a';
		fscanf(data,"%d,%d,%f,%f\n",&steps,&counts,&current,&currentErr);
		//fscanf(data,"%d,%d,%f\n",&steps,&counts,&current); 	// Because I think I might want to revert 
																// to non-error calculations 
																// quickly at some point
																// I'm going to keep it hanging 
																// around commented out for a while.
		//printf("%d,%d,%f,%f\n",steps,counts,current,currentErr);
		float intensityErr;
		for (k=0; k < totalDataPoints/2; k++){
			intensityErr=counts/fabs(current)*sqrt(pu2(counts,sqrt(counts))+pu2(current,currentErr));

			fcReturn[COS+k] += calculateOneSumTerm(COS,counts/fabs(current), (float)i, k);
			fcReturn[SIN+k] += calculateOneSumTerm(SIN,counts/fabs(current), (float)i, k);

			fcErrReturn[COS+POS+k] += pow(calculateOneSumTermError(COS,POS,counts/fabs(current),intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[COS+NEG+k] += pow(calculateOneSumTermError(COS,NEG,counts/fabs(current),intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[SIN+POS+k] += pow(calculateOneSumTermError(SIN,POS,counts/fabs(current),intensityErr,(float)i,DSTEP,k),2);
			fcErrReturn[SIN+NEG+k] += pow(calculateOneSumTermError(SIN,NEG,counts/fabs(current),intensityErr,(float)i,DSTEP,k),2);
		}
		//printf("%f\t%f\n",fcErrReturn[COS+POS+0],fcErrReturn[COS+POS+1]);
		//printf("Lines Read: %d\n",i);
	}
	for (k=0; k < totalDataPoints/2; k++){
		fcErrReturn[COS+POS+k]=sqrt(fcErrReturn[COS+POS+k]);
		fcErrReturn[COS+NEG+k]=sqrt(fcErrReturn[COS+NEG+k]);
		fcErrReturn[SIN+POS+k]=sqrt(fcErrReturn[SIN+POS+k]);
		fcErrReturn[SIN+NEG+k]=sqrt(fcErrReturn[SIN+NEG+k]);
	}
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
		return 2 * intensity * cos(k*2*PI*i/totalDataPoints)/(totalDataPoints*(1+d0_L));
	else
		return 2 * intensity * sin(k*2*PI*i/totalDataPoints)/(totalDataPoints*(1+d0_L));
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
	int neg=4;
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
	temp[1]=calculateStokes(i,alpha,    beta+sgn*(DBETA)      , delta, c0, c2, c4, s2, s4)-k;
	temp[2]=calculateStokes(i,alpha, beta,     delta+sgn*DDELTA      , c0, c2, c4, s2, s4)-k;
	temp[3]=calculateStokes(i,alpha, beta, delta, c0+fcErrors[COS+sign+0], c2, c4, s2, s4)-k;
	temp[4]=calculateStokes(i,alpha, beta, delta, c0, c2+fcErrors[COS+sign+2], c4, s2, s4)-k;
	temp[5]=calculateStokes(i,alpha, beta, delta, c0, c2, c4+fcErrors[COS+sign+4], s2, s4)-k;
	temp[6]=calculateStokes(i,alpha, beta, delta, c0, c2, c4, s2+fcErrors[SIN+sign+2], s4)-k;
	temp[7]=calculateStokes(i,alpha, beta, delta, c0, c2, c4, s2, s4+fcErrors[SIN+sign+4])-k;

	int j;
	for(j=0;j<numVars;j++)
		totalError+=pow(temp[j],2);

	return sqrt(totalError);
}

int printOutFC(float* fourierCoefficients, float* fcError){
	printf("Cos Coefficients:\n");
	printf("Cos 0: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[COS+0],fcError[COS+POS+0],fcError[COS+NEG+0]);
	printf("Cos 1: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[COS+1],fcError[COS+POS+1],fcError[COS+NEG+1]);
	printf("Cos 2: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[COS+2],fcError[COS+POS+2],fcError[COS+NEG+2]);
	printf("Cos 3: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[COS+3],fcError[COS+POS+3],fcError[COS+NEG+3]);
	printf("Cos 4: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[COS+4],fcError[COS+POS+4],fcError[COS+NEG+4]);
	printf("Sin Coefficients:\n");
	printf("Sin 1: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[SIN+1],fcError[SIN+POS+1],fcError[SIN+NEG+1]);
	printf("Sin 2: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[SIN+2],fcError[SIN+POS+2],fcError[SIN+NEG+2]);
	printf("Sin 3: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[SIN+3],fcError[SIN+POS+3],fcError[SIN+NEG+3]);
	printf("Sin 4: %010.3f +%0.4f -%0.4f\n",fourierCoefficients[SIN+4],fcError[SIN+POS+4],fcError[SIN+NEG+4]);
	return 0;
}

int printOutSP(float* sp, float* spError){
	printf("Stokes Parameters:\n");
	printf("  I: %010.3f +%0.4f -%0.4f\n",sp[0],spError[0+0],spError[4+0]);
	printf("M/I: %010.3f +%0.4f -%0.4f\n",sp[1],spError[0+1],spError[4+1]);
	printf("C/I: %010.3f +%0.4f -%0.4f\n",sp[2],spError[0+2],spError[4+2]);
	printf("S/I: %010.3f +%0.4f -%0.4f\n",sp[3],spError[0+3],spError[4+3]);
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

int processFiles(char* backgroundFile, char* dataFile){
	float* fourierCoefficients = malloc(DATAPOINTS*sizeof(float));
	float* fcErr = malloc(DATAPOINTS*2*sizeof(float));

	int _switch = 0;

	calculateFourierCoefficients(dataFile,DATAPOINTS,fourierCoefficients,fcErr);
	if (_switch == 1){
		printf("====Data Fourier Coefficients====\n");
		printOutFC(fourierCoefficients,fcErr);
		printf("\n");
	}

	float* fcBg = malloc(DATAPOINTS*sizeof(float));
	float* fcBgErr = malloc(DATAPOINTS*2*sizeof(float));
	calculateFourierCoefficients(backgroundFile,DATAPOINTS,fcBg,fcBgErr);
	if (_switch == 1){
		printf("====Background Fourier Coefficients====\n");
		printOutFC(fcBg,fcErr);
		printf("\n");
	}

	float* stokesParameters = malloc(4*sizeof(float));
	float* spErr = malloc(8*sizeof(float));

	calculateStokesParameters(fcBg,fcBgErr,stokesParameters, spErr);

	printf("====Background Stokes Parameters====\n");
	printOutSP(stokesParameters,spErr);
	printf("\n");

	int k;
	for (k=0; k < DATAPOINTS; k++){
		fourierCoefficients[k]-=fcBg[k];
		fcErr[COS+POS+k]=sqrt(pow(fcErr[COS+POS+k],2)+pow(fcBgErr[COS+POS+k],2));
		fcErr[COS+NEG+k]=sqrt(pow(fcErr[COS+NEG+k],2)+pow(fcBgErr[COS+NEG+k],2));
		fcErr[SIN+POS+k]=sqrt(pow(fcErr[SIN+POS+k],2)+pow(fcBgErr[SIN+POS+k],2));
		fcErr[SIN+NEG+k]=sqrt(pow(fcErr[SIN+NEG+k],2)+pow(fcBgErr[SIN+NEG+k],2));
	}

	if (_switch==1){
		printf("====Signal Fourier Coefficients====\n");
		printOutFC(fourierCoefficients,fcErr);
		printf("\n");
	}

	// Calculate Stokes Parameters from Fourier Coefficients.
	calculateStokesParameters(fourierCoefficients,fcErr,stokesParameters,spErr);

	printf("====Stokes Parameters====\n");
	printOutSP(stokesParameters,spErr);
	printf("\n");
	
	free(fourierCoefficients);
	free(fcErr);
	free(fcBg);
	free(fcBgErr);
	free(stokesParameters);
	return 0;
}

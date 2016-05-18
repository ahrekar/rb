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
#include "fileTools.c"

#define CLK 3
#define DIR 4
#define STEPSPERREV 1200
#define DATAPOINTS 150
#define PI 3.14159265358979
#define REVOLUTIONS 2
#define DWELL 1
#define ALPHA 0
#define BETA 0
#define DELTA 90

FILE* getPolarizationData(char* fileName, int aout);
int calculateFourierCoefficients(FILE* data, int dataPoints, float* fcCReturn, float* fcSReturn);
int calculateStokesParameters(float* fourierCoefficientsCos,float* fourierCoefficientsSin, float* stokesReturn);
int printOutFC(float* fourierCoefficientsCos, float* fourierCoefficientsSin, int kmax);
int printOutFloatArray(float* array, int n);

int main (int argc, char **argv)
{
	int aout,kmax;
	int flag;
	//char* tmp="/home/pi/RbData/tmp.dat"; //INCLUDE
	
	// For development on home laptop
	char* fileName="/home/karl/Dropbox/00School/gradYear02Summer/polarizationData/dataLessBack.csv"; // REMOVE
	char* tmp="/home/karl/Dropbox/00School/gradYear02Summer/polarizationData/data.csv"; // REMOVE

	//char fileName[80], comments[80],backgroundFile[80]; //INCLUDE
	char comments[80],backgroundFile[80]; // REMOVE
	float HPcal;
	FILE* data;
	FILE* dataSummary;

	// Variables for getting time information to identify
	// when we recorded the data
	time_t rawtime;
	struct tm * timeinfo;
	signed short svalue;
	char buffer[80],comments[80];
	float bias, offset, HPcal,energy, current;
	FILE *fp;
	__s16 sdata[1024];
	__u16 value;
	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0, channel, gain;

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
	}
	else {
		printf("usage '~$ sudo ./polarization <aout_for_target> <Pump Laser Flag> <(optional) background file> <comments_in_double_quotes>'\n");
		return 1;
	}

	// RUDAMENTARIY ERROR CHECKING
	if (aout<0) aout=0;
	if (aout>1023) aout=1023;


	// Create file name.  Use format "EX"+$DATE+$TIME+".dat"
	//time(&rawtime); //INCLUDE
	//timeinfo=localtime(&rawtime); //INCLUDE
	//strftime(fileName,80,"/home/pi/RbData/POL%F_%H%M%S.dat",timeinfo); //INCLUDE
	printf("\n%s\n",fileName);

	// Collect raw data
	//data = getPolarizationData(tmp, aout); //INCLUDE
	data = fopen(tmp,"r"); // REMOVE
	if (!data) {	//REMOVE
		printf("Unable to open file %s\n",fileName);// REMOVE
		exit(1); //REMOVE
	} //REMOVE
	
	kmax=DATAPOINTS/2;
	float* fourierCoefficientsSin = malloc(kmax*sizeof(float));
	float* fourierCoefficientsCos = malloc(kmax*sizeof(float));

	// Find fourier coefficients from raw data.
	calculateFourierCoefficients(data,DATAPOINTS,fourierCoefficientsCos,fourierCoefficientsSin);
	printOutFC(fourierCoefficientsCos,fourierCoefficientsSin,kmax);

	// Calculate fourier coefficients from BG data, if provided
	if(argc==5){
		// Begin File setup
		FILE* background = fopen(backgroundFile,"r");
		if (!background) {
			printf("Unable to open file %s\n",backgroundFile);
			exit(1);
		}
		// End File setup
		
		float* fcSinBg = malloc(kmax*sizeof(float));
		float* fcCosBg = malloc(kmax*sizeof(float));
		calculateFourierCoefficients(background,DATAPOINTS,fcCosBg,fcSinBg);

		printOutFC(fcCosBg,fcSinBg,kmax);

		int k;
		for (k=0; k < kmax; k++){
			fourierCoefficientsCos[k]=fourierCoefficientsCos[k]-fcCosBg[k];
			fourierCoefficientsSin[k]=fourierCoefficientsSin[k]-fcSinBg[k];
		}
	}
	
	// Calculate Stokes Parameters from Fourier Coefficients.
	float* stokesParameters = malloc(4*sizeof(float));
	calculateStokesParameters(fourierCoefficientsCos,fourierCoefficientsSin,stokesParameters);

	printf("Stokes Parameters:\n");
	printOutFloatArray(stokesParameters,4);

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
	fprintf(dataSummary,"#DataPoints,%d\n",DATAPOINTS);
	fprintf(dataSummary,"#PMT dwell time (s),%d\n",DWELL);
	fprintf(dataSummary,"#REVOLUTIONS,%d\n",REVOLUTIONS);
	fprintf(dataSummary,"#Comments,%s\n",comments);
	fprintf(dataSummary,"#ALPHA,%d\n",ALPHA);
	fprintf(dataSummary,"#BETA,%d\n",BETA);
	fprintf(dataSummary,"#DELTA,%d\n",DELTA);
	fprintf(dataSummary,"#f0,%f\n",fourierCoefficientsCos[0]);
	fprintf(dataSummary,"#f1,%f\n",fourierCoefficientsCos[4]);
	fprintf(dataSummary,"#f2,%f\n",fourierCoefficientsSin[4]);
	fprintf(dataSummary,"#f3,%f\n",fourierCoefficientsSin[2]);
	fprintf(dataSummary,"#f4,%f\n",fourierCoefficientsCos[2]);
	fprintf(dataSummary,"#p1,%f\n",stokesParameters[0]);
	fprintf(dataSummary,"#p2,%f\n",stokesParameters[1]);
	fprintf(dataSummary,"#p3,%f\n",stokesParameters[2]);
	fprintf(dataSummary,"#p4,%f\n",stokesParameters[3]);

	fclose(dataSummary);
	fclose(data);

	append(fileName,tmp);

	return 0;
}
/* // INCLUDE
FILE* getPolarizationData(char* fileName, int aout){
	// File variables.
	FILE* rawData;

	// Variables for interfacing with the USB1208
	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;
	signed short svalue;
	__u8 channel, gain;
	
	// Variables for stepper motor control.
	int nsteps,steps,ninc,i;

	// Variables for data collections.
	int counts,numMeasurements;
	float current;

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
		return -1;
	}

	if ((interface = PMD_Find_Interface(&hid, 0, USB1208LS_PID)) < 0) {
		fprintf(stderr, "USB 1208LS not found.\n");
		exit(1);
	} else {
		printf("USB 208LS Device is found! interface = %d\n", interface);
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

	// Begin File setup
	rawData=fopen(fileName,"w");
	if (!rawData) {
		printf("unable to open file \n");
		exit(1);
	}
	// End File setup


	// Give a small delay so that we can be sure
	// the stepperMotor has settled into its state.
	delayMicrosecondsHard(2000); 

	nsteps=STEPSPERREV*REVOLUTIONS;
	ninc=STEPSPERREV/DATAPOINTS; // The number of steps to take between readings.

	fprintf(rawData,"#steps,count,current\n");
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

		printf("steps %d,",(steps));
		fprintf(rawData,"%d,",(steps));

		counts=0;
		for (i=0;i<DWELL;i++){
			usbInitCounter_USB1208LS(hid);
			delayMicrosecondsHard(1000000);//WiringPi
			counts+=usbReadCounter_USB1208LS(hid);
		}

		current=0.0;
		for (i=0;i<8;i++){

		svalue = usbAIn_USB1208LS(hid,channel,gain);
		current = current+volts_LS(gain,svalue);
		}
		current = current/8.0;

		printf("Counts %d,",counts);
		fflush(stdout);
		fprintf(rawData,"%d,",counts);

		printf("current %f\n",current);
		fflush(stdout);
		fprintf(fp,"%f \n",current);

	}


	fclose(fp);
	//cleanly close USB
	ret = hid_close(hid);
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_close failed with return code %d\n", ret);
		return 1;
	}

	hid_delete_HIDInterface(&hid);
	ret = hid_cleanup();
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_cleanup failed with return code %d\n", ret);
		return 1;
	}
	return 0;
}
**/

int calculateFourierCoefficients(FILE* data, int dataPoints, float* fourierCoefficientsCosReturn,float* fourierCoefficientsSinReturn){	
	// TODO: implement the FFT version of this. 
	int i;
	// Zero the malloced data. 
	int k;
	for (k=0; k< dataPoints/2; k++){
		fourierCoefficientsCosReturn[k]=0;
		fourierCoefficientsSinReturn[k]=0;
	}
	char trash[100]; 	// We need to skip several lines in the file that aren't data
					 	// This is a buffer to accomplish that.
	trash[0]='#'; 	// This is a quickly thrown together hack to avoid having an fgets statement 
				 	// outside of the while loop.
	for (i=0; i< dataPoints; i++){
		int steps, counts;
		float current;
		int j=0;
		while(trash[0]=='#'){ // Skip over all lines that have a # at the beginning. 
			fgets(trash,100,data);
			j++;
		}
		trash[0]='a';
		fscanf(data,"%d,%d,%f\n",&steps,&counts,&current);
		printf("%d,%d,%f\n",steps,counts,current);
		int d0_L=0;	// d0_L represents two delta functions. See Berry for
					// more info.
		for (k=0; k < dataPoints/2; k++){
			if(k==0 || k==dataPoints/2-1){
				d0_L=1;
			}else{
				d0_L=0;
			}
			fourierCoefficientsCosReturn[k]+= 2 * counts * cos(k*2*PI*i/dataPoints)/(dataPoints*(1+d0_L));
			fourierCoefficientsSinReturn[k]+= 2 * counts * sin(k*2*PI*i/dataPoints)/(dataPoints*(1+d0_L));
		}
		printf("Lines Read: %d\n",i);
	}
	return 0;
}

int calculateStokesParameters(float* fourierCoefficientsCos,float* fourierCoefficientsSin, float* stokesReturn){
	float delta=2*PI*(DELTA)/360.0;
	float alpha=2*PI*(ALPHA)/360.0;
	float beta_0=2*PI*(BETA)/360.0;
	float c0=fourierCoefficientsCos[0];
	float c2=fourierCoefficientsCos[2];
	float c4=fourierCoefficientsCos[4];
	float s2=fourierCoefficientsSin[2];
	float s4=fourierCoefficientsSin[4];
	stokesReturn[0]=c0-(1+cos(delta))/(1-cos(delta))*(c4*cos(4*alpha+4*beta_0)+s4*sin(4*alpha+4*beta_0));
	stokesReturn[1]=2.0/(1-cos(delta))*(c4*cos(2*alpha+4*beta_0)+s4*sin(2*alpha+4*beta_0))/stokesReturn[0];
	stokesReturn[2]=2.0/(1-cos(delta))*(s4*cos(2*alpha+4*beta_0)-c4*sin(2*alpha+4*beta_0))/stokesReturn[0];
	stokesReturn[3]=sqrt(pow(c2,2)+pow(s2,2))/pow(sin(delta),2)/stokesReturn[0];
	return 0;
}

int printOutFC(float* fourierCoefficientsCos, float* fourierCoefficientsSin, int kmax){
	printf("Cos Coefficients:\n");
	printOutFloatArray(fourierCoefficientsCos,5);
	printf("Sin Coefficients:\n");
	printOutFloatArray(fourierCoefficientsSin,5);
	return 0;
}


int printOutFloatArray(float* array, int n){
	int i;
	for(i=0;i<n;i++){
		printf("Element[%d]:%f\n",i,array[i]);
	}
	return 0;
}

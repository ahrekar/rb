/*
   Program to record polarizqtion.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
 */

#include <stdlib.h>
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

#define CLK 3
#define DIR 4
#define STEPSPERREV 1200
#define DATAPOINTS 16

FILE* getPolarizationData(char* fileName, int aout);
float** calculateFourierCoefficients(FILE* data, int dataPoints);
float** calculateStokesParameters(float** fourierCoefficients, int dataPoints);

int main (int argc, char **argv)
{
	int aout,flag;
	char fileName[80], comments[80];
	float HPcal;
	FILE* data;
	//__s16 sdata[1024];
	//__u16 value;
	//__u16 count;
	//__u8 gains[8];
	//__u8 options;
	//__u8 input, pin = 0, channel, gain;

	// Variables for getting time information to identify
	// when we recorded the data
	time_t rawtime;
	struct tm * timeinfo;

	// Get parameters.
	if (argc==4){
		aout=atoi(argv[1]);
		flag=atoi(argv[2]);
		strcpy(comments,argv[3]);
	} else {
		printf("usage '~$ sudo ./polarization <aout_for_target> <Pump Laser Flag> <comments_in_double_quotes>'\n");
		return 1;
	}

	// RUDAMENTARIY ERROR CHECKING
	if (aout<0) aout=0;
	if (aout>1023) aout=1023;


	// Create file name.  Use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(fileName,80,"/home/pi/RbData/POL%F_%H%M%S.dat",timeinfo);

	printf("\n"); //TODO Consolidation
	printf(fileName);
	printf("\n");


	// Collect raw data
	data = getPolarizationData(fileName, aout);


	// These statements are useful pieces of information
	// that should be included in the analysis file, but
	// I will not record them in the rawData. Until I figure
	// out exactly how that analysis file will work,
	// These statements will live, commented out, here.
	/**
	fprintf(rawData,fileName);
	fprintf(rawData,"\n"); //TODO Consolidate this into one line
	// Set up for stepmotor


	HPcal=28.1/960.0;
	fprintf(rawData,"nsteps %d\n",nsteps);
	fprintf(rawData,"Aout %d\n",aout); //TODO Consolidation
	fprintf(rawData,comments);
	fprintf(rawData,"\nAssumed USB1208->HP3617A converstion %2.6f\n",HPcal);
	**/

	return 0;
}

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
	int dwell,revolutions,nsteps,steps,ninc,i;

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
	// leave Aout undisturbed after closing. TODO: try this out.
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

	//	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0); // I'm not sure what this is doing here, KEN?
	
	// End USB setup

	// Begin USB configuration
	// Write aout for He target here
	usbAOut_USB1208LS(hid,1,aout);

	// NOTE THAT THIS SETS THE FINAL ELECTRON ENERGY. THIS ALSO DEPENDS ON BIAS AND TARGET OFFSET.  AN EXCITATION FN WILL TELL THE
	// USER WHAT OUT TO USE, OR JUST MANUALLY SET THE TARGET OFFSET FOR THE DESIRED ENERGY


	gain=BP_10_00V;
	channel = 0; // analog input for k617 ammeter
	//usbDOut_USB1208LS(hid, DIO_PORTA, flag);

	// End USB configuration

	// Begin Stepmotor setup
	wiringPiSetup();
	pinMode(CLK,OUTPUT);
	pinMode(DIR,OUTPUT);
	digitalWrite(DIR,1);
	digitalWrite(CLK,LOW);
	// End Stepmotor setup

	rawData=fopen(fileName,"w");
	if (!rawData) {
		printf("unable to open file \n");
		exit(1);
	}

	// Give a small delay so that we can be sure
	// the stepperMotor has settled into its state.
	delayMicrosecondsHard(2000);

	dwell=3;
	revolutions=1;
	nsteps=STEPSPERREV*revolutions;
	ninc=STEPSPERREV/DATAPOINTS; // The number of steps to take between readings.

	for (steps=0;steps<nsteps;steps+=ninc){
		for (i=0;i<ninc;i++){
			// increment steppermotor by ninc steps
			digitalWrite(CLK,HIGH);
			delayMicrosecondsHard(2300);
			digitalWrite(CLK,LOW);
			delayMicrosecondsHard(2300);
		}

		printf("steps %d\t",(steps));
		fprintf(rawData,"%d\t",(steps));

		counts=0;
		for (i=0;i<dwell;i++){
			usbInitCounter_USB1208LS(hid);
			delayMicrosecondsHard(1000000); // wiringPi
			counts+=usbReadCounter_USB1208LS(hid);
		}

		current=0.0;
		numMeasurements=8;
		for (i=0;i<numMeasurements;i++){
			svalue = usbAIn_USB1208LS(hid,channel,gain);
			current = current+volts_LS(gain,svalue);
		}
		current = current/numMeasurements;

		printf("Counts %d\t",counts);
		fflush(stdout);
		fprintf(rawData,"%d \t",counts);

		printf("current %f\n",current);
		fflush(stdout);
		fprintf(rawData,"%f \n",current);
	}
	fclose(rawData);

	// Begin USB closing
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
	// End USB Closing

	return rawData;
}

float** calculateFourierCoefficients(FILE* data, int dataPoints){
	return NULL;

}
float** calculateStokesParameters(float** fourierCoefficients, int dataPoints){
	return NULL;
}

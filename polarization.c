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
#include "fileTools.h"
#include "tempControl.h"
#include "stepperMotorControl.h"
#include "polarizationAnalysisTools.h"
#ifndef DEFINITIONS_H
#define DEFINITIONS_H
	#include "mathTools.h"
#endif

#define REVOLUTIONS 2
#define STEPSPERREV 1200 
#define DATAPOINTSPERREV 60 //Options: 16,20,24,30,40,48,50,60,80,150,240,400,600,1200 // TURNBACKTIME
#define DATAPOINTS (DATAPOINTSPERREV * REVOLUTIONS)
#define PI 3.14159265358979
#define HPCAL 28.1/960.0
#define POLMOTOR 0	// The integer that represents the polarization stepper motor is zero.
#define PROBEMOTOR 1	// The integer that represents the probe stepper motor is one.
#define CCLOCKWISE 0	// 0 spins the motor counterclockwise
#define CLOCKWISE 1	// 1 spins it clockwise

int getPolarizationData(char* fileName, int aout, int dwell);

int main (int argc, char **argv)
{
	int aout;
	int flag;
	int dwell;
	
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
	if (argc==4){
		aout=atoi(argv[1]);
		dwell=atoi(argv[2]);
		strcpy(comments,argv[3]);
		*backgroundFileName=NULL;
	} else {
		printf("There is one option for using this program: \n\n");
		printf("usage '~$ sudo ./polarization <aout_for_HeTarget> <dwell> <comments_in_double_quotes>'\n");
		printf("                                (0-1023)           (1-5)s                            '\n");
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
	fprintf(rawData,"#Cell Temp 1:\t%f\n",getTemperature(3));
	fprintf(rawData,"#Cell Temp 2:\t%f\n",getTemperature(5));
	fprintf(rawData,"#Aout\t%d\n",aout);
	fprintf(rawData,"#Assumed USB1208->HP3617A conversion\t%2.6f\n",HPCAL);
	fprintf(rawData,"#REVOLUTIONS\t%d\n",REVOLUTIONS);
	fprintf(rawData,"#DataPointsPerRev\t%d\n",DATAPOINTSPERREV);
	fprintf(rawData,"#StepsPerRev\t%d\n",STEPSPERREV);
	fprintf(rawData,"#Datapoints\t%d\n",DATAPOINTS);
	fprintf(rawData,"#PMT dwell time (s)\t%d\n",dwell);

	fclose(rawData);

	// Collect raw data
	getPolarizationData(rawDataFileName, aout, dwell); //INCLUDE

	processFileWithBackground(analysisFileName,"NONE",rawDataFileName,DATAPOINTSPERREV,REVOLUTIONS,comments);
	

	// Remove the file indicating that we are taking data.
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

// INCLUDE
int getPolarizationData(char* fileName, int aout, int dwell){

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
	float current,angle;
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

	homeMotor(POLMOTOR); //TURNBACKTIME

	nsteps=STEPSPERREV*REVOLUTIONS;
	ninc=STEPSPERREV/DATAPOINTSPERREV; // The number of steps to take between readings.

	fprintf(rawData,"Steps\tCount\tCurrent\tCurrent Error\tAngle\n");// This line withough a comment is vital for being able to quickly process data. DON'T REMOVE
	printf("Steps\tCounts\tCurrent\n");

	for (steps=0;steps<nsteps;steps+=ninc){

		//200 steps per revoluion
		moveMotor(POLMOTOR,CLOCKWISE,ninc); //TURNBACKTIME
		//moveMotor(PROBEMOTOR,CLOCKWISE,ninc);

		counts=0;
		for (i=0;i<dwell;i++){
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
		angle = (float)steps/STEPSPERREV*2.0*PI;

		printf("%d\t%d\t%f\n",steps,counts,current);
		fprintf(rawData,"%d\t%d\t%f\t%f\t%f\n",steps,counts,current,currentErr,angle);
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

/*
   Program to record excitation function. This is accomplished by 
   stepping up the voltage at the target in increments and recording
   the number of counts at each of those voltages.

   RasPi connected to USB 1204LS.


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
#include "tempControl.h"

#define BUFSIZE 1024
#define NUMCHANNELS 3

float stdDeviation(float* values, int numValues);

int main (int argc, char **argv)
{
	int i,startvalue,endvalue,stepsize,nSamples;
	time_t rawtime;
	struct tm * timeinfo;
	signed short svalue;
	char buffer[BUFSIZE],fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 

	float involts2,involts3;
	float involts[3];
	float prevVal3;
	float slope3;
	float prevSlope3;
	int lowValue[4];
	int k=0;
	int currentDataPoint;
	lowValue[0]=20;

	FILE *fp, *gnuplot, *dataCollectionFlagFile;
	/** Unused RasbPi things.
	__s16 sdata[1024];
	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0;
	**/
	__u16 value;
	__u8  gain;

	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;

	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file \n");
		exit(1);
	}

	// set up USB interface

	ret = hid_init();
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_init failed with return code %d\n", ret);
		return -1;
	}

	if ((interface = PMD_Find_Interface(&hid, 0, USB1208LS_PID)) < 0) {
		fprintf(stderr, "USB 1208LS not found.\n");
		exit(1);
	} else {
		printf("USB 1208LS Device is found! interface = %d\n", interface);
	}



	if (argc==5) {
		startvalue=atoi(argv[1]);
		endvalue=atoi(argv[2]);
		stepsize=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else {
		printf("Usage:\n$ sudo ./RbAbsorbScan <begin> <end> <step> <comments>\n");
		printf("                              ( 0 - 1023 )                   \n");
		return 0;
	}
	if (endvalue>1024) endvalue=1024;
	if (startvalue>1024) endvalue=1024;
	if (startvalue<1) startvalue=0;
	if (endvalue<1) endvalue=0;
	if (startvalue>endvalue) {
		printf("error: startvalue > endvalue.\nYeah, i could just swap them in code.. or you could just enter them in correctly. :-)\n");
		return 1;
	}


	// config mask 0x01 means all inputs
	usbDConfigPort_USB1208LS(hid, DIO_PORTB, DIO_DIR_IN);
	usbDConfigPort_USB1208LS(hid, DIO_PORTA, DIO_DIR_OUT);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);


	// get file name.  use format "RbAbs"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F",timeinfo); //INCLUDE
	if (stat(fileName, &st) == -1){ // Create the directory for the Day's data 
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/RbAbs%F_%H%M%S.dat",timeinfo);

	printf("\n");
	printf(fileName);
	printf("\n");

	printf("Opening File...\n");

	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	fprintf(fp,"#");			//gnuplot needs non-data lines commented out.
	fprintf(fp,fileName);
	fprintf(fp,"\n");
	fflush(fp);

	//TODO Scanf terminates read after hitting a space?!?!?!?!?
	fprintf(fp,"#Comments:\t%s\n",comments);			//gnuplot needs non-data lines commented out.
	fprintf(fp,"#Cell Temp 1:\t%f\n",getTemperature(3));
	fprintf(fp,"#Cell Temp 2:\t%f\n",getTemperature(5));
	fprintf(fp,"Aout\tPUMP\tStdDev\tPROBE\tStdDev\tREF\tStdDev\n");

	gain = BP_5_00V;

	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 32;
	float* measurement = malloc(nSamples*sizeof(float));
	float* reference = malloc(nSamples*sizeof(float));

	currentDataPoint =0;
	for (value=endvalue;value > startvalue && value <= endvalue;value-=stepsize){
		currentDataPoint++;
		usbAOut_USB1208LS(hid, 0, value);
		printf("Aout %d \t",value);
		fflush(stdout);
		fprintf(fp,"%d\t",value);

		// delay to allow transients to settle
		delay(100);
		for(k=0;k<NUMCHANNELS;k++){
			involts[k]=0.0;	
		}

		// grab several readings and average
		for(k=0;k<NUMCHANNELS;k++){
			for (i=0;i<nSamples;i++){
				svalue = usbAIn_USB1208LS(hid,k+1,gain);  //channels are 1-3 for pump,probe, and ref. respectively.
				measurement[i] = volts_LS(gain,svalue);
				involts[k]=involts[k]+measurement[i];
				delay(1);
			}
			involts[k]=fabs(involts[k])/(float)nSamples;
			fprintf(fp,"%f\t%f\t",involts[k],stdDeviation(measurement,nSamples));
			printf("%f\t%f\t",involts[k],stdDeviation(measurement,nSamples));
		}
		fprintf(fp,"\n");
		printf("\n");

		fflush(stdout);
	}

	free(measurement);
	free(reference);


// we might rethink where we end the Aout and return.  this just sets an arbitrary voltage. is there a better choice.
	value=(int)(1.325*617.0);
	usbAOut_USB1208LS(hid,0,value); //sets vout such that 0 v at the probe laser


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

	// Create graphs for data see gnutest.c for an explanation of 
	// how this process works.
	gnuplot = popen("gnuplot","w"); 

	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 158,32\n");
		fprintf(gnuplot, "set output\n");			
		
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Aout (Detuning)'\n");			
		fprintf(gnuplot, "set ylabel 'Transmitted Current'\n");			
		fprintf(gnuplot, "set yrange [-.1:*]\n");			
		fprintf(gnuplot, "set xrange [*:*] reverse\n");			
		sprintf(buffer, "plot '%s' using 1:6:7 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:4:5 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:2:3 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:6:7 with errorbars,\
						 	  '%s' using 1:4:5 with errorbars,\
						 	  '%s' using 1:2:3 with errorbars\n", fileName,fileName,fileName);
		fprintf(gnuplot, buffer);
	}
	pclose(gnuplot);

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

float stdDeviation(float* value, int numValues){
	float stdDev, sum, avg;

	// First Calculate the Average value
	sum = 0.0;
	int i;
	for(i=0; i < numValues;i++){ 
		sum += value[i];
	}
	avg = sum / (float) numValues;

	// Then calculate the Standard Deviation
	sum = 0.0;
	for(i=0; i < numValues;i++){
		sum += pow(avg-value[i],2);
	}
	stdDev = sqrt(sum/(numValues-1));

	return stdDev;
}

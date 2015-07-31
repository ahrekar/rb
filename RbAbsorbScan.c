/*
   Program to record excitation function. This is accomplished by 
   stepping up the voltage at the target in increments and recording
   the number of counts at each of those voltages.

   RasPi connected to USB 1204LS.


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


int main (int argc, char **argv)
{
	int i,startvalue,endvalue,stepsize,steprange;
	time_t rawtime;
	struct tm * timeinfo;
	signed short svalue;
	char buffer[80],fileString[80],comments[1024];
	float involts;
	FILE *fp, *gnuplot;
	__s16 sdata[1024];
	__u16 value;
	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0, channel, gain;

	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;

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
		printf("USB 208LS Device is found! interface = %d\n", interface);
	}



	if (argc==5) {
		startvalue=atoi(argv[1]);
		endvalue=atoi(argv[2]);
		stepsize=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else {
		printf("Usage:\n$ sudo ./RbAbsorbScan <begin> <end> <step> <comments>\n");
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
	strftime(fileString,80,"/home/pi/RbData/RbAbs%F_%H%M%S.dat",timeinfo);

	printf("\n");
	printf(fileString);
	printf("\n");


	fp=fopen(fileString,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	fprintf(fp,"#");			//gnuplot needs non-data lines commented out.
	fprintf(fp,fileString);
	fprintf(fp,"\n");

//TODO Scanf terminates read after hitting a space?!?!?!?!?
	fprintf(fp,"#");			//gnuplot needs non-data lines commented out.
	fprintf(fp,comments);
	fprintf(fp,"\n#Aout\tPhotoCurrent\n");
	channel = 2; //analog input... for photodiode
	gain = BP_5_00V;


	for (value=startvalue;value<endvalue;value+=stepsize){
		usbAOut_USB1208LS(hid, 0, value);
		printf("Aout %d \t",value);
		fflush(stdout);
		fprintf(fp,"%d \t",value);

		// delay to allow transients to settle
		delay(300);
		involts = 0.0;
// grab several readings and average
		for (i=0;i<16;i++){
		svalue = usbAIn_USB1208LS(hid,channel,gain);
		involts=involts+volts_LS(gain,svalue);
		}
		involts=involts/16.0;

		printf("Current %f\n",involts);
		fprintf(fp,"%f \n",involts);

		fflush(stdout);
	}



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

	// Create graphs for data
	gnuplot = popen("gnuplot","w");


	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb\n");
		fprintf(gnuplot, "set output\n");			
		sprintf(buffer, "plot '%s'\n", fileString);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		/**
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s'.png\n", fileString);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s'\n", fileString);
		fprintf(gnuplot, buffer);
		**/
	}
	pclose(gnuplot);

	return 0;
}

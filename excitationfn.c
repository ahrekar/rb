/*
   Program to record excitation function. This is accomplished by 
   stepping up the voltage at the target in increments and recording
   the number of counts at each of those voltages.

   RasPi connected to USB 1208LS.

   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book

   PMT Counts: data received from CTR in USB1208

   The step size will be adjusted to the next available value if the specific Voltage 
   chosen is not available.

Usage: 

./excitationfn.c <filament bias> <target offset> <scan range (0-30)> <step size> <comment less than 80 char>

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
	int counts,i,stepsize,steprange;
	time_t rawtime;
	struct tm * timeinfo;
	signed short svalue;
	char buffer [1024];
	float bias, offset, HPcal,energy,scanrange;
	FILE *fp;
	__s16 sdata[1024];
	__u16 value;
	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0, channel, gain;

	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;

	// Make sure the correct number of arguments were supplied. If not,
	// prompt the user with the proper form for input. 
	//
	if (argc == 6){
		bias = *argv[1];
		offset = *argv[2];
		scanrange = *argv[3];
		stepsize = *argv[4];
		strcpy(buffer,argv[5]);
	} else{
		printf("It seems you made an error in your input, please examine\n");
		printf("the following usage to fix your error.\n");
		printf("    Usage: ./excitationfn.c <filament bias> <target offset> <scan range> <step size> <comments>\n");
		printf("                                                            (   0-30   ) (  1-11   )           \n");
		printf("   Step sizes: 1: 0.029V   5: 0.146V   9: 0.263V                                               \n");
		printf("               2: 0.059V   6: 0.176V  10: 0.293V                                               \n");
		printf("               3: 0.088V   7: 0.205V  11: 0.322V                                               \n");
		printf("               4: 0.117V   8: 0.234V                                                           \n");
		return 1;
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
		printf("USB 208LS Device is found! interface = %d\n", interface);
	}


	// config mask 0x01 means all inputs
	usbDConfigPort_USB1208LS(hid, DIO_PORTB, DIO_DIR_IN);
	usbDConfigPort_USB1208LS(hid, DIO_PORTA, DIO_DIR_OUT);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);


	// get file name.  use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(buffer,80,"/home/pi/RbData/EX%F_%H%M%S.dat",timeinfo);

	printf("\n");
	printf(buffer);


	fp=fopen(buffer,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	fprintf(fp,buffer);

	HPcal=28.1/960.0;
	fprintf(fp,"Assumed USB1208->HP3617A converstion %2.6f\n",HPcal);

	steprange = 1+(int)(scanrange/HPcal);
	if (steprange>1023) steprange = 1023;
	if (steprange < 8 ) steprange = 8;

	if (stepsize<1){
		printf("Step size too small, using 1 (0.029V) instead.\n");
		stepsize=1;
	else if (stepsize > 11){
		printf("Step size too large, using 11 (0.322V) instead.\n");
		stepsize=11;
	}

	fprintf(fp,buffer);

	fprintf(fp,"\nAout\tEnergy\tCounts\tCurrent\n");
	channel = 0; //analog input  for Keithly K617
	gain = BP_10_00V;


	//printf("Starting exciation Function scan Ch1 Aout\n");
	//	temp=1;
	//	channel = (__u8) temp;
	//temp=1;
	//channel = (__u8) temp;

	for (value=0;value<steprange;value+=stepsize){
		usbAOut_USB1208LS(hid, 1, value);
		printf("Aout %d \t",value);
		fflush(stdout);
		fprintf(fp,"%d \t",value);

		energy = bias - (offset + HPcal*(float)value);
		printf("eV %4.2f\t",energy);
		fprintf(fp,"%4.2f\t",energy);

		// delay to allow transients to settle
		delay(500);

		counts=0;
		for (i=0;i<1;i++){
			usbInitCounter_USB1208LS(hid);
			delayMicrosecondsHard(1000000); // wiringPi
			counts+=usbReadCounter_USB1208LS(hid);
		}
		printf("Counts %d\t",counts);
		svalue = usbAIn_USB1208LS(hid,channel,gain);

		printf("Current %f\n",volts_LS(gain,svalue));

		fprintf(fp,"%d \t",counts);
		fprintf(fp,"%f \n",volts_LS(gain,svalue));

		fflush(stdout);
	}

	usbAOut_USB1208LS(hid,1,0);

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

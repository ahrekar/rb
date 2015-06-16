/*
   Program to record excitation function. This is accomplished by 
   stepping up the voltage at the target in increments and recording
   the number of counts at each of those voltages.

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


int main (int argc, char **argv)
{
	int counts,i;
	time_t rawtime;
	struct tm * timeinfo;
	signed short svalue;
	char buffer [80];
	float bias, offset, HPcal,energy;
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
	printf("\n");


	fp=fopen(buffer,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	fprintf(fp,buffer);

	printf("Enter filament bias potential ");
	scanf("%f",&bias);
	fprintf(fp,"\nfilament bias %4.2f\n",bias);

	printf("Enter target offset potential ");
	scanf("%f",&offset);
	fprintf(fp,"target offset %4.2f\n",offset);

	HPcal=28.1/960.0;

	fprintf(fp,"Assumed USB1208->HP3617A converstion %2.6f\n",HPcal);

	printf("Enter, other, single line comments for data run(80 char limit): ");
	scanf("%79s",buffer);
	fprintf(fp,buffer);

	fprintf(fp,"\nAout \t Energy \t Counts \n");
	channel = 0; //analog input  for Keithly K617
	gain = BP_5_00V;


	//printf("Starting exciation Function scan Ch1 Aout\n");
	//	temp=1;
	//	channel = (__u8) temp;
	//temp=1;
	//channel = (__u8) temp;

	/**	TODO Make the Aout step range and sizes
		be user customizeable, probably input
		as an argument to the program. 
		**/
	for (value=0;value<1023;value+=4){
		usbAOut_USB1208LS(hid, 1, value);
		printf("Aout %d \t",value);
		fflush(stdout);
		fprintf(fp,"%d \t",value);

		energy = bias - (offset + HPcal*(float)value);
		printf("eV %4.2f\t",energy);
		fprintf(fp,"%4.2f\t",energy);

		// delay to allow transients to settle
		delay(200);

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





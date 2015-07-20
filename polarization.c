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

#define CLK 0
#define DIR 1

int main (int argc, char **argv)
{
	int counts,i,steps,nsteps,ninc,dwell;
	time_t rawtime;
	struct tm * timeinfo;
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

	// set up for stepmotor

	wiringPiSetup();
	pinMode(CLK,OUTPUT);
	pinMode(DIR,OUTPUT);
	digitalWrite(DIR,1);


	// get file name.  use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(buffer,80,"/home/pi/RbData/POL%F_%H%M%S.dat",timeinfo);

	printf("\n");
	printf(buffer);
	printf("\n");


	fp=fopen(buffer,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}
	printf("Enter filament bias potential ");
	scanf("%f",&bias);

	printf("Enter target offset potential ");
	scanf("%f",&offset);

	fprintf(fp,"filament bias %4.2f\n",bias);
	fprintf(fp,"target offset %4.2f\n",offset);
	// Set up for stepmotor
	printf("Enter total number of steps (1200/revolution) ");
	scanf("%d",&nsteps);
	printf("Enter number of steps per data point");
	scanf("%d",&ninc);
	printf("Enter number of seconds of data to aquire per data point");
	scanf("%d",&dwell);

	if (ninc<1) ninc=1;
	if (ninc>nsteps) ninc=nsteps/2;
	if (dwell<1) dwell=1;

	HPcal=28.1/960.0;
	fprintf(fp,"Assumed USB1208->HP3617A converstion %2.6f\n",HPcal);

	printf("Enter, other, single line comments for data run(80 char limit): ");
	gets(buffer);
	fprintf(fp,buffer);

	// Write the header for the data to the file.
	fprintf(fp,"\nsteps\tCounts\n");

	digitalWrite(CLK,LOW);
	delayMicrosecondsHard(2000);

	for (steps=0;steps<nsteps;steps+=ninc){

		//200 steps per revoluion

		for (i=0;i<ninc;i++){
		// increment steppermotor by ninc steps
		digitalWrite(CLK,HIGH);
		delayMicrosecondsHard(2300);
		digitalWrite(CLK,LOW);
		delayMicrosecondsHard(2300);
		}

		printf("steps %d\t",(steps));
		fprintf(fp,"%d\t",(steps));

		counts=0;
		for (i=0;i<dwell;i++){
			usbInitCounter_USB1208LS(hid);
			delayMicrosecondsHard(1000000); // wiringPi
			counts+=usbReadCounter_USB1208LS(hid);
		}
		printf("Counts %d\n",counts);
		fflush(stdout);
		fprintf(fp,"%d \n",counts);
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





/*
   Program to record polarizqtion.
   RasPi connected to USB 1208LS.


FARADAY ROTATION


steppermotor 1500 steps per revolution. 

use Aout 0 to set laser wavelength. see page 98-100

currently, use same stepper motor driver/port as polarimeter. just swap motor connection. perhaps in future install second driver/port.

ussage

/$ sudo ./faradayrotation <aout> <totalsteps> <deltastep> <comments_no_spaces>

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

#define CLK 0
#define DIR 1
#define DEL 2000
#define PI 3.14159265358979

int main (int argc, char **argv)
{
	int Aout,i,nsteps,ninc,steps,stepsPerRev;
	time_t rawtime;
	signed short svalue;
	float sumsin2b,sumcos2b,angle,count;
	struct tm * timeinfo;
	char buffer[80],comments[80];
	float involts;
FILE *fp;
	__s16 sdata[1024];
	__u16 value;
//	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0, channel, gain;

	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;

	// set up USB interface

	if (argc==5){
		Aout= atoi(argv[1]);
		nsteps=atoi(argv[2]);
		ninc=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else { 
		printf("usage '~$ sudo ./faradayrotation <aout> <numsteps> <stepsize> <comments_no_spaces>'\n");
	return 1;
	}

	sumsin2b=0.0;
	sumcos2b=0.0;
	angle=0.0;
	count=0.0;
	stepsPerRev=1500;



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
	strftime(buffer,80,"/home/pi/RbData/FDayRot%F_%H%M%S.dat",timeinfo);

	printf("\n");
	printf(buffer);
	printf("\n");
	printf(comments);


	fp=fopen(buffer,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	usbAOut_USB1208LS(hid,0,Aout);

	fprintf(fp,"Aout %d\n",Aout);
	if (ninc<1) ninc=1;
	if (ninc>nsteps) ninc=nsteps/2;
	fprintf(fp,buffer);
	fprintf(fp,"\n");
	fprintf(fp,comments);
	fprintf(fp,"\n");

	// Write the header for the data to the file.
	fprintf(fp,"\nsteps\tPhotoDiode\n");

	digitalWrite(CLK,LOW);
	delayMicrosecondsHard(2000);
	channel = 2;// analog input for photodiode
	gain=BP_5_00V;

				//6000    30
	for (steps=0;steps < nsteps;steps+=ninc){

		delay(300);

		//1500?? steps per revoluion
		involts=0.0;
		for (i=0;i<8;i++){
			svalue=usbAIn_USB1208LS(hid,channel,gain);
			involts=involts+volts_LS(gain,svalue);
		}
		involts=involts/8.0;
		angle=2.0*PI*(float)(steps)/(float)stepsPerRev;
		count=count+1.0;
		sumsin2b=sumsin2b+involts*sin(2*angle);
		sumcos2b=sumcos2b+involts*cos(2*angle);


		printf("steps %d\t",(steps));
		fprintf(fp,"%d\t",(steps));
		//delay(100);
		printf("PhotoI %f\n",involts);
		fflush(stdout);
		fprintf(fp,"%f \n",involts);

		for (i=0;i<ninc;i++){
		// increment steppermotor by ninc steps
		digitalWrite(CLK,HIGH);
		delayMicrosecondsHard(DEL);
		digitalWrite(CLK,LOW);
		delayMicrosecondsHard(DEL);
		}

	}
// reverse motor to bring back to same starting point.  This would not be needed
// but there is a small mis-match with the belt-pulley size. 

	digitalWrite(DIR,0);

	printf("moving stepper back\n");
	for (steps=0;steps<nsteps;steps++){
		// increment steppermotor by ninc steps
		digitalWrite(CLK,HIGH);
		delayMicrosecondsHard(DEL);
		digitalWrite(CLK,LOW);
		delayMicrosecondsHard(DEL);

	}


	digitalWrite(DIR,1);

	sumsin2b=sumsin2b/count;
	sumcos2b=sumcos2b/count;
	angle = 0.5*atan(sumsin2b/sumcos2b);
	angle = angle*180.0/PI;
	printf("f3 = %f\n",sumsin2b);
	printf("f4 = %f\n",sumcos2b);
	printf("angle = %f\n",angle);


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





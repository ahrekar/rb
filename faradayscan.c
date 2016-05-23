/*
   Program to record polarizqtion.
   RasPi connected to USB 1208LS.


   FARADAY ROTATION

   steppermotor 1500 steps per revolution. 

   use Aout 0 to set laser wavelength. see page 98-100

   currently, use same stepper motor driver/port as polarimeter. just swap motor connection. perhaps in future install second driver/port.

   ussage

   $ sudo ./faradayscan <aoutstart> <aoutstop> <deltaaout> <comments_no_spaces>

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
#include "stepperMotorControl.h"

#define PI 3.14159265358979
#define NUMSTEPS 700
#define STEPSIZE 25
#define STEPSPERREV 350.0


int main (int argc, char **argv)
{
	int AoutStart,AoutStop,deltaAout,i,steps,Aout,nsamples;
	time_t rawtime;
	signed short svalue;
	float sumI,sumsin2b,sumcos2b,angle,count;
	struct tm * timeinfo;
	char fileName[80], buffer[80],comments[80];
	float involts;
	FILE *fp;
	__s16 sdata[1024];
	__u16 value;
	//	__u16 count;
	//	__u8 gains[8];
	//	__u8 options;
	__u8 input, pin = 0, channel, gain;

	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;

	// set up USB interface

	if (argc==5){
		AoutStart= atoi(argv[1]);
		AoutStop=atoi(argv[2]);
		deltaAout=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else { 
		printf("usage '~$ sudo ./faradayscan <aoutstart> <aoutstop> <deltaaout> < comments in quotes>'\n");
		return 1;
	}

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


	// config mask 0x01 means all inputs
	usbDConfigPort_USB1208LS(hid, DIO_PORTB, DIO_DIR_IN);
	usbDConfigPort_USB1208LS(hid, DIO_PORTA, DIO_DIR_OUT);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);

	// set up for stepmotor


	// get file name.  use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(fileName,80,"/home/pi/RbData/FDayScan%F_%H%M%S.dat",timeinfo);

	printf("\n");
	printf(fileName);
	printf("\n");
	printf(comments);


	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	channel = 2;// analog input for photodiode
	gain=BP_5_00V;

	fprintf(fp,fileName);
	fprintf(fp,"\n");
	fprintf(fp,comments);
	fprintf(fp,"\n");
	// Write the header for the data to the file.
	fprintf(fp,"\nAout\tf0\tf3\tf4\tangle\n");

	for(Aout=AoutStart;Aout<AoutStop;Aout+=deltaAout){
		printf("Aout %d\n",Aout);
		sumsin2b=0.0;
		sumcos2b=0.0;
		angle=0.0;
		count=0.0;
		sumI=0.0;

		usbAOut_USB1208LS(hid,0,Aout);

		for (steps=0;steps < NUMSTEPS;steps+=STEPSIZE){

			delay(150); // watching the o-scope, it looks like it takes ~100ms for the ammeter to settle after a change in LP
			//get samples and average
			nsamples=32;
			involts=0.0;
			for (i=0;i<nsamples;i++){
				svalue=usbAIn_USB1208LS(hid,channel,gain);
				involts=involts+volts_LS(gain,svalue);
				delay(1);
			}
			involts=involts/(float)nsamples;

			angle=2.0*PI*(float)(steps)/STEPSPERREV;
			count=count+1.0;
			sumsin2b=sumsin2b+involts*sin(2*angle);
			sumcos2b=sumcos2b+involts*cos(2*angle);
			sumI+=involts;


			printf("steps %d\t",(steps));
			printf("PhotoI %f\t",involts);
			fflush(stdout);
			moveMotor(1,1,STEPSIZE);
		} // end increment num steps

		sumI=sumI/count;
		sumsin2b=sumsin2b/count;
		sumcos2b=sumcos2b/count;

		// NEEDED there needs to be a check?? for atan.  what if sumcos2b is zero?
		//   CHANGED atan2 takes care of sumcos2b being zero.
		angle = 0.5*atan2(sumcos2b,sumsin2b);
		angle = angle*180.0/PI;

		// WOULD BE NICE.  atan always returns a number between -45 and 45.  
		// once this is done, then return angle in mRad.

		printf("f0 = %f\t",sumI);
		printf("f3 = %f\t",sumsin2b);
		printf("f4 = %f\t",sumcos2b);
		printf("angle = %f\n",angle);
		fprintf(fp,"%d\t%f\t%f\t%f\t%f\n",Aout,sumI,sumsin2b,sumcos2b,angle);

	}//end for Aout

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





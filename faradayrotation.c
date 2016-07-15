/*
   Program to record polarizqtion.
   RasPi connected to USB 1208LS.


   FARADAY ROTATION


   steppermotor 1500 steps per revolution. 

   use Aout 0 to set laser wavelength. see page 98-100

   currently, use same stepper motor driver/port as polarimeter. just swap motor connection. perhaps in future install second driver/port.

   ussage

   /$ sudo ./faradayrotation <aout> <totalsteps> <deltastep> <comments>

Note: Comments must be enclosed in quotes.

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
#include "stepperMotorControl.h"

#define CLK 21		// We will control the clock with pin 0 on the RPi
#define DIR 26		// We will control the direction with pin 1 on the RPi
#define DEL 2000	// Whenever a delay is needed, use this value
#define CWISE 0		// Clockwise =0
#define CCWISE 1	// CounterClockwise =1
#define STEPSPERREV 350; // Define the number of steps in a revolution
#define PI 3.14159265358979 

void moveStepperMotorSingleStep();
void moveStepperMotorSingleStepWithDirection(int dir);
void moveStepperMotor(int dir, int steps);
int closeUSB(HIDInterface* hid);

int main (int argc, char **argv)
{
	int Aout,i,nsteps,ninc,steps,numMeasurements, returnValue;
	time_t rawtime;
	signed short svalue;
	float sumsin2b,sumcos2b,angle,count;
	struct tm * timeinfo;
	char buffer[80],comments[80];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 

	float involts;
	FILE *fp,*dataCollectionFlagFile;
	__s16 sdata[1024];
	__u16 value;
	//	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0, channel, gain;

	// set up USB interface
	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;
	
	//Setup for wiring Pi
	wiringPiSetup();

	if (argc==5){ // Note that first argument (argv[0]) is the name of the command.
		Aout= atoi(argv[1]);
		nsteps=atoi(argv[2]);
		ninc=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else { 
		printf("usage '~$ sudo ./faradayrotation <aout> <numsteps> <stepsize> <comments_no_spaces>'\n");
		return 1;
	}

	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file \n");
		exit(1);
	}

	sumsin2b=0.0;
	sumcos2b=0.0;
	angle=0.0;
	count=0.0;

	// Check to make sure USB DAQ is properly connected
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

	// get file name.  use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(buffer,80,"/home/pi/RbData/%F/FDayRot%F_%H%M%S.dat",timeinfo);

	// Print filename to screen
	printf("\n");
	printf(buffer);
	printf("\n");

	// Open the file for data writing.
	fp=fopen(buffer,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	usbAOut_USB1208LS(hid,0,Aout);

	// Record the starting value of Aout in the file.
	fprintf(fp,"#Aout %d\n",Aout);
	if (ninc<1) ninc=1;
	if (ninc>nsteps) ninc=nsteps/2;

	// Print the filename inside the file
	fprintf(fp,"#");
	fprintf(fp,buffer);
	fprintf(fp,"\n");

	// Print the comments in the file.
	fprintf(fp,"#");
	fprintf(fp,comments);
	fprintf(fp,"\n");

	// Write the header for the data to the file.
	fprintf(fp,"\n");
	fprintf(fp,"#");
	fprintf(fp,"Steps\tPhotoDiode\n");

	channel = 2;// analog input for photodiode
	gain=BP_5_00V;

	//6000    30
	for (steps=0;steps < nsteps;steps+=ninc){

		delay(150);

		//1500?? steps per revoluion
		involts=0.0;
		numMeasurements=32;

		for (i=0;i<numMeasurements;i++){
			svalue=usbAIn_USB1208LS(hid,channel,gain);
			involts=involts+volts_LS(gain,svalue);
		}

		involts=involts/(float)numMeasurements;
		angle=2.0*PI*(float)(steps)/(float)STEPSPERREV;
		count=count+1.0;
		sumsin2b=sumsin2b+involts*sin(2*angle);
		sumcos2b=sumcos2b+involts*cos(2*angle);


		printf("steps %d\t",(steps));
		fprintf(fp,"%d\t",(steps));
		//delay(100);
		printf("PhotoI %f\n",involts);
		fflush(stdout);
		fprintf(fp,"%f \n",involts);

		// increment steppermotor by ninc steps
		moveMotor(1,CCWISE,ninc);
	}

	sumsin2b=sumsin2b/count;
	sumcos2b=sumcos2b/count;
	angle = 0.5*atan2(sumcos2b,sumsin2b);
	angle = angle*180.0/PI;
	printf("f3 = %f\n",sumsin2b);
	printf("f4 = %f\n",sumcos2b);
	printf("angle = %f\n",angle);

	// Close the data file
	fclose(fp);

	//cleanly close USB
	returnValue = closeUSB(hid);

	// Remove the file indicating that we are taking data.
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return returnValue;
}

int closeUSB(HIDInterface* hid){
	hid_return ret;
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

/*
   Program to record polarization.
   RasPi connected to USB 1208LS.

   FARADAY SCAN


   use Aout 0 to set laser wavelength. see page 98-100
   usage
   $ sudo ./faradayscan <aoutstart> <aoutstop> <deltaaout> <comments_no_spaces>


   2015-12-31
   added error calculations. see page 5 and 6 of "FALL15" lab book
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
#include "mathTools.h" //includes stdDeviation

#define CLK 21
#define DIR 26
#define DEL 1500
#define PI 3.14159265358979
#define NUMSTEPS 350	
#define STEPSIZE 25
#define STEPSPERREV 350.0

#define BASE 100
#define SPI_CHAN 0

int main (int argc, char **argv)
{
	int AoutStart,AoutStop,deltaAout,i,steps,Aout,nsamples,j;
	time_t rawtime;
	signed short svalue;
	float sumI, sumSin, sumCos;
	float f4,f3,df4,df3,angle,stderrangle,count;
	struct tm * timeinfo;
	char fileName[80], buffer[80], comments[80];
	float involts; 	// The amount of light that is entering into the sensor. 
	float stderrinvolts;
	FILE *fp;
	__s16 sdata[1024];
	__u16 value;
	//	__u16 count;
	//	__u8 gains[8];
	//	__u8 options;
	__u8 input, pin = 0, channel, gain;

	int x;
	int chan;
	float CVGauge,IonGauge; // Rb target CV Gauge (buffer pressure)

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
		printf("usage '~$ sudo ./faradayscan <aoutstart> <aoutstop> <deltaaout> <comments in quotes>'\n");
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

	nsamples=32;
	float* measurement = malloc(nsamples*sizeof(float));

	// set up for stepmotor

	wiringPiSetup();
	mcp3004Setup(BASE,SPI_CHAN);

	// get file name.  use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(fileName,80,"/home/pi/RbData/FDayScan2%F_%H%M%S.dat",timeinfo);

	printf("%s\n",fileName);
	printf("%s\n",comments);

	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	fprintf(fp,"%s\n%s\n",fileName,comments);

	chan = 1; // IonGauge
	x=analogRead(BASE + chan);
	IonGauge = 0.0107 * (float)x;
	IonGauge = pow(10,(IonGauge-9.97));
	printf("IonGauge %2.2E Torr \n",IonGauge);
	fprintf(fp,"IonGauge %2.2E Torr \n",IonGauge);

	chan = 3; //pressure
	x=analogRead(BASE + chan);
	CVGauge = (float)x;
	CVGauge = pow(10,(0.00499*CVGauge - 4.05));
	printf("CVGauge %2.2E Torr\n", CVGauge);
	fprintf(fp,"CVGauge %2.2E Torr\n", CVGauge);


	channel = 2;// analog input for photodiode
	gain=BP_5_00V;

	// Write the header for the data to the file.
	fprintf(fp,"\nFlag\tAout\tf0\tf3\td-f3\tf4\td-f4\tangle\n");

	for(Aout=AoutStart;Aout<AoutStop;Aout+=deltaAout){
		//for (j=0;j<2;j++){

		//	usbDOut_USB1208LS(hid, DIO_PORTA, (j*2));  // this sets the  beam flag

			printf("Aout %d\n",Aout);
			sumSin=0.0;
			sumCos=0.0;
			df4=0.0;
			df3=0.0;
			angle=0.0;
			count=0.0;
			sumI=0.0;

			usbAOut_USB1208LS(hid,0,Aout);

			for (steps=0;steps < NUMSTEPS;steps+=STEPSIZE){ // We want to go through a full revolution of the linear polarizer
				// (NUMSTEPS) in increments of STEPSIZE

				delay(150); // watching the o-scope, it looks like it takes ~100ms for the ammeter to settle after a change in LP
				//get samples and average
				involts=0.0;	
				for (i=0;i<nsamples;i++){ // Take several samples of the voltage and average them.
					svalue=usbAIn_USB1208LS(hid,channel,gain);
					measurement[i]=volts_LS(gain,svalue);
					involts=involts+measurement[i];
					delay(2);
				}
				involts=involts/(float)nsamples; 

				stderrinvolts = stdDeviation(measurement,nsamples);

				angle=2.0*PI*(float)(steps)/STEPSPERREV; // Calculate the angle in radians of the axis of the LP
				count=count+1.0;
				sumSin+=involts*sin(2*angle);
				sumCos+=involts*cos(2*angle);
				sumI+=involts;
				df3+=pow(stderrinvolts,2)*pow(sin(2*angle),2);
				df4+=pow(stderrinvolts,2)*pow(cos(2*angle),2);

				printf("steps %d\t",(steps));
				printf("PhotoI %f\t",involts);
				fflush(stdout);

				moveMotor(1,1,STEPSIZE);

			}
			sumI=sumI/count;
			f3=sumSin/count;
			f4=sumCos/count;
			df3=sqrt(df3)/count;
			df4=sqrt(df4)/count;

			// NEEDED there needs to be a check?? for atan.  what if sumcos2b is zero?
			//     CHANGED: Implemented atan2, which handles when f3 is zero.
			angle = 0.5*atan2(f4,f3);
			angle = angle*180.0/PI;

			//		stderrangle=pow(f4/f3,2);
			stderrangle=(1/(1+pow(f4/f3,2)))*sqrt(pow(f3,-2))*(sqrt(pow(df4,2) + stderrangle*pow(df3,2))/2.0);

			stderrangle = stderrangle*180.0/PI;

			printf("f0 = %f\t",sumI);
			printf("f3 = %f\t",f3);
			printf("f4 = %f\t",f4);
			printf("angle = %f (%f)\n",angle,stderrangle);
			// As a reminder, these are the headers: fprintf(fp,"\nFlag\tAout\tf0\tf3\td-f3\tf4\td-f4\tangle\n");
			fprintf(fp,"%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",j,Aout,sumI,f3,df3,f4,df4,angle,stderrangle);
		//}//end j
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

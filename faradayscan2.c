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
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include <wiringPi.h>
#include "pmd.h"
#include "usb-1208LS.h"
#include "mathTools.h" //includes stdDeviation
#include "tempControl.h"
#include "interfacing/interfacing.h"

#define CLK 21
#define DIR 26
#define DEL 1500
#define PI 3.14159265358979
#define NUMSTEPS 350	
#define STEPSIZE 25
#define STEPSPERREV 350.0

#define BASE 100
#define SPI_CHAN 0
#define BUFSIZE 1024

int plotData(char* fileName);

int main (int argc, char **argv)
{
	int AoutStart,AoutStop,deltaAout,i,steps,Aout,nsamples;
	time_t rawtime;
	signed short svalue;
	float sumI, sumSin, sumCos;
	float f4,f3,df4,df3,angle,stderrangle,count;
	float returnFloat;
	struct tm * timeinfo;
	char fileName[BUFSIZE], comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 

	float involts; 	// The amount of light that is entering into the sensor. 
	float stderrinvolts;
	FILE *fp,*dataCollectionFlagFile;
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


	if (argc==5){
		AoutStart= atoi(argv[1]);
		AoutStop=atoi(argv[2]);
		deltaAout=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else { 
		printf("usage '~$ sudo ./faradayscan <aoutstart> <aoutstop> <deltaaout> <comments in quotes>'\n");
		return 1;
	}

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

	// Setup wiringPi
	wiringPiSetup();


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
	struct stat st = {0};
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F",timeinfo); //INCLUDE
	if (stat(fileName, &st) == -1){
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/FDayScan%F_%H%M%S.dat",timeinfo); //INCLUDE

	printf("%s\n",fileName);
	printf("%s\n",comments);

	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	fprintf(fp,"# %s\n# %s\n",fileName,comments);

	getIonGauge(&returnFloat);
	printf("IonGauge %2.2E Torr \n",returnFloat);
	fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

	getConvectron(GP_N2_CHAN,&returnFloat);
	printf("CVGauge(N2) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(N2)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_HE_CHAN,&returnFloat);
	printf("CVGauge(He) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);

	fprintf(fp,"# Cell Temp 1:\t%f\n",getTemperature(3));
	fprintf(fp,"# Cell Temp 2:\t%f\n",getTemperature(5));


	channel = 2;// analog input for photodiode
	gain=BP_5_00V;

	// Write the header for the data to the file.
	fprintf(fp,"Aout\tf0\tf3\td-f3\tf4\td-f4\tangle\tangleError\n");

	for(Aout=AoutStart;Aout<AoutStop;Aout+=deltaAout){

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

			//printf("steps %d\t",(steps));
			//printf("PhotoI %f\t",involts);
			//fflush(stdout);

			stepMotor(PROBE_MOTOR,CLK,STEPSIZE);

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
		// As a reminder, these are the headers: fprintf(fp,"Aout\tf0\tf3\td-f3\tf4\td-f4\tangle\tangleError\n");
		fprintf(fp,"%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",Aout,sumI,f3,df3,f4,df4,angle,stderrangle);
	}//end for Aout

	fclose(fp);

	plotData(fileName);

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

	// Remove the file indicating that we are taking data.
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

int plotData(char* fileName){
	char buffer[BUFSIZE];
	FILE *gnuplot;
	gnuplot = popen("gnuplot","w"); 

	if (gnuplot != NULL){
		fprintf(gnuplot, "set terminal dumb size 100,28\n");
		fprintf(gnuplot, "set output\n");			

		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnheader\n");
		fprintf(gnuplot, "set xlabel 'Aout (Detuning)'\n");			
		fprintf(gnuplot, "set ylabel 'Theta'\n");			
		fprintf(gnuplot, "set xrange [0:1024] reverse\n");			
		sprintf(buffer, "plot '%s' using 1:7:8 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "unset output\n"); 
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s.png'\n", fileName);
		fprintf(gnuplot, buffer);
		sprintf(buffer, "plot '%s' using 1:7:8 with errorbars\n",fileName);
		fprintf(gnuplot, buffer);
	}
	return pclose(gnuplot);
}

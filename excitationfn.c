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

./excitationfn.c <filament bias> <target offset> <scan range (0-30)> <step size> <comments>

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
#define BASE 100
#define SPI_CHAN 0

float stdDeviation(float* values, int numValues);

int main (int argc, char **argv)
{
	int counts,i,stepsize,steprange,chan;
	int minstepsize,maxstepsize, nSamples;
	int dwell;
	time_t rawtime;
	struct tm * timeinfo;
	signed short svalue;
	char buffer[80],fileString[80],comments[1024];
	float bias, offset, HPcal,energy,scanrange, involts;
	FILE *fp;
	/** These are not used in this program, but might
		be useful in the future.
	__s16 sdata[1024];
	_u16 count;
	_u8 gains[8];
	_u8 options,input,pin = 0;
	**/
	__u16 value;
	__u8 channel,gain;

	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;

	// Make sure the correct number of arguments were supplied. If not,
	// prompt the user with the proper form for input. 
	if (argc == 7){
		bias = atof(argv[1]);
		offset = atof(argv[2]);
		scanrange =atof(argv[3]);
		stepsize = atoi(argv[4]);
		dwell= atoi(argv[5]);
		strcpy(comments,argv[6]);
	} else{
		printf("Hey, DUMBASS. you made an error in your input, please examine\n");
		printf("the following usage to fix your error.\n");
		printf("...dumbass\n");
		printf("                                                                                               \n");
		printf("    Usage:                                                                                     \n");
		printf("           sudo ./excitationfn <filament bias> <target offset> <scan range> <step size> <dwell time> <comments>\n");
		printf("                                                               (   0-30   ) (  1-24   )    (1-5)s       \n");
		printf("                                                                                               \n");
		printf("   Step sizes:                                                                                 \n");
		printf("               1: 0.029V    9: 0.263V   17: 0.497V                                             \n");
		printf("               2: 0.059V   10: 0.293V   18: 0.527V                                             \n");
		printf("               3: 0.088V   11: 0.322V   19: 0.556V                                             \n");
		printf("               4: 0.117V   12: 0.351V   20: 0.585V                                             \n");
		printf("               5: 0.146V   13: 0.381V   21: 0.615V                                             \n");
		printf("               6: 0.176V   14: 0.410V   22: 0.644V                                             \n");
		printf("               7: 0.205V   15: 0.439V   23: 0.673V                                             \n");
		printf("               8: 0.234V   16: 0.468V   24: 0.703V                                             \n");
		printf("                                                                                               \n");
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

mcp3004Setup(BASE,SPI_CHAN);


	// config mask 0x01 means all inputs
	usbDConfigPort_USB1208LS(hid, DIO_PORTB, DIO_DIR_IN);
	usbDConfigPort_USB1208LS(hid, DIO_PORTA, DIO_DIR_OUT);
//	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);
//	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);


	// get file name.  use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(fileString,80,"/home/pi/RbData/EX%F_%H%M%S",timeinfo);

	sprintf(buffer,"%s.dat",fileString);
	printf("\n%s\n",buffer);

	fp=fopen(buffer,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}

	fprintf(fp,"#");
	fprintf(fp,"%s\n",buffer);

	HPcal=28.1/960.0;
	fprintf(fp,"#Assumed USB1208->HP3617A converstion %2.6f\n",HPcal);

	steprange = 1+(int)(scanrange/HPcal);
	if (steprange>1023) steprange = 1023;
	if (steprange < 8 ) steprange = 8;

	minstepsize=1;
	maxstepsize=24;
	if (stepsize<minstepsize){
		printf("Step size too small, using %d (%0.3fV) instead.\n",minstepsize,minstepsize*HPcal);
		stepsize=minstepsize;
	}
	else if (stepsize > maxstepsize){
		printf("Step size too large, using %d (%0.3fV) instead.\n",maxstepsize,maxstepsize*HPcal);
		stepsize=maxstepsize;
	}

	fprintf(fp,"#");
	fprintf(fp,comments);
	fprintf(fp,"\n");

	// Print the header for the information in the datafile
	fprintf(fp,"Aout\tEnergy\tCount\tCountStDev\tCurrent\tCurrentStDev\tIonGauge\n");
	channel = 0; //analog input  for Keithly K617
	gain = BP_10_00V;


	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 16;
	float* measurement = malloc(nSamples*sizeof(float));

	for (value=0;value<steprange;value+=stepsize){
		usbAOut_USB1208LS(hid, 1, value);
		printf("Aout %d \t",value);
		fflush(stdout);
		fprintf(fp,"%d \t",value);

		energy = bias - (offset + HPcal*(float)value);
		printf("eV %4.2f\t",energy);
		fprintf(fp,"%4.4f\t",energy);

		// delay to allow transients to settle
		delay(500);

		counts=0;
		for (i=0;i<1;i++){
			usbInitCounter_USB1208LS(hid);
			delayMicrosecondsHard(dwell*1000000); // wiringPi
			counts+=usbReadCounter_USB1208LS(hid);
		}
		printf("Counts %d\t",counts);

		involts = 0.0;
		// grab several readings and average
		for (i=0;i<nSamples;i++){
			svalue = usbAIn_USB1208LS(hid,0,gain);  //channel = 0 for k617
			measurement[i] = volts_LS(gain,svalue);
			involts=involts+measurement[i];
		}

		involts=involts/(float)nSamples;

		printf("Current %f\t",volts_LS(gain,svalue));

		fprintf(fp,"%d\t%f\t",counts,sqrt(counts));
		fprintf(fp,"%f\t%f\t",involts,stdDeviation(measurement,nSamples));

		involts = 0.0;
		chan = 1;
		// grab several readings and average
		for (i=0;i<8;i++){
			svalue= analogRead(BASE+chan);
			involts = involts + 0.0107 * (float) svalue;
		}
		involts=involts/8.0;
		involts = pow(10,involts-9.97);
		printf("IG= %2.2E \n",involts);
		fprintf(fp,"%2.4E\n",involts);

		fflush(stdout);
	}

	free(measurement);

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

	// Create graphs for data see gnutest.c for an explanation of 
	// how this process works.
	FILE *gnuplot;
	gnuplot = popen("gnuplot","w"); 

	if (gnuplot != NULL){
		// First print to the terminal screen.

		// Set up the output for printing to terminal
		fprintf(gnuplot, "set terminal dumb size 160,30\n");
		fprintf(gnuplot, "set output\n");			
		fprintf(gnuplot, "set key autotitle columnheader\n");			

		// Set up the axis for the first plot
		sprintf(buffer, "set title '%s'\n", fileString);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "set xlabel 'Energy'\n");			
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using 2:3:4 with errorbars\n", fileString);
		fprintf(gnuplot, buffer);

		// Set up the axis for the second plot x axis stays the same
		sprintf(buffer, "set title '%s'\n", fileString);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		fprintf(gnuplot, "set yrange [*:1]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using 2:5:6 with errorbars\n", fileString);
		fprintf(gnuplot, buffer);
		// End printing to screen

		// Clear the previous output settings, we no longer want to 
		// output to the terminal.
		fprintf(gnuplot, "unset output\n"); 

		// Then print to an image file.
		
		// Set up the output.
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s_counts.png'\n", fileString);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "set key autotitle columnhead\n");			
		// Set up the axis labels
		sprintf(buffer, "set title '%s'\n", fileString);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "set yrange [0:*]\n");			
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		// Print the plot
		sprintf(buffer, "plot '%s.dat' using 2:3:4 with errorbars\n", fileString);
		fprintf(gnuplot, buffer);

		fprintf(gnuplot, "unset output\n"); 

		sprintf(buffer, "set output '%s_current.png'\n", fileString);
		fprintf(gnuplot, buffer);
		// Set up the axis labels, x stays the same
		sprintf(buffer, "set title '%s'\n", fileString);
		fprintf(gnuplot, buffer);
		fprintf(gnuplot, "set yrange [*:.1]\n");			
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		// Print the plot
		sprintf(buffer, "plot '%s.dat' using 2:5:6 with errorbars\n", fileString);
		fprintf(gnuplot, buffer);
	}
	pclose(gnuplot);

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

/*
   Program to record excitation function. This is accomplished by 
   stepping up the voltage at the target in increments and recording
   the number of counts at each of those voltages.

   RasPi connected to USB 1208LS.

   Target primaryEnergy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book

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
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include <wiringPi.h>
#include "interfacing/interfacing.h"
#include "mathTools.h"
#define BUFSIZE 1024
#define HPCAL 31.65/1023.0

void graphData(char* fileName);

int main (int argc, char **argv)
{
	int i,stepsize,steprange;
	long totalCounts;
	int minstepsize,maxstepsize, nSamples;
	int dwell,magnitude;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[BUFSIZE],fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	float bias, HeOffset, N2Offset,primaryEnergy, secondaryEnergy, scanrange;
	float returnFloat;
	float current, pressure;
	long returnCounts;
	FILE *fp,*dataCollectionFlagFile;
	__u16 value;

	// Make sure the correct number of arguments were supplied. If not,
	// prompt the user with the proper form for input. 
	if (argc == 9){
		bias = atof(argv[1]);
		N2Offset = atof(argv[2]);
		HeOffset = atof(argv[3]);
		scanrange =atof(argv[4]);
		stepsize = atoi(argv[5]);
		dwell= atoi(argv[6]);
		magnitude= atoi(argv[7]);
		strcpy(comments,argv[8]);
	} else{
		printf("Hey, DUMBASS. you made an error in your input, please examine\n");
		printf("the following usage to fix your error.\n");
		printf("...dumbass\n");
		printf("                                                                                               \n");
		printf("    Usage:                                                                                     \n");
		printf("           sudo ./excitationfn <filament bias> <N2 Offset> <He offset> <scan range> <step size> <dwell time> <orderOfMagnitudeOfCurrent> <comments>\n");
		printf("                                (assumed neg.)            (assmd. neg) (   0-30   ) (  1-24   )    (1-5)s       \n");
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

	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("Unable to open file \n");
		exit(1);
	}

	// set up USB interface
	initializeBoard();
	initializeUSB1208();

	// get file name.  use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(fileName,80,"/home/pi/RbData/%F",timeinfo); //INCLUDE
	if (stat(fileName, &st) == -1){
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(fileName,80,"/home/pi/RbData/%F/EX%F_%H%M%S",timeinfo);

	sprintf(buffer,"%s.dat",fileName);
	printf("\n%s\n",buffer);

	fp=fopen(buffer,"w");
	if (!fp) {
		printf("Unable to open file: %s\n",buffer);
		exit(1);
	}

	fprintf(fp,"#Filename:\t%s\n",buffer);

	fprintf(fp,"#USB1208->HP3617Aconversion:\t%2.6f\n",HPCAL);

	steprange = 1+(int)(scanrange/(HPCAL));
    printf("Step range:%d\n",steprange);
	if (steprange>1023) steprange = 1023;
	if (steprange < 8 ) steprange = 8;

	minstepsize=1;
	maxstepsize=24;
	if (stepsize<minstepsize){
		printf("Step size too small, using %d (%0.3fV) instead.\n",minstepsize,minstepsize*HPCAL);
		stepsize=minstepsize;
	}
	else if (stepsize > maxstepsize){
		printf("Step size too large, using %d (%0.3fV) instead.\n",maxstepsize,maxstepsize*HPCAL);
		stepsize=maxstepsize;
	}

	fprintf(fp,"#FilamentBias:\t%f\n",-bias);
	fprintf(fp,"#NumberOfSecondsPerCountMeasurement:\t%d\n",dwell);
	fprintf(fp,"#Comments:\t%s\n",comments);

	
	getIonGauge(&returnFloat);
	printf("IonGauge %2.2E Torr \n",returnFloat);
	fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

	getConvectron(GP_N2_CHAN,&returnFloat);
	printf("CVGauge(N2) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(N2)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_HE_CHAN,&returnFloat);
	printf("CVGauge(He) %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(He)(Torr):\t%2.2E\n", returnFloat);

	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#CellTemp(Res):\t%f\n",returnFloat);
	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#CellTemp(Targ):\t%f\n",returnFloat);

	fprintf(fp,"#MagnitudeOfCurrent(*10^-X):\t%d\n",magnitude);


	// Print the header for the information in the datafile
	fprintf(fp,"Aout\tbias\tN2Offset\tTotalHeOffset\tPrimaryElectronEnergy\tSecondaryElectronEnergy\tCount\tCountStDev\tCurrent\tCurrentStDev\tIonGauge\tIGStdDev\n");

	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 16;
	float* measurement = malloc(nSamples*sizeof(float));

	totalCounts=0;
	for (value=0;value<steprange;value+=stepsize){
		setUSB1208AnalogOut(HETARGET,value);
		printf("Aout %d \t",value);
		fprintf(fp,"%d\t",value);

		fprintf(fp,"%4.4f\t",-bias);
		fprintf(fp,"%4.4f\t",N2Offset);
		fprintf(fp,"%4.4f\t",HeOffset + HPCAL*(float)value);

		primaryEnergy = bias + (-HeOffset - HPCAL*(float)value);
		printf("eV %4.2f\t",primaryEnergy);
		fprintf(fp,"%4.4f\t",primaryEnergy);

		secondaryEnergy =  (bias - N2Offset) + (-HeOffset - HPCAL*(float)value);
		printf("eV %4.2f\t",secondaryEnergy);
		fprintf(fp,"%4.4f\t",secondaryEnergy);

		// delay to allow transients to settle
		delay(500);

		getUSB1208Counter(dwell*10,&returnCounts);
		printf("Counts %ld\t",returnCounts);
		totalCounts+=returnCounts;

		current = 0.0;
		// grab several readings and average
		for (i=0;i<nSamples;i++){
			getUSB1208AnalogIn(K617,&measurement[i]);
			current+=measurement[i];
		}

		current=current/(float)nSamples;

		printf("Current %f\t",current);

		fprintf(fp,"%ld\t%Lf\t",returnCounts,sqrtl(returnCounts));
		fprintf(fp,"%f\t%f\t",-current,stdDeviation(measurement,nSamples));

		// Grab several readings and average
		pressure=0;
		for (i=0;i<nSamples;i++){
			getIonGauge(&measurement[i]);
			pressure+=measurement[i];
		}
		pressure=pressure/(float)nSamples;
		printf("IG= %2.2E \n",pressure);
		fprintf(fp,"%2.4E\t%2.4E\n",pressure,stdDeviation(measurement,nSamples));
	}

	setUSB1208AnalogOut(HETARGET,0);

	closeUSB1208();

	free(measurement);
	fclose(fp);

	graphData(fileName);

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

void graphData(char* fileName){
	// Create graphs for data see gnutest.c for an explanation of 
	// how this process works.
	FILE *gnuplot;
	char buffer[BUFSIZE];
	gnuplot = popen("gnuplot","w"); 

	if (gnuplot != NULL){
		// First print to the terminal screen.

		// Set up the output for printing to terminal
		fprintf(gnuplot, "set terminal dumb size 100,28\n");
		fprintf(gnuplot, "set output\n");			
		fprintf(gnuplot, "set key autotitle columnheader\n");			

		// Set up the axis for the first plot
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "set xlabel 'Energy'\n");			
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using 5:7:8 with yerrorbars\n", fileName);
		fprintf(gnuplot, "%s", buffer);

		// Set up the axis for the second plot x axis stays the same
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using 5:9:10 with yerrorbars\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		// End printing to screen

		// Clear the previous output settings, we no longer want to 
		// output to the terminal.
		fprintf(gnuplot, "unset output\n"); 

		// Then print to an image file.

		// Set up the output.
		fprintf(gnuplot, "set terminal png\n");
		sprintf(buffer, "set output '%s_counts.png'\n", fileName);
		fprintf(gnuplot, "%s",buffer);

		fprintf(gnuplot, "set key autotitle columnhead\n");			
		// Set up the axis labels
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "set yrange [0:*]\n");			
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		// Print the plot
		sprintf(buffer, "plot '%s.dat' using 5:7:8 with yerrorbars\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "unset output\n"); 

		sprintf(buffer, "set output '%s_current.png'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		// Set up the axis labels, x stays the same
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "set yrange [0:*]\n");			
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		// Print the plot
		//fprintf(fp,"Aout\tbias\tN2Offset\tTotalHeOffset\tPrimaryElectronEnergy\tSecondaryElectronEnergy\tCount\tCountStDev\tCurrent\tCurrentStDev\tIonGauge\n");
		sprintf(buffer, "plot '%s.dat' using 5:9:10 with yerrorbars\n", fileName);
		fprintf(gnuplot, "%s",buffer);
	}
	pclose(gnuplot);

}

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

void graphData(char* fileName);

int main (int argc, char **argv)
{
	int i,stepsize,steprange;
	int minstepsize,maxstepsize, nSamples;
	int dwell,magnitude;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[BUFSIZE],fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	float bias, HeOffset, N2Offset, N2Sweep,primaryEnergy, secondaryEnergy, scanrange;
	float returnFloat;
	float current, pressure;
	long returnCounts;
	FILE *fp,*dataCollectionFlagFile;
	__u16 value;

	// Make sure the correct number of arguments were supplied. If not,
	// prompt the user with the proper form for input. 
    int expectedArguments=10;
	if (argc == expectedArguments){
		bias = atof(argv[1]);
		N2Offset = atof(argv[2]);
        N2Sweep = atof(argv[3]);
		HeOffset = atof(argv[4]);
		scanrange =atof(argv[5]);
		stepsize = atoi(argv[6]);
		dwell= atoi(argv[7]);
		magnitude= atoi(argv[8]);
		strcpy(comments,argv[9]);
	} else{
		printf("Hey, DUMBASS. you made an error in your input, please examine\n");
		printf("the following usage to fix your error.\n");
		printf("...dumbass. You supplied %d arguments, %d were expected\n", argc-1,expectedArguments-1);
		printf("                                                                                               \n");
		printf("    Usage:                                                                                     \n");
		printf("           sudo ./excitationfn <filament bias> <N2 Offset> <N2 Sweep> <He offset> <scan range> <step size> <dwell time> <orderOfMagnitudeOfCurrent> <comments>\n");
		printf("                                (remember neg.)                     (remember neg.!) (   0-30   ) (  1-24   )    (1-5)s       \n");
		printf("                                                                                               \n");
		printf("   Step sizes:                                                                                 \n");
		printf("               1: %1.3fV    9: %1.3fV   17: %1.3fV  25: %1.3fV           \n",1*HPCAL,9*HPCAL,17*HPCAL,25*HPCAL);
		printf("               2: %1.3fV   10: %1.3fV   18: %1.3fV  26: %1.3fV           \n",2*HPCAL,10*HPCAL,18*HPCAL,26*HPCAL);
		printf("               3: %1.3fV   11: %1.3fV   19: %1.3fV  27: %1.3fV           \n",3*HPCAL,11*HPCAL,19*HPCAL,27*HPCAL);
		printf("               4: %1.3fV   12: %1.3fV   20: %1.3fV  28: %1.3fV           \n",4*HPCAL,12*HPCAL,20*HPCAL,28*HPCAL);
		printf("               5: %1.3fV   13: %1.3fV   21: %1.3fV  29: %1.3fV           \n",5*HPCAL,13*HPCAL,21*HPCAL,29*HPCAL);
		printf("               6: %1.3fV   14: %1.3fV   22: %1.3fV  30: %1.3fV           \n",6*HPCAL,14*HPCAL,22*HPCAL,30*HPCAL);
		printf("               7: %1.3fV   15: %1.3fV   23: %1.3fV  31: %1.3fV           \n",7*HPCAL,15*HPCAL,23*HPCAL,31*HPCAL);
		printf("               8: %1.3fV   16: %1.3fV   24: %1.3fV  32: %1.3fV           \n",8*HPCAL,16*HPCAL,24*HPCAL,32*HPCAL);
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

	fprintf(fp,"#FilamentBias:\t%f\n",bias);
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

    /** Temperature Controllers **/
	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#CurrTemp(Res):\t%f\n",returnFloat);
	getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#SetTemp(Res):\t%f\n",returnFloat);

	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#CurrTemp(Targ):\t%f\n",returnFloat);
	getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#SetTemp(Targ):\t%f\n",returnFloat);

	fprintf(fp,"#MagnitudeOfCurrent(*10^-X):\t%d\n",magnitude);

	// Print the header for the information in the datafile
	fprintf(fp,"Aout\tbias\tN2Offset\tN2Sweep\tTotalHeOffset\tPrimaryElectronEnergy\tSecondaryElectronEnergy\tCountRate\tCountRateStDev\tCurrent\tCurrentStDev\tIonGauge\tIGStdDev\n");

	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 16;
	float* measurement = malloc(nSamples*sizeof(float));

	for (value=0;value<steprange;value+=stepsize){
		setUSB1208AnalogOut(HETARGET,value);
		printf("Aout %d \t",value);
		fprintf(fp,"%d\t",value);

		fprintf(fp,"%4.4f\t",bias);
		fprintf(fp,"%4.4f\t",N2Offset);
		fprintf(fp,"%4.4f\t",N2Sweep);
		fprintf(fp,"%4.4f\t",HeOffset - HPCAL*(float)value);

		primaryEnergy = (HeOffset - HPCAL*(float)value) - bias;
		printf("eV %4.2f\t",primaryEnergy);
		fprintf(fp,"%4.4f\t",primaryEnergy);

		secondaryEnergy = (HeOffset - HPCAL*(float)value) - (bias + N2Offset) ;
		printf("eV %4.2f\t",secondaryEnergy);
		fprintf(fp,"%4.4f\t",secondaryEnergy);

		// delay to allow transients to settle
		delay(500);

		getUSB1208Counter(dwell*10,&returnCounts);
		printf("Counts %ld\t",returnCounts);

		current = 0.0;
		// grab several readings and average
		for (i=0;i<nSamples;i++){
			getUSB1208AnalogIn(K617,&measurement[i]);
			current+=measurement[i];
		}

		current=current/(float)nSamples;

		printf("Current %f\t",current);

		fprintf(fp,"%ld\t%Lf\t",returnCounts/dwell,sqrtl(returnCounts)/dwell);
		fprintf(fp,"%f\t%f\t",-current,stdDeviation(measurement,nSamples));

		// Grab several readings and average
		pressure=0;
        getIonGauge(&pressure);
		printf("IG= %2.2E \n",pressure);
		fprintf(fp,"%2.4E\t%2.4E\n",pressure,0.);
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
		sprintf(buffer, "plot '%s.dat' using 7:abs(8):9 with yerrorbars\n", fileName);
		fprintf(gnuplot, "%s", buffer);

		// Set up the axis for the second plot x axis stays the same
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using 7:abs(10):11 with yerrorbars\n", fileName);
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
		sprintf(buffer, "plot '%s.dat' using 7:abs(8):9 with yerrorbars\n", fileName);
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
		sprintf(buffer, "plot '%s.dat' using 7:abs(10):11 with yerrorbars\n", fileName);
		fprintf(gnuplot, "%s",buffer);
	}
	pclose(gnuplot);

}

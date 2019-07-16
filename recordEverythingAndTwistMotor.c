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
#define WAITTIME 2

void graphData(char* fileName);

int main (int argc, char **argv)
{
	int i,k;
	long totalCounts;
	int nSamples;
	int dwell,magnitude;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[BUFSIZE],fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	float returnFloat;
	float current, pressure;
	long returnCounts;
	FILE *fp,*dataCollectionFlagFile;

	// Make sure the correct number of arguments were supplied. If not,
	// prompt the user with the proper form for input. 
	if (argc == 4){
		dwell= atoi(argv[1]);
		magnitude= atoi(argv[2]);
		strcpy(comments,argv[3]);
	} else{
		printf("    Usage:                                                                                     \n");
		printf("           sudo ./recordEverythingAndTwistMotor <time per measurement> <orderOfMagnitudeOfCurrent> <comments>\n");
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
	strftime(fileName,80,"/home/pi/RbData/%F/MonitorCounts%F_%H%M%S",timeinfo);

	sprintf(buffer,"%s.dat",fileName);
	printf("\n%s\n",buffer);

	fp=fopen(buffer,"w");
	if (!fp) {
		printf("Unable to open file: %s\n",buffer);
		exit(1);
	}

	fprintf(fp,"#Filename:\t%s\n",buffer);
	fprintf(fp,"#USB1208->HP3617Aconversion:\t%2.6f\n",HPCAL);
	fprintf(fp,"#DWELL:\t%d\n",dwell);
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

	int numPhotoDetectors = 3;
    int photoDetector[] = {BROWN_KEITHLEY,BOTTOM_KEITHLEY,TOP_KEITHLEY};
    char* names[]={"REF","HORIZ","VERT"};

	// Print the header for the information in the datafile
	fprintf(fp,"Measurement\tCount\tCountStDev\tCurrent\tCurrentStDev\tIonGauge\tIGStdDev\n");
    for(i=0;i<numPhotoDetectors;i++){
        fprintf(fp,"\t%s\t%ssd",names[i],names[i]);
    }
    fprintf(fp,"\n");
    //fclose(fp);

	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 16;
	float* measurement = malloc(nSamples*sizeof(float));
    float* involts = calloc(numPhotoDetectors,sizeof(float));
    float* stdDev = calloc(numPhotoDetectors,sizeof(float));

	int steps;
	int stepsPerRev=350;
	int stepSize=7;
	int motor=PUMP_MOTOR;

	totalCounts=0;
	for (steps=0;steps < stepsPerRev;steps+=stepSize){ // steps
		fprintf(fp,"%d\t",steps);
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

		// Record Pressure
		pressure=0;
		for (i=0;i<nSamples;i++){
			getIonGauge(&measurement[i]);
			pressure+=measurement[i];
		}
		pressure=pressure/(float)nSamples;
		printf("IG= %2.2E \n",pressure);
		fprintf(fp,"%2.4E\t%2.4E\t",pressure,stdDeviation(measurement,nSamples));

		// Record photodiode signals
		for(k=0;k<numPhotoDetectors;k++){ // numPhotoDet1
			involts[k]=0.0;	
			for (i=0;i<nSamples;i++){ // nSamples
					getUSB1208AnalogIn(photoDetector[k],&measurement[i]);
					involts[k]=involts[k]+fabs(measurement[i]);
					delay(WAITTIME);
			} // nSamples
			involts[k]=involts[k]/(float)nSamples; 
			stdDev[k]=stdDeviation(measurement,nSamples);
		} // numPhotoDet1

		for(k=0;k<numPhotoDetectors;k++){
			if(k!=numPhotoDetectors-1)
				fprintf(fp,"%f\t%f\t",involts[k],stdDev[k]);
			else
				fprintf(fp,"%f\t%f\n",involts[k],stdDev[k]);
		}
		stepMotor(motor,CLK,stepSize);
	}
	closeUSB1208();

	free(measurement);
	free(involts);
	free(stdDev);
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
		fprintf(gnuplot, "set xlabel 'Measurement Number'\n");			
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using 1:2:3 with yerrorbars\n", fileName);
		fprintf(gnuplot, "%s", buffer);

		// Set up the axis for the second plot x axis stays the same
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using 1:4:5 with yerrorbars\n", fileName);
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
		sprintf(buffer, "plot '%s.dat' using 1:2:3 with yerrorbars\n", fileName);
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
		sprintf(buffer, "plot '%s.dat' using 1:4:5 with yerrorbars\n", fileName);
		fprintf(gnuplot, "%s",buffer);
	}
	pclose(gnuplot);

}

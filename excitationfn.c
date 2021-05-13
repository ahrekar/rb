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
#include "interfacing/Sorensen120.h"
#include "mathTools.h"
#define BUFSIZE 1024

void graphData(char* fileName);
void collectAndRecordData(char* fileName, int steprange, int stepsize, float bias, float N2Offset, float oneD, float twoA, float startValue, int dwell);
void writeFileHeader(char* fileName, char* comments, 
                    float bias, float oneD, float twoA, float N2Sweep, 
                    int dwell, int magnitude);

int main (int argc, char **argv)
{
    float bias, N2Offset, oneD, twoA, startValue;
	int stepsize, scanrange;
	int minstepsize,maxstepsize;
	int dwell,magnitude;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[BUFSIZE],filePath[BUFSIZE],fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	FILE *fp,*dataCollectionFlagFile;

	// Make sure the correct number of arguments were supplied. If not,
	// prompt the user with the proper form for input. 
    int expectedArguments=11;
	if (argc == expectedArguments){
		bias = atof(argv[1]);
		N2Offset = atof(argv[2]);
        oneD = atof(argv[3]);
        twoA = atof(argv[4]);
		startValue = atof(argv[5]);
		scanrange =atoi(argv[6]);
		stepsize = atoi(argv[7]);
		dwell= atoi(argv[8]);
		magnitude= atoi(argv[9]);
		strcpy(comments,argv[10]);
	} else{
		printf("Hey, you made an error in\n");
        printf("your input, please examine\n");
		printf("the following usage to fix your error.\n");
		printf("\n");
        printf("You supplied %d arguments, %d were expected\n", argc-1,expectedArguments-1);
		printf("\n");
		printf("Usage:\n");
		printf("  sudo ./excitationfn <filament bias> (remember neg.)\n");
        printf("                      <N2 Offset>\n");
        printf("                      <V 1D>\n");
        printf("                      <V 2A>\n");
        printf("                      <He Offset> (remember neg.!)\n");
        printf("                      <scan range> ( 0-63 )\n");
        printf("                      <step size> ( 1-24 )\n");
        printf("                      <dwell time> ( 1-5 s ) \n");
        printf("                      <Order of magnitude of current>\n");
        printf("                      <comments>\n");
		printf("\n");
		printf("Step sizes:\n");
		printf("  1: %1.2fV    9: %1.2fV   17: %1.2fV\n",1*HPCAL,9*HPCAL,17*HPCAL );
		printf("  2: %1.2fV   10: %1.2fV   18: %1.2fV\n",2*HPCAL,10*HPCAL,18*HPCAL);
		printf("  3: %1.2fV   11: %1.2fV   19: %1.2fV\n",3*HPCAL,11*HPCAL,19*HPCAL);
		printf("  4: %1.2fV   12: %1.2fV   20: %1.2fV\n",4*HPCAL,12*HPCAL,20*HPCAL);
		printf("  5: %1.2fV   13: %1.2fV   21: %1.2fV\n",5*HPCAL,13*HPCAL,21*HPCAL);
		printf("  6: %1.2fV   14: %1.2fV   22: %1.2fV\n",6*HPCAL,14*HPCAL,22*HPCAL);
		printf("  7: %1.2fV   15: %1.2fV   23: %1.2fV\n",7*HPCAL,15*HPCAL,23*HPCAL);
		printf("  8: %1.2fV   16: %1.2fV   24: %1.2fV\n",8*HPCAL,16*HPCAL,24*HPCAL);
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
	strftime(filePath,80,"/home/pi/RbData/%F",timeinfo); //INCLUDE
	if (stat(filePath, &st) == -1){
		mkdir(filePath,S_IRWXU | S_IRWXG | S_IRWXO );
	}
    strftime(fileName,80,"EX%F_%H%M%S",timeinfo);

	sprintf(buffer,"%s.dat",fileName);
	printf("\n%s\n",buffer);
	sprintf(buffer,"%s/%s.dat",filePath,fileName);

	fp=fopen(buffer,"w");
	if (!fp) {
		printf("Unable to open file: %s\n",buffer);
		exit(1);
	}

	fprintf(fp,"#File:\t%s\n",buffer);
	fprintf(fp,"#Comments:\t%s\n",comments);

	fprintf(fp,"#USB1208->HP3617Aconversion:\t%2.6f\n",HPCAL);

    // Start Input Error Checking
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

    if (abs(startValue) > 120){
        printf("Error! Start value enetered was too large. Starting at -120 V instead\n");
        startValue=-120;
    }


    if (abs(startValue) + abs(scanrange) > 180)
    {
        printf("Error! Trying to reach too high of voltages.\n");
        startValue=-120;
        scanrange=60;
    }
    // End Input Error Checking

	fclose(fp);

	writeFileHeader(buffer, comments, bias, N2Offset, oneD, twoA, dwell, magnitude);

    collectAndRecordData(buffer, scanrange, stepsize, 
                        bias, N2Offset, oneD, twoA, startValue, dwell);

	setUSB1208AnalogOut(HETARGET,0);

	closeUSB1208();


	sprintf(buffer,"%s/%s",filePath,fileName);
	graphData(buffer);

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

void graphData(char* fileName){
	// Create graphs for data see gnutest.c for an explanation of 
	// how this process works.
	FILE *gnuplot;
	char buffer[BUFSIZE];
    int heColumn=6;
	gnuplot = popen("gnuplot","w"); 
    /* 
     * Here's the Column order:
     *  1. Aout
     *  2. V_fil
     *  3. V_N2
     *  4. V_sw
     *  5. V_he
     *  6. e_fil_Eng
     *  7. e_trg_Eng
     *  8. Count Rate
     *  9. Sqrt Count Rate
     *  10. I_f
     *  11. I_f Standard Deviation
     *  12. Ion Gauge Reading
     *  13. Ion Gauge Reading Std. Dev.
     */

	if (gnuplot != NULL){
		// First print to the terminal screen.

		// Set up the output for printing to terminal
		fprintf(gnuplot, "set terminal dumb size 54,24\n");
		fprintf(gnuplot, "set output\n");			
		fprintf(gnuplot, "set key autotitle columnheader\n");			

		// Set up the axis for the first plot
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "set xlabel 'He Potential'\n");			
		fprintf(gnuplot, "set ylabel 'Counts'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using %d:abs(9):10 with yerrorbars\n", fileName,heColumn);
		fprintf(gnuplot, "%s", buffer);

		// Set up the axis for the second plot x axis stays the same
		sprintf(buffer, "set title '%s'\n", fileName);
		fprintf(gnuplot, "%s",buffer);
		fprintf(gnuplot, "set ylabel 'Current'\n");			
		fprintf(gnuplot, "set yrange [0:*]\n");			

		// Print the plot to the screen
		sprintf(buffer, "plot '%s.dat' using %d:abs(11):12 with yerrorbars\n", fileName,heColumn);
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
		fprintf(gnuplot, "set ylabel 'Count Rate (Hz)'\n");			
		// Print the plot
		sprintf(buffer, "plot '%s.dat' using %d:abs(9):10 with yerrorbars\n", fileName, heColumn);
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
		sprintf(buffer, "plot '%s.dat' using %d:abs(11):12 with yerrorbars\n", fileName, heColumn);
		fprintf(gnuplot, "%s",buffer);
	}
	pclose(gnuplot);

}

void writeFileHeader(char* fileName, char* comments, 
                    float bias, float oneD, float twoA, float N2Sweep, 
                    int dwell, int magnitude){
    float returnFloat;
	FILE* fp;
	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	fprintf(fp,"#File:\t%s\n",fileName);
	fprintf(fp,"#Comments:\t%s\n",comments);

	fprintf(fp,"#V_fil:\t%.2f\n",bias);
	fprintf(fp,"#V_1D:\t%.2f\n",oneD);
	fprintf(fp,"#V_2A:\t%.2f\n",twoA);
	fprintf(fp,"#NumberOfSecondsPerCountMeasurement:\t%d\n",dwell);
	fprintf(fp,"#Comments:\t%s\n",comments);

    /** Record System Stats to File **/
    /** Pressure Gauges **/
	getIonGauge(&returnFloat);
	printf("IonGauge: %2.2E Torr\n",returnFloat);
	fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

	getConvectron(GP_TOP2,&returnFloat);
	printf("CVGauge(Source Foreline): %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(Source Foreline)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_TOP1,&returnFloat);
	printf("CVGauge(Target Foreline): %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(Target Foreline)(Torr):\t%2.2E\n", returnFloat);

    /** Temperature Controllers **/
	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#T_res:\t%f\n",returnFloat);
	printf("T_res:\t%.2f\t",returnFloat);
	getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#T_res_set:\t%f\n",returnFloat);

	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#T_trg:\t%f\n",returnFloat);
	printf("T_trg:\t%.2f\n",returnFloat);
	getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#T_trg_set:\t%f\n",returnFloat);

    /** End System Stats Recording **/
	fprintf(fp,"#MagnitudeOfCurrent(*10^-X):\t%d\n",magnitude);

	// Print the header for the information in the datafile
	fprintf(fp,"Aout\tV_fil\tV_N2\tV_1D\tV_2A\tV_he\te_fil_Eng\te_trg_Eng\tCountRate\tCountRateStDev\tI_f\tI_fStDev\tIonGauge\tIGStdDev\n");

	fclose(fp);
}


void collectAndRecordData(char* fileName, int scanrange, int stepsize, float bias, float N2Offset, float oneD, float twoA, float startValue, int dwell) {
	float primaryEnergy, secondaryEnergy;
	float current, pressure;
	float totalHeOffset;
	__u16 value;
	long returnCounts;
    int nSamples,i,k,hpCycles,hpEndRange,endStepRange,stepRange,stepRangeStart=0;
    int err,sorensenSet=startValue,endStepRangeStart;
	FILE* fp;

    err=resetGPIBBridge(GPIBBRIDGE1);
    delay(200);
    err=initSorensen120(SORENSEN120,GPIBBRIDGE1);

    hpEndRange=scanrange%60;
    hpCycles=scanrange/60+1;
    
	stepRange = 1023;
	endStepRange = 1+(int)(hpEndRange/(HPCAL));
    printf("Step range:%d\n",stepRange);

    err = setSorensen120Volts(-sorensenSet,SORENSEN120,GPIBBRIDGE1);
    if(err!=0)
    {
        printf("Error setting Sorensen Code: %d, Trying again\n",err);
        err = setSorensen120Volts(-sorensenSet,SORENSEN120,GPIBBRIDGE1);
        if(err!=0)
        {
            printf("Error setting Sorensen Code: %d, You need to fix something\n",err);
        }
    }

	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}
	// Allocate some memory to store measurements for calculating
	// error bars.
	nSamples = 16;
	float* measurement = malloc(nSamples*sizeof(float));

    printf("aout  e_fil_Eng    e_trg_Eng   V_He    Counts   Current\n");
    // Now that I'm going to use the Sorensen supply in combination
    // with the HP supply, we'll need two for loops. One for setting
    // the Sorensen to each of the needed values, the second for
    // rastering the HP supply over the range. 
    for (k=0;k<hpCycles;k++){
        // START SET SORENSEN 
        setUSB1208AnalogOut(HETARGET,0);
        sorensenSet=startValue-k*60;
        if(abs(sorensenSet) > 120){
            endStepRangeStart=abs(sorensenSet)-120;
            sorensenSet=-120;
            endStepRange = 1+(int)((hpEndRange+endStepRangeStart)/(HPCAL));
            endStepRangeStart=1+(int)((endStepRangeStart)/(HPCAL));
            stepRangeStart=endStepRangeStart;
        }

        // The supply expects a positive value for the voltage,
        // and we are providing a negative value as input.
        printf("Setting Sorensen to %d\nt", sorensenSet);
        err = setSorensen120Volts(-sorensenSet,SORENSEN120,GPIBBRIDGE1);
        if(err!=0)
        {
            printf("Error setting Sorensen Code: %d, Trying again\n",err);
            err = setSorensen120Volts(-sorensenSet,SORENSEN120,GPIBBRIDGE1);
            if(err!=0)
            {
                printf("Error setting Sorensen Code: %d, You need to fix something\n",err);
            }
        }
        // END SET SORENSEN
        if (k==hpCycles-1 && hpEndRange > 0) // If it's the last cycle.
        {
            stepRange=endStepRange;
        }
        for (value=stepRangeStart;value<stepRange;value+=stepsize){
        //for (value=steprange;value>24;value-=stepsize){
            setUSB1208AnalogOut(HETARGET,value);
            printf("%04d  ",value);
            fprintf(fp,"%d\t",value);

            fprintf(fp,"%4.2f\t",bias);
            fprintf(fp,"%4.2f\t",N2Offset);
            fprintf(fp,"%4.2f\t",oneD);
            fprintf(fp,"%4.2f\t",twoA);
            fprintf(fp,"%4.2f\t",sorensenSet - HPCAL*(float)value);

            primaryEnergy = (sorensenSet - HPCAL*(float)value) - bias;
            printf("% 6.1f eV  ",primaryEnergy);
            fprintf(fp,"%4.4f\t",primaryEnergy);

            secondaryEnergy = (sorensenSet - HPCAL*(float)value) - (bias + N2Offset) ;
            printf("% 6.1f eV  ",secondaryEnergy);
            fprintf(fp,"%4.4f\t",secondaryEnergy);

            totalHeOffset=sorensenSet - HPCAL*(float)value;
            printf("% 6.1f eV  ",totalHeOffset);

            // delay to allow transients to settle
            delay(500);

            getUSB1208Counter(dwell*10,&returnCounts);
            printf("%06ld  ",returnCounts);

            current = 0.0;
            // grab several readings and average
            for (i=0;i<nSamples;i++){
                getUSB1208AnalogIn(K617,&measurement[i]);
                current+=measurement[i];
            }

            current=current/(float)nSamples;

            printf("%+01.2e\n",current);

            fprintf(fp,"%ld\t%Lf\t",returnCounts/dwell,sqrtl(returnCounts)/dwell);
            fprintf(fp,"%e\t%f\t",-current,/*0*/stdDeviation(measurement,nSamples));

            // Grab several readings and average
            pressure=0;
            getIonGauge(&pressure);
            //printf("IG= %2.2E \n",pressure);
            fprintf(fp,"%2.4E\t%2.4E\n",pressure,0.);
        }
    }
    setUSB1208AnalogOut(HETARGET,0);

    err = setSorensen120Volts(0,SORENSEN120,GPIBBRIDGE1);
    if(err!=0)
    {
        printf("Error setting Sorensen Code: %d, Trying again\n",err);
        err = setSorensen120Volts(0,SORENSEN120,GPIBBRIDGE1);
        if(err!=0)
        {
            printf("Error setting Sorensen Code: %d, You need to fix something\n",err);
        }
    }
	free(measurement);
	fclose(fp);
}

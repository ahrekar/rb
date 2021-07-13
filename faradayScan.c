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
#include "mathTools.h" //includes stdDeviation
#include "interfacing/interfacing.h"
#include "interfacing/sacherLaser.h"
#include "faradayScanAnalysisTools.h"
#include "interfacing/waveMeter.h"
#include "probeLaserControl.h"

#define PI 3.14159265358979
#define NUMSTEPS 350
#define STEPSIZE 7
#define STEPSPERREV 350.0
#define DATAPTSPERREV STEPSPERREV/STEPSIZE
#define WAITTIME 2

#define BUFSIZE 1024

int plotData(char* fileName);
int recordNumberDensity(char* fileName);
void collectDiscreteFourierData(FILE* fp, int* photoDetector, int numPhotoDetectors,int motor, int revolutions);

int main (int argc, char **argv)
{
    float value, returnFloat, wavelength;
    int revolutions;

    /* Variables for generating a file name with date and time */
	time_t rawtime;
	struct tm * timeinfo;

	char filePath[BUFSIZE],fileName[BUFSIZE], comments[BUFSIZE],buffer[BUFSIZE];
	char dailyFileName[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 

	FILE *fp,*dataCollectionFlagFile,*configFile;


	if (argc==2){
		strcpy(comments,argv[1]);
	} else { 
		printf("usage '~$ sudo ./faradayScan <comments in quotes>'\n");
		return 1;
	}

	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file:\t%s\n",dataCollectionFileName);
		exit(1);
	}

    revolutions=1;

	// Set up interfacing devices
	initializeBoard();
	initializeUSB1208();
//	i = resetGPIBBridge(GPIBBRIDGE1);
//	i = resetGPIBBridge(GPIBBRIDGE2);
//	if(i != 0) printf("ERROR RESETTING GPIB BRIDGE\n");

	// Get file name.  Use format "FDayScan"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(filePath,BUFSIZE,"/home/pi/RbData/%F",timeinfo);
	if (stat(fileName, &st) == -1){
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}

	strftime(fileName,BUFSIZE,"FDayScan%F_%H%M%S",timeinfo);

	sprintf(buffer,"%s.dat",fileName);
	printf("\n%s\n",buffer);
	sprintf(fileName,"%s/%s",filePath,buffer);

	strftime(dailyFileName,BUFSIZE,"/home/pi/RbData/%F/FDayScan%F.dat",timeinfo); //INCLUDE

	printf("%s\n",comments);



	fp=fopen(fileName,"w");
	if (!fp) {
		printf("Unable to open file: %s\n",fileName);
        fflush(stdout);
		exit(1);
	}

    configFile=fopen("/home/pi/RbControl/system.cfg","r");
    if (!configFile) {
        printf("Unable to open config file\n");
        fflush(stdout);
        exit(1);
    }

	fprintf(fp,"#File:\t%s\n#Comments:\t%s\n",fileName,comments);

    /** Record System Stats to File **/
    /** Pressure Gauges **/
	getIonGauge(&returnFloat);
	printf("IonGauge %2.2E Torr \n",returnFloat);
	fprintf(fp,"#IonGauge(Torr):\t%2.2E\n",returnFloat);

	getConvectron(GP_TOP2,&returnFloat);
	printf("CVGauge(Source Foreline): %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(Source Foreline)(Torr):\t%2.2E\n", returnFloat);

	getConvectron(GP_TOP1,&returnFloat);
	printf("CVGauge(Target Foreline): %2.2E Torr\n", returnFloat);
	fprintf(fp,"#CVGauge(Target Foreline)(Torr):\t%2.2E\n", returnFloat);

    /** Temperature Controllers **/
	getPVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#T_res:\t%.2f\n",returnFloat);
	getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#T_res_set:\t%.2f\n",returnFloat);

	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#T_trg:\t%.2f\n",returnFloat);
	getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#T_trg_set:\t%.2f\n",returnFloat);

    /** End System Stats Recording **/

    char line[1024];
	fgets(line,1024,configFile);
	while(line[0]=='#'){
		fprintf(fp,"%s",line);
		fgets(line,1024,configFile);
	}

	fclose(configFile);

    /* For a rigorous look at the structure 
	int numDet=18,j;
	float scanDet[]={-33,-30,-19,-18,-9,-5,-4.5,-4,-3.5,6,6.5,7,7.5,11,20,21,30,33};
    */
    

/*    
	int numDet=12,j;
	float scanDet[]={-45,-30,-15,-10,-5,-4,4,5,10,15,30,45};
    */
   
    /*
	int numDet=10,j;
	float scanDet[]={-29,-26,-19,-18,-9,11,20,21,30,33};
    */

    /*For the lower densities */
	int numDet=6,j;
	float scanDet[]={-29,-16,-10,11,16,30};
   
    /* For the higher densities 
	int numDet=4,j;
	float scanDet[]={-29,-16,16,33};
    */
    
   

	fprintf(fp,"#Revolutions:\t%d\n",revolutions);
	fprintf(fp,"#DataPointsPerRev:\t%f\n",DATAPTSPERREV);
	fprintf(fp,"#NumVolts:\t%d\n",numDet);
	fprintf(fp,"#StepSize:\t%d\n",STEPSIZE);


	printf("Homing motor...\n");
	homeMotor(PROBE_MOTOR);

    // WHEN THE PUMP LASER IS OFF, measure with ammeters.
    int numPd=3;
    int pd[] = {BOTTOM_KEITHLEY,TOP_KEITHLEY,BROWN_KEITHLEY};
	fprintf(fp,"STEP\tPUMP\tPUMPsd\tPROBE\tPROBEsd\tREF\tREFsd\n");// Write the header for the data to the file.
    // WHEN THE PUMP LASER IS OFF, measure with ammeters.
    
    // WHEN THE PUMP LASER IS ON, measure with lock-in.
    //int numPd=1;
    //int pd[] = {BOTLOCKIN};
	//fprintf(fp,"STEP\tPUMP\tPUMPsd\n");
    // WHEN THE PUMP LASER IS ON, measure with lock-in.

    fclose(fp);

	fp=fopen(fileName,"a");

	setProbeDetuning(scanDet[0]);
	delay(3000);


	for(j=0;j<numDet;j++){
		setProbeDetuning(scanDet[j]);
		value=getSacherTemperature();
		delay(1000); 

	    getProbeFrequency(&wavelength);
	   // wavelength=-1;

        fprintf(fp,"\n\n#VOLT:%3.1f(%.2f)\n",value,wavelength);
        printf("TEMP:%3.1f(%.2f) ",value,wavelength-LINECENTER);
        fflush(stdout);

        quickHomeMotor(PROBE_MOTOR);
        collectDiscreteFourierData(fp,pd,numPd /*numPhotoDet*/,PROBE_MOTOR,revolutions);
	}//end for Volt

	fclose(fp);

	setProbeDetuning(scanDet[0]);
	delay(3000);
    printf("\n");
    fflush(stdout);

	//printf("Processing Data...\n");
	//analyzeData(fileName, numDet, revolutions, dataPointsPerRevolution, FOI);

	char* extensionStart;
	extensionStart=strstr(fileName,".dat");
	strcpy(extensionStart,"RotationAnalysis.dat");

	//printf("Plotting Data...\n");
	//plotData(fileName);
	//printf("Calculating number density...\n");
	//calculateNumberDensity(fileName, 0, 0);
	//printf("Recording number density to file...\n");
	//recordNumberDensity(fileName);

	closeUSB1208();

	// Remove the file indicating that we are taking data.
	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

void collectDiscreteFourierData(FILE* fp, int* photoDetector, int numPhotoDetectors,int motor, int revolutions)
{
    int count=0;
    int steps,i,j,k;

    int nSamples=16;
	float* measurement = malloc(nSamples*sizeof(float));

    /*
	i=initializeK6485(K6485METERVERT,GPIBBRIDGE2);
	if(i != 0) printf("ERROR INITIALIZING K6485 VERT\n");
	i=initializeK6485(K6485METERHORIZ,GPIBBRIDGE2);
	if(i != 0) printf("ERROR INITIALIZING K6485 HORIZ\n");
    */

    
    float* involts = calloc(numPhotoDetectors,sizeof(float));
    float* stdDev = calloc(numPhotoDetectors,sizeof(float));

    for (k=0;k<revolutions;k++){ //revolutions
        for (steps=0;steps < NUMSTEPS;steps+=STEPSIZE){ // steps
            // (NUMSTEPS) in increments of STEPSIZE
            /* watching the o-scope, we find
             * that:
             *   L-IN RELAX     Sig. Settle time.
             *      30 ms    -> 300 ms
             *     100 ms    -> 500 ms
             */
            delay(500); 

            // ------
            // get samples and average
            // ------
            // ----- When measuring using the lock-in, use this piece of code.
            //for(j=0;j<numPhotoDetectors;j++){ // numPhotoDet1
            //    involts[j]=0.0;	
            //    for (i=0;i<nSamples;i++){ // nSamples
            //            getMCPAnalogIn(photoDetector[j],&measurement[i]);
            //            involts[j]=involts[j]+measurement[i];
            //            delay(WAITTIME);
            //    } // nSamples
            //    involts[j]=involts[j]/(float)nSamples; 
            //    stdDev[j]=stdDeviation(measurement,nSamples);
            //} // numPhotoDet1
            // ----- When measuring using the lock-in, use this piece of code.
     
			// ----- When measuring using the ammeter, use this piece of code.
            for(j=0;j<numPhotoDetectors;j++){ // numPhotoDet1
                involts[j]=0.0;	
                for (i=0;i<nSamples;i++){ // nSamples
                        getUSB1208AnalogIn(photoDetector[j],&measurement[i]);
                        involts[j]=involts[j]+measurement[i];
                        delay(WAITTIME);
                } // nSamples
                involts[j]=involts[j]/(float)nSamples; 
                stdDev[j]=stdDeviation(measurement,nSamples);
            } // numPhotoDet1
            /* THE GPIB WAY ************/
            //i = getReadingK6485(&involts[0], K6485METERVERT, GPIBBRIDGE2);
            //i = getReadingK6485(&involts[1], K6485METERHORIZ, GPIBBRIDGE2);
            //i = getReadingK485(&involts[2], K485METER, GPIBBRIDGE1);

            //printf("%02d  | ", timeCounter);

            //for(k=0;k<NUMCHANNELS;k++){
            //    fprintf(fp,"%+0.5e\t%+0.5e\t", involts[k], 0.);
            //    printf("%+0.5e   ", involts[k]);
            //    if(k<NUMCHANNELS) printf(" | ");
            //    delay(delayTime/NUMCHANNELS);
            //}
            /******* END GPIB Way */
			// ----- When measuring using the ammeter, use this piece of code.

            fprintf(fp,"%d\t",steps+NUMSTEPS*k);
            for(j=0;j<numPhotoDetectors;j++){
                if(j!=numPhotoDetectors-1)
                    fprintf(fp,"%f\t%f\t",involts[j],stdDev[j]);
                else
                    fprintf(fp,"%f\t%f\n",involts[j],stdDev[j]);
            }

            count++;
            stepMotor(motor,CLK,STEPSIZE);
        } // steps
    } // revolutions
    free(measurement);
    free(stdDev);
    free(involts);
}

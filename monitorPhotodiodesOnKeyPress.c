/*
 *
 * This program will be used to quickly record the amount of 
 * contamination present in the probe detector from the
 * pump laser. 
 *
 * 
*/

#include "interfacing/interfacing.h"
#include "mathTools.h"

#define NUMCHANNELS 4

void collectAndRecordData(char* fileName);
void writeFileHeader(char* fileName, char* comments);

int main (int argc, char **argv)
{
	time_t rawtime;
	struct tm * timeinfo;
	char fileName[BUFSIZE],comments[BUFSIZE];
	char dataCollectionFileName[] = "/home/pi/.takingData"; 

	FILE *dataCollectionFlagFile, *fp;

	if (argc==2) {
		strcpy(comments,argv[1]);
	} else {
		printf("Usage:\n$ sudo ./monitorPhotodiodes <comments>\n");
		return 0;
	}
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("Unable to open file: %s\n",dataCollectionFileName);
		exit(1);
	}

	initializeBoard();
	initializeUSB1208();

	// get file name. Use format "MonitorPhotodiodes"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F",timeinfo);
	if (stat(fileName, &st) == -1){ // Create the directory for the Day's data 
		mkdir(fileName,S_IRWXU | S_IRWXG | S_IRWXO );
	}
	strftime(fileName,BUFSIZE,"/home/pi/RbData/%F/MonitorPhotodiodes%F_%H%M%S.dat",timeinfo);

	printf("\n%s\n",fileName);

	writeFileHeader(fileName, comments);
	fp=fopen(fileName,"a");

	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}
	
	homeMotor(PROBE_MOTOR);

	collectAndRecordData(fileName);

	closeUSB1208();

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

void writeFileHeader(char* fileName, char* comments){
	FILE* fp;
	float returnFloat;
	fp=fopen(fileName,"w");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	fprintf(fp,"#Filename:\t%s\n",fileName);
	fprintf(fp,"#Comments:\t%s\n",comments);

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
	fprintf(fp,"#T_res:\t%f\n",returnFloat);
	getSVCN7500(CN_RESERVE,&returnFloat);
	fprintf(fp,"#T_res_set:\t%f\n",returnFloat);

	getPVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#T_trg:\t%f\n",returnFloat);
	getSVCN7500(CN_TARGET,&returnFloat);
	fprintf(fp,"#T_trg_set:\t%f\n",returnFloat);

    /** End System Stats Recording **/

	//fprintf(fp,"TEMP\tFREQ\tPUMP\tStdDev\tPROBE\tStdDev\tREF\tStdDev\n");
	fprintf(fp,"K617\tStdDev\tPUMP\tStdDev\tPROBE\tStdDev\tREF\tStdDev\n");
	fclose(fp);
}

void collectAndRecordData(char* fileName){
	FILE* fp;
	char c = 'r';
	int k=0,i;
    int timeCounter=0;
	int nSamples = 16; // The number of data points to collect
    int delayTime=1000; // Time in ms
	float involts[NUMCHANNELS];


	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	// Allocate some memory to store measurements for calculating
	// error bars.
	float* measurement = malloc(nSamples*sizeof(float));

    while(c !='q'){

        for(k=0;k<NUMCHANNELS;k++){
            involts[k]=0.0;	
        }
        if(timeCounter%15==0) printf("       Keithly    |       PUMP        |        PROBE      |        REFERENCE\n"); // Channels 0-3

        // grab several readings and average
        for(k=0;k<NUMCHANNELS;k++){
            for (i=0;i<nSamples;i++){
                getUSB1208AnalogIn(k,&measurement[i]);
                involts[k]=involts[k]+measurement[i];
                delay(delayTime/NUMCHANNELS/nSamples);
            }
            involts[k]=fabs(involts[k])/(float)nSamples;
            fprintf(fp,"%0.4f\t%0.4f\t",involts[k],stdDeviation(measurement,nSamples));
            printf("  %0.4f %0.4f  ",involts[k],stdDeviation(measurement,nSamples));
            if(k<NUMCHANNELS) printf(" | ");
        }
        fprintf(fp,"\n");
        printf("\n Press Return for next datapoint or enter 'q' to quit\n");
		c=getchar();
		timeCounter++;
    }

	fprintf(fp,"\n");
	fclose(fp);
	free(measurement);
}

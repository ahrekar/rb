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

#define NUMCHANNELS 3

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

	setUSB1208AnalogOut(PROBEOFFSET,512);//sets vout such that 0 v offset at the probe laser

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

    /** End System Stats Recording **/

	fprintf(fp,"Time\tPUMP\tStdDev\tPROBE\tStdDev\tREF\tStdDev\n");
	fclose(fp);
}

void collectAndRecordData(char* fileName){
	FILE* fp;
	int k=0,i;
    int timeCounter;
	int nSamples = 32; // The number of data points to collect
    int evaluationTime=10;
	float involts[NUMCHANNELS];

	fp=fopen(fileName,"a");
	if (!fp) {
		printf("unable to open file: %s\n",fileName);
		exit(1);
	}

	// Allocate some memory to store measurements for calculating
	// error bars.
	float* measurement = malloc(nSamples*sizeof(float));

    for (timeCounter=0;timeCounter < evaluationTime; timeCounter+=1){
        fprintf(fp,"%d\t",timeCounter);

        for(k=0;k<NUMCHANNELS;k++){
            involts[k]=0.0;	
        }
        if(timeCounter%15==0) printf("       PUMP       |        PROBE      |        REFERENCE\n");

        // grab several readings and average
        for(k=1;k<NUMCHANNELS+1;k++){
            for (i=0;i<nSamples;i++){
                getUSB1208AnalogIn(k,&measurement[i]);
                involts[k-1]=involts[k-1]+measurement[i];
            }
            involts[k-1]=fabs(involts[k-1])/(float)nSamples;
            fprintf(fp,"%0.4f\t%0.4f\t",involts[k-1],stdDeviation(measurement,nSamples));
            printf("  %0.4f %0.4f  ",involts[k-1],stdDeviation(measurement,nSamples));
            if(k<NUMCHANNELS) printf(" | ");
        }
        fprintf(fp,"\n");
        printf("\n");
        delay(1000);
    }

	fprintf(fp,"\n");
	fclose(fp);
	free(measurement);
}

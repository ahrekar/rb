/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "interfacing.h"

#define HOURSINDAY 24
#define MINUTESINHOUR 60
#define SECONDSINMINUTE 60
#define BUFSIZE 1024

int main (int argc, char* argv[]){
	char dataCollectionFlagName[] = "/home/pi/.takingData";
	if( access(dataCollectionFlagName, F_OK) != -1){
		// Someone is taking data, don't try to communicate with 
		// any of the instruments. 
		return 1;
	} else{
        int dwell=1;
        long returnCounts;
		float myTemp;
        int i, fileExists;
		char buffer[BUFSIZE];
		char systemStatsFile[]="/home/pi/recordStats.dat";
		float volts, amps;
        char echoData[128];
		FILE* fp;

		// We're okay to continue.
		// variables for recording time
		time_t currentTime;
		struct tm * timeinfo;
		int day,hour,min,sec;
		int totalMinutes;


		initializeBoard();
		initializeUSB1208();


		time(&currentTime);
		timeinfo=localtime(&currentTime);

		strftime(buffer,BUFSIZE,"%d",timeinfo);
		day=atoi(buffer);
		strftime(buffer,BUFSIZE,"%H",timeinfo);
		hour=atoi(buffer);
		strftime(buffer,BUFSIZE,"%M",timeinfo);
		min=atoi(buffer);
		strftime(buffer,BUFSIZE,"%S",timeinfo);
	    sec=atoi(buffer);

	    fileExists=access(systemStatsFile,F_OK);
		fp = fopen(systemStatsFile,"a");


		printf("_____TEMPERATURE______\n");
		i=getPVCN7500(CN_TARGET,&myTemp);
		printf("Trg. T= %.1f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);


		i=getPVCN7500(CN_RESERVE,&myTemp);
        if(i==0)
            printf("Res. T= %.1f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		printf("\n\n_____PRESSURE_____\n");

		getConvectron(GP_N2_CHAN,&myTemp);
		printf("N2CV: %2.2E\n",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		getIonGauge(&myTemp);
		printf("IonG: %2.2E\t",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		printf("\n\n_____PHOTODIODES_____\n");
		getUSB1208AnalogIn(REF_LASER,&myTemp);
		printf("RefCell: %.2f\t",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		getUSB1208AnalogIn(PROBE_LASER,&myTemp);
		printf("PrbLaser: %.2f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		getUSB1208AnalogIn(PUMP_LASER,&myTemp);
		printf("PumpLaser: %.2f\n",myTemp);
		fprintf(fp,"%.2f\n",myTemp);

		fclose(fp);

		closeUSB1208();
		return 0 ;
	}
}

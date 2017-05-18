/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "kenBoard.h"
#include "omegaCN7500.h"
#include "grandvillePhillips.h"
#include "USB1208.h"
#include "BK1696.h"

#define K617 0
#define PROBELASER 2
#define REFLASER 3
#define HOURSINDAY 24
#define MINUTESINHOUR 60
#define BUFSIZE 1024

int main (int argc, char* argv[]){
	char dataCollectionFlagName[] = "/home/pi/.takingData";
	if( access(dataCollectionFlagName, F_OK) != -1){
		// Someone is taking data, don't try to communicate with 
		// any of the instruments. 
		return 1;
	} else{
        int i;
		float myTemp;
		char buffer[BUFSIZE];
		float volts, amps;
		FILE* fp;

		// We're okay to continue.
		// variables for recording time
		time_t currentTime;
		struct tm * timeinfo;
		int day,hour,min;
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

		fp = fopen("/home/pi/recordStats.dat","a");

		strftime(buffer,BUFSIZE,"%d\t%H\t%M\t",timeinfo);
		fprintf(fp,"%s",buffer);

		totalMinutes=day*HOURSINDAY*MINUTESINHOUR+hour*MINUTESINHOUR+min;
		fprintf(fp,"%d\t",totalMinutes);

		i=getPVCN7500(CN_RESERVE,&myTemp);
		printf("Reservoir T= %.1f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);


		i=getPVCN7500(CN_TARGET,&myTemp);//i holds error information. if anyone cares.
		printf("Target T= %.1f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		getConvectron(GP_HE_CHAN,&myTemp);
		printf("Helium %2.2E\n",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		getConvectron(GP_N2_CHAN,&myTemp);
		printf("N2 %2.2E\n",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		getIonGauge(&myTemp);
		printf("IonGauge %2.2E\n",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		getUSB1208AnalogIn(K617,&myTemp);
		printf("Kiethly 617 %.2f\n",myTemp);//the is no way to read the scale, or order of magnitude. This
		// number is just the mantissa
		fprintf(fp,"%.2f\t",myTemp);

		getUSB1208AnalogIn(REFLASER,&myTemp);
		printf("ReferenceLaser: %.2f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		getUSB1208AnalogIn(PROBELASER,&myTemp);
		printf("Probe Laser%.2f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		getVoltsAmpsBK1696(9,&volts,&amps);
		printf("BK  volts %.2f\tamps %.2f\n",volts,amps);
		fprintf(fp,"%.2f\t%.2f\n",volts,amps);

		fclose(fp);

		closeUSB1208();
		return 0 ;
	}
}

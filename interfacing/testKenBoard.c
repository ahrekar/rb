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

		unsigned short numAdcChan=7;
		unsigned short chan;
		unsigned int adcInt;


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

		strftime(buffer,BUFSIZE,"%d\t%H\t%M\t%S\t",timeinfo);

		totalMinutes=day*HOURSINDAY*MINUTESINHOUR+hour*MINUTESINHOUR+min;
        //int totalSeconds;
		//totalSeconds=day*HOURSINDAY*MINUTESINHOUR*SECONDSINMINUTE+hour*MINUTESINHOUR*SECONDSINMINUTE+min*SECONDSINMINUTE+sec;

		printf("_____TEMPERATURE______\n");
		i=getPVCN7500(CN_TARGET,&myTemp);
		printf("Trg. T= %.1f\n",myTemp);


		i=getPVCN7500(CN_RESERVE,&myTemp);
        if(i==0)
            printf("Res. T= %.1f\n",myTemp);

		i=getPVCN7500(CN_CHAMWALL,&myTemp);
        if(i==0)
            printf("Chm. T= %.1f\n",myTemp);

		printf("\n\n_____PRESSURE_____\n");
		getConvectron(GP_HE_CHAN,&myTemp);
		printf("HeCV: %2.2E\n",myTemp);

		getConvectron(GP_N2_CHAN,&myTemp);
		printf("N2CV: %2.2E\n",myTemp);

		getConvectron(GP_CHAMB_CHAN,&myTemp);
		printf("MainChamber: %2.2E\n",myTemp);

		getIonGauge(&myTemp);
		printf("IonG: %2.2E\t",myTemp);

		printf("\n\n_____CURRENT_____\n");
        writeRS485to232Bridge("READ?",echoData,0xCA);
        myTemp = atof(echoData);
		//getUSB1208AnalogIn(K617,&myTemp);
		printf("Kiethly 617: %2.2e\n",myTemp);//the is no way to read the scale, or order of magnitude. This
		// number is just the mantissa
		
		
		printf("\n\n_____RASPI-GPIO_____\n");
		for(chan=0;chan<numAdcChan;chan++){
			getADC(chan,&adcInt);
			printf("Channel %d: %d V\n",chan,adcInt);
		}


		printf("\n\n_____COUNTS_____\n");
		getUSB1208Counter(dwell,&returnCounts);
		printf("Counts: %ld\n",returnCounts);

		printf("\n\n_____PHOTODIODES_____\n");
		getUSB1208AnalogIn(REF_LASER,&myTemp);
		printf("RefCell: %.2f\t",myTemp);

		getUSB1208AnalogIn(PROBE_LASER,&myTemp);
		printf("PrbLaser: %.2f\n",myTemp);

		getUSB1208AnalogIn(PUMP_LASER,&myTemp);
		printf("PumpLaser: %.2f\n",myTemp);

		fclose(fp);

		closeUSB1208();
		return 0 ;
	}
}

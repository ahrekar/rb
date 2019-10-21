/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "sacherLaser.h"
#include "grandvillePhillips.h"
#include "kenBoard.h"
#include "omegaCN7500.h"
#include "USB1208.h"

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
		int year,month,day,hour,min,sec;
		int totalMinutes;


		initializeBoard();
		initializeUSB1208();


		time(&currentTime);
		timeinfo=localtime(&currentTime);

		strftime(buffer,BUFSIZE,"%y",timeinfo);
		year=atoi(buffer);
		strftime(buffer,BUFSIZE,"%m",timeinfo);
		month=atoi(buffer);
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
        if(fileExists){
            fprintf(fp,"Year\tMonth\tDay\tHour\tMinute\tSecond\tTotalMinutes\tTrg. T\tRes. T\tHeCV\tN2CV\tMainChamber\tIonG\tKiethly 617\tCounts\tRefCell\tPRbLaser\tPumpLaser\n");
        }

		strftime(buffer,BUFSIZE,"%y\t%m\t%d\t%H\t%M\t%S\t",timeinfo);
		fprintf(fp,"%s",buffer);

		totalMinutes=(year-14)*525600+(month-1)*31*HOURSINDAY*MINUTESINHOUR+(day-1)*HOURSINDAY*MINUTESINHOUR+hour*MINUTESINHOUR+min;
		fprintf(fp,"%d\t",totalMinutes);
        //int totalSeconds;
		//totalSeconds=day*HOURSINDAY*MINUTESINHOUR*SECONDSINMINUTE+hour*MINUTESINHOUR*SECONDSINMINUTE+min*SECONDSINMINUTE+sec;
		//fprintf(fp,"%d\t",totalSeconds);

		printf("_____TEMPERATURE______\n");
		i=getPVCN7500(CN_TARGET,&myTemp);
		printf("Trg. T= %.1f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);


		i=getPVCN7500(CN_RESERVE,&myTemp);
        if(i==0)
        printf("Res. T= %.1f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		printf("\n\n_____PRESSURE_____\n");
		getConvectron(GP_HE_CHAN,&myTemp);
		printf("HeCV: %2.2E\n",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		getConvectron(GP_N2_CHAN,&myTemp);
		printf("N2CV: %2.2E\n",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		getConvectron(GP_CHAMB_CHAN,&myTemp);
		printf("MainChamber: %2.2E\n",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		getIonGauge(&myTemp);
		printf("IonG: %2.2E\t",myTemp);
		fprintf(fp,"%2.2E\t",myTemp);

		printf("\n\n_____CURRENT_____\n");
        //writeRS485to232Bridge("READ?",echoData,0xCA);
        //myTemp = atof(echoData);
		getUSB1208AnalogIn(K617,&myTemp);
		printf("Kiethly 617: %2.2e\n",myTemp);//the is no way to read the scale, or order of magnitude. This
		// number is just the mantissa
		fprintf(fp,"%.2f\t",myTemp);

		printf("\n\n_____COUNTS_____\n");
		getUSB1208Counter(dwell,&returnCounts);
		printf("Counts: %ld\n",returnCounts);
		fprintf(fp,"%.2ld\t",returnCounts);

		printf("\n\n_____PHOTODIODES_____\n");
		getUSB1208AnalogIn(BROWN_KEITHLEY,&myTemp);
		printf("RefCell: %.2f\t",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		getUSB1208AnalogIn(TOP_KEITHLEY,&myTemp);
		printf("PrbLaser: %.2f\n",myTemp);
		fprintf(fp,"%.2f\t",myTemp);

		getUSB1208AnalogIn(BOTTOM_KEITHLEY,&myTemp);
		printf("PumpLaser: %.2f\n",myTemp);
		fprintf(fp,"%.2f\n",myTemp);

        /**
		printf("\n\n_____POWERSUPPLIES_____\n");
        int bkChan = 8;
		initializeBK1696(bkChan);

		getVoltsAmpsBK1696(bkChan,&volts,&amps);
		printf("BK volts (filament): %.2f\tamps %.2f\n",volts,amps);
		fprintf(fp,"%.2f\t%.2f\t",volts,amps);

        bkChan=9;
		initializeBK1696(bkChan);

		getVoltsAmpsBK1696(bkChan,&volts,&amps);
		printf("BK volts (other): %.2f\tamps %.2f\n",volts,amps);
		fprintf(fp,"%.2f\t%.2f\n",volts,amps);
        **/


		fclose(fp);

		closeUSB1208();
		return 0 ;
	}
}

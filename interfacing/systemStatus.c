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
		// We're okay to continue, set up variables.
        int i;
        long returnCounts;
        int dwell=10;
		float myTemp;
		float volts, amps;
        char stringBuffer[BUFSIZE];

        // Variables for recording the time. 
        time_t rawtime;
        struct tm * timeinfo;

		initializeBoard();
		initializeUSB1208();

        time(&rawtime);
        timeinfo=localtime(&rawtime);
        strftime(stringBuffer,BUFSIZE,"Current Time: %F_%H%M%S\n\n",timeinfo);
        printf(stringBuffer);

        /* i holds error information */
		printf("_____TEMPERATURE______\n");
		i=getPVCN7500(CN_TARGET,&myTemp);
		printf("Trg. T= %.1f\n",myTemp);

		i=getPVCN7500(CN_RESERVE,&myTemp);
        if(i==0)
            printf("Res. T= %.1f\t",myTemp);

		printf("\n\n_____PRESSURE_____\n");
		getConvectron(GP_HE_CHAN,&myTemp);
		printf("HeCV: %2.2E\t",myTemp);

		getConvectron(GP_N2_CHAN,&myTemp);
		printf("N2CV: %2.2E\n",myTemp);

		getIonGauge(&myTemp);
		printf("IonG: %2.2E\n",myTemp);

		printf("\n\n_____CURRENT_____\n");
		getUSB1208AnalogIn(K617,&myTemp);
		printf("Kiethly 617: %.2f\n",myTemp);
        // There is no way to read the scale, or order of magnitude,
		// this number is just the mantissa

		printf("\n\n_____COUNTS_____\n");
		getUSB1208Counter(dwell,&returnCounts);
		printf("Counts: %ld\n",returnCounts);

		printf("\n\n_____PHOTODIODES_____\n");
		getUSB1208AnalogIn(REF_LASER,&myTemp);
		printf("RefCell: %.2f\t",myTemp);

		getUSB1208AnalogIn(PUMP_LASER,&myTemp);
		printf("PumpLaser: %.2f\t",myTemp);

		getUSB1208AnalogIn(PROBE_LASER,&myTemp);
		printf("PrbLaser: %.2f\n",myTemp);

		printf("\n\n_____POWERSUPPLIES_____\n");
		getVoltsAmpsBK1696(8,&volts,&amps);
		printf("BK volts (filament): %.2f\tamps %.2f\n",volts,amps);
		getVoltsAmpsBK1696(9,&volts,&amps);
		printf("BK volts (other): %.2f\tamps %.2f\n",volts,amps);


		closeUSB1208();
		return 0 ;
	}
}

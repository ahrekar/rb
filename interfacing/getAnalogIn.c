/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "interfacing.h"
#include "kenBoard.h"
#include "USB1208.h"

int main (int argc, char* argv[]){
	char dataCollectionFlagName[] = "/home/pi/.takingData";
	if( access(dataCollectionFlagName, F_OK) != -1){
		// Someone is taking data, don't try to communicate with 
		// any of the instruments. 
		return 1;
	} else{
		float myTemp=0;
        int nSamples=128;
        int i;
        float* measurement= calloc(nSamples,sizeof(float));

		initializeBoard();
		initializeUSB1208();

        for (i=0;i<nSamples;i++){
            getUSB1208AnalogIn(REF_LASER,&measurement[i]);
            myTemp=myTemp+measurement[i];
            delay(1);
        }
		myTemp=fabs(myTemp)/(float)nSamples;
		printf("%.4f\n",myTemp);

        myTemp=0;

        for (i=0;i<nSamples;i++){
            getUSB1208AnalogIn(PUMP_LASER,&measurement[i]);
            myTemp=myTemp+measurement[i];
            delay(1);
        }
		myTemp=fabs(myTemp)/(float)nSamples;
		printf("%.4f\n",myTemp);

		closeUSB1208();
		return 0 ;
	}
}

/*
   program to set the control temperature of the Omega CN7500

   usage $ sudo ./setOmega <float temperature>

   e.g  $sudo ./setOmega 45.5
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interfacing.h"

int main (int argc, char* argv[]){

	float returnRes, returnTarg, returnTargSet;
//	int changed =0;
	float temperatureStep=100;
	float modTemperature=10;


/*

if reservoir > 45 C, set both heaters to 20

if reservoir < 30 set target to 100

*/
initializeBoard();


getPVCN7500(CN_RESERVE,&returnRes);
getPVCN7500(CN_TARGET,&returnTarg);
getSVCN7500(CN_TARGET,&returnTargSet);
if (returnRes == 0  || returnRes > 120){ 
	delay(5000); // This pause is used in the case that the program is being run from crontab
				 // and there is another program that caused an error in the reading of the 
				 // variable.
	getPVCN7500(CN_RESERVE,&returnRes);
	getPVCN7500(CN_TARGET,&returnTarg);
}

modTemperature=(returnRes>40)?10:5;
if (returnRes>95)
	modTemperature=30;
else if (returnRes > 70)
	modTemperature=20;
else if (returnRes > 50)
	modTemperature=10;
else if (returnRes > 40)
	modTemperature=5;
else
	modTemperature=3;

if (returnRes < 36 && returnTarg < 80){
	setSVCN7500(CN_RESERVE, 0.0);
	setSVCN7500(CN_TARGET, 0.0);
} else {
	setSVCN7500(CN_RESERVE, 0.0);
	setSVCN7500(CN_TARGET, returnRes+modTemperature);
}

printf("temperature %f > %f, setting to %f and 20\n",returnRes,temperatureStep,temperatureStep+modTemperature);
//changed=1;
/**
while(!changed){
	if (returnRes > temperatureStep){
		modTemperature=(returnRes>35)?10:5;
		if (temperatureStep < 30 && returnTarg < 85){
			setSVCN7500(CN_RESERVE, 20.0);
			setSVCN7500(CN_TARGET, 85);
		} else {
			setSVCN7500(CN_RESERVE, 20.0);
			setSVCN7500(CN_TARGET, temperatureStep+modTemperature);
		}
		
		printf("temperature %f > %f, setting to %f and 20\n",returnRes,temperatureStep,temperatureStep+modTemperature);
		changed=1;
	}
	printf("%f\n",temperatureStep);
	temperatureStep-=5;
}
**/
/**
if (returnRes > 90.0){
	setSVCN7500(CN_RESERVE, 20.0);
	setSVCN7500(CN_TARGET, 110.0);
	printf("temperature %f > 90, setting to 110 and 20\n",returnRes);
} else if (returnRes > 80.0){
	setSVCN7500(CN_RESERVE, 20.0);
	setSVCN7500(CN_TARGET, 100.0);
	printf("temperature %f > 80, setting to 100 and 20 \n",returnRes);
} else if (returnRes > 70.0){
	setSVCN7500(CN_RESERVE, 20.0);
	setSVCN7500(CN_TARGET, 90.0);
	printf("temperature %f > 70, setting to 90 and 20 \n",returnRes);
} else if (returnRes > 60.0){
	setSVCN7500(CN_RESERVE, 20.0);
	setSVCN7500(CN_TARGET, 80.0);
	printf("temperature %f < 60, setting to 80 and 20 \n",returnRes);
} else if (returnRes > 50.0){
 	setSVCN7500(CN_RESERVE, 20.0);
	setSVCN7500(CN_TARGET, 70.0);
	printf("temperature %f > 50, setting to 70 and 20 \n",returnRes);
} else if (returnRes > 40.0){
 	setSVCN7500(CN_RESERVE, 20.0);
	setSVCN7500(CN_TARGET, 60.0);
	printf("temperature %f > 40, setting to 60 and 20 \n",returnRes);
} else if (returnRes > 30.0){
 	setSVCN7500(CN_RESERVE, 20.0);
	setSVCN7500(CN_TARGET, 50.0);
	printf("temperature %f > 30, setting to 50 and 20 \n",returnRes);
} else if (returnRes > 20.0){
 	setSVCN7500(CN_RESERVE, 20.0);
	setSVCN7500(CN_TARGET, 40.0);
	printf("temperature %f > 20, setting to 40 and 20 \n",returnRes);
}
**/

printf("Target Temp:%f\n",returnRes);

return 0 ;
}

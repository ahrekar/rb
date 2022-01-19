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
if (returnRes == 0  || returnRes > 170){ 
	getPVCN7500(CN_RESERVE,&returnRes);
	getPVCN7500(CN_TARGET,&returnTarg);
    getSVCN7500(CN_TARGET,&returnTargSet);
}

if (returnRes>95){
	modTemperature=40;
    printf("temperature %f > %f, setting to %f and 0\n",returnRes,95,returnRes+modTemperature);
}else if (returnRes > 70){
	modTemperature=30;
    printf("temperature %f > %f, setting to %f and 0\n",returnRes,70,returnRes+modTemperature);
}else if (returnRes > 50){
	modTemperature=20;
    printf("temperature %f > %f, setting to %f and 0\n",returnRes,50,returnRes+modTemperature);
}else if (returnRes > 40){
	modTemperature=5;
    printf("temperature %f > %f, setting to %f and 0\n",returnRes,40,returnRes+modTemperature);
}else{
	modTemperature=3;
    printf("temperature %f < %f, setting to %f and 0\n",returnRes,40,returnRes+modTemperature);
}

if (returnRes < 38 && returnTarg < 80){
	setSVCN7500(CN_RESERVE, 0.0);
	setSVCN7500(CN_TARGET, 0.0);
} else {
	setSVCN7500(CN_RESERVE, 0.0);
	setSVCN7500(CN_TARGET, returnRes+modTemperature);
}


printf("Target Temp:%f\n",returnRes);

return 0 ;
}

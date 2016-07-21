/*
   program to set the control temperature of the Omega CN7500

   usage $ sudo ./setOmega <float temperature>

   e.g  $sudo ./setOmega 45.5
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interfacing/interfacing.h"

int main (int argc, char* argv[]){

	float myTemp;
	float returnFloat;

	if(argc>1){
		myTemp=atof(argv[1]);
		getPVCN7500(CN_TARGET,&returnFloat);
		printf("Target Temp:%f\n",returnFloat);
		getPVCN7500(CN_RESERVE,&returnFloat);
		printf("Reservoir Temp:%f\n",returnFloat);

		setSVCN7500(CN_RESERVE, myTemp);
		setSVCN7500(CN_TARGET, myTemp+3.0);
	}else{
		printf("usage $ sudo ./setOmega <float temperature>\n Automatically sets RbTarget Reservoir to set temperature and Target +3.0°C higher\n");
	}
	return 0 ;
}

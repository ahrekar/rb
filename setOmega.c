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

	float tempTarg;
	float tempRes;
	float returnFloat;

	if(argc==2){
		tempRes=atof(argv[1]);
		getPVCN7500(CN_TARGET,&returnFloat);
		printf("Target Temp:%f\n",returnFloat);
		getPVCN7500(CN_RESERVE,&returnFloat);
		printf("Reservoir Temp:%f\n",returnFloat);

		setSVCN7500(CN_RESERVE, tempRes);
		setSVCN7500(CN_TARGET, tempRes+3.0);
	}else if(argc==3){
		tempTarg=atof(argv[1]);
		tempRes=atof(argv[2]);
		setSVCN7500(CN_TARGET, tempTarg);
		setSVCN7500(CN_RESERVE, tempRes);
	}else{
		printf("usages:\n");
		printf("   $ sudo ./setOmega <float temperature> --- Automatically sets RbTarget Reservoir to set temperature and Target +3.0°C higher\n");
		printf("   $ sudo ./setOmega <float targTemp> <float resTemp>\n");
	}
	return 0 ;
}

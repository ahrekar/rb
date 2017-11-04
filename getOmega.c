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
	
	initializeBoard();

	getPVCN7500(CN_TARGET,&returnFloat);
	printf("Target Temp:%f\n",returnFloat);
	getPVCN7500(CN_RESERVE,&returnFloat);
	printf("Reservoir Temp:%f\n",returnFloat);

	closeBoard();
	return 0 ;
}

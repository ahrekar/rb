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

	float returnFloat,setReturnFloat;
	
	initializeBoard();

	getSVCN7500(CN_TARGET,&setReturnFloat);
	getPVCN7500(CN_TARGET,&returnFloat);
	printf("CollCellTemp(degC):%3.1f/%3.1f\n",returnFloat,setReturnFloat);
	getSVCN7500(CN_RESERVE,&setReturnFloat);
	getPVCN7500(CN_RESERVE,&returnFloat);
	printf("ResTemp(degC):%3.1f/%3.1f\n",returnFloat,setReturnFloat);

	return 0;
}

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

	float returnFloat;
	
	initializeBoard();

	getPVCN7500(CN_TARGET,&returnFloat);
	printf("CollCellTemp(degC):%3.1f\n",returnFloat);
	getPVCN7500(CN_RESERVE,&returnFloat);
	printf("ResTemp(degC):%3.1f\n",returnFloat);

	closeBoard();
	return 0 ;
}

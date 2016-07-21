
/*
Program to aid in the automated taking of data.
It waits for te omega temperature controller 
to read a certain temperature before it returns 
a value. This can be used in scripts to wait
until the cell has reached the desired temperature
before it begins taking data.

usage $ sudo ./setOmega <(float) target temperature>

e.g  $sudo ./setOmega 45.5


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interfacing/interfacing.h"

int main (int argc, char* argv[]){

	float myTemp;
	float actualTemp;


	if(argc>1){
		myTemp=atof(argv[1]);
		do{
			getPVCN7500(CN_TARGET,&actualTemp);
			printf("It is currently: %f\n", actualTemp);
			printf("Temperature has not been reached yet.\n");
			delay(5000);
		} while	(actualTemp < (myTemp - .1));
	}else{
		printf("usage $ sudo ./waitForOmega <target temperature>\n Program does not end until target temp. has been reached\n");
	}

	return 0 ;
}

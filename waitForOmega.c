
/*


program to set the control temperature of the Omega CN7500

usage $ sudo ./setOmega <float temperature>

e.g  $sudo ./setOmega 45.5


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tempControl.h"


int main (int argc, char* argv[]){


int i,j,chan;
float myTemp;
float actualTemp;
unsigned int returndata;


if(argc>1){
	myTemp=atof(argv[1]);
	do{
		actualTemp = getTemperature(5);
		printf("It is currently: %f\n", actualTemp);
		printf("Temperature has not been reached yet.\n");
		delay(5000);
	} while	(getTemperature(5) < (myTemp - .1));
}else{
printf("usage $ sudo ./waitForOmega <target temperature>\n Program does not end until target temp. has been reached\n");
}


 return 0 ;
}

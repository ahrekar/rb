
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
unsigned int returndata;


if(argc>1){
		myTemp=atof(argv[1]);

	setTargetTemperature(3,myTemp+3.0);
	setTargetTemperature(5,myTemp);

}else{
printf("usage $ sudo ./setOmega <float temperature>\n Automatically sets RbTarget Reservoir to set temperature and Target +3.0°C higher\n");
}


 return 0 ;
}

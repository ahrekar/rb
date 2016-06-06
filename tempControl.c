#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rs485.h"
#include "tempControl.h"

// These are register definitions specific to the Omega CN7800
#define PV_REG 0x1000
#define SV_REG 0x1001

float getTemperature(int channel);
float getTargetTemperature(int channel);
float setTargetTemperature(int channel);

/** Get the current temperature as read by the Omega controller.
 */
float getTemperature(int channel){
	unsigned int returnData;
	int exitStatus;

	initialize_rs485(9600,25);

	exitStatus=read_Modbus_RTU(channel,PV_REG,&returndata); //register4096 is PV

	if(exitStatus==0){ // If no errors, return temperature.
		return (float)returndata/10.0;
	}else{
		printf("Reading PV Error code %d\n",exitStatus);
		return -1;
	}
}

/** Get the temperature that the omega is attempting to 
 * reach.
 */
float getTargetTemperature(int channel){
	unsigned int returnData;
	int exitStatus;

	exitStatus=read_Modbus_RTU(channel,SV_REG,&returndata); //register4096 is SV

	if(exitStatus==0){ // If no errors, return temp.
		return (float)returnData/10.0;
	}else{ // Otherwise print helpful debugging info.
		printf("reading SV Error code %d\n",returnValExitStatus);
		return -1;
	}
}

/** Set the temperature that you would like the omega 
 * controller to maintain.
 */
float setTargetTemperature(int channel, float targetTemp){
	int exitStatus;

	exitStatus=write_Modbus_RTU(channel,SV_REG, (unsigned int) (targetTemp*10));

	return exitStatus;
}

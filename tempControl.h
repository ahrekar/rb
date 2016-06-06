#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// These are register definitions specific to the Omega CN7800
#define PV_REG 0x1000
#define SV_REG 0x1001

// Takes as input the channel to look for on RS485 communication
// outputs a float which represents the temperature. 
float getTemperature(int channel);
// Takes as input the channel to look for on RS485 communication
// outputs a float which represents the target temperature. 
float getTargetTemperature(int channel);
// Inputs: 	Channel to read from (RS 485 communication)
// 			Temperature set point 	
// Ouputs: 	Float which represents whether the set was successful,
// 			0 if successful
// 			error number otherwise
int setTargetTemperature(int channel, float targetTemp);

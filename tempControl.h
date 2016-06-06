#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rs485.h"

// These are register definitions specific to the Omega CN7800
#define PV_REG 0x1000
#define SV_REG 0x1001

float getTemperature(int channel);
float getTargetTemperature(int channel);
float setTargetTemperature(int channel);

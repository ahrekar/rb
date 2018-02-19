/*
   notes and comments 
   useful information
   to follow

*/

#include <stdio.h>
#include <stdlib.h>
#include "kenBoard.h"

#define BRIDGE 0xC0

int initializeLaser(void);
int setLaserStatus(unsigned short status);
float getLaserTemperature(void);
int setLaserTemperature(float temperature);
// chan is the rs485 channel

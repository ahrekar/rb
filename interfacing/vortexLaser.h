/*
   notes and comments 
   useful information
   to follow

*/

#include <stdio.h>
#include <stdlib.h>
#include "kenBoard.h"

int initializeVortex(void);

int setVortexPiezo(float volts);
int getVortexPiezo(float* volts);
int setLaserOnOff(int state);

// chan is the rs485 channel

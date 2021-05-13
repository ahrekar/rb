/*
machine layer. assumes that analog outputs from grandville phillips 
are connected to ADC channels 


*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kenBoard.h"

#include "RS485Devices.h"

#define GP_TOP1 0 // The Alcatel foreline
#define GP_TOP2 1 // The Welch foreline
#define GP_CHAMB_CHAN 2

int getConvectron(unsigned int chan, float* returnvalue);
int getIonGauge(float* returnvalue);

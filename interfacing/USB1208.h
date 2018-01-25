/*


*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include "pmd.h"
#include "usb-1208LS.h"

#define K617 0
#define PUMP_LASER 1
#define PROBE_LASER 2
#define REF_LASER 3

#define PROBEOFFSET 0
#define HETARGET 1
#define HPCAL 0.0310068


int closeUSB1208();

int initializeUSB1208();

int getUSB1208AnalogIn(unsigned short chan, float* returnvalue);

int getUSB1208Counter(unsigned short dwell, long int * returncounts);
// dwell is time in tenth's of seconds.  So, if want to count for 1 sec, pass '10'
int setUSB1208AnalogOut(unsigned short chan, unsigned int outValue);
// out value ranges from 0 to 1024



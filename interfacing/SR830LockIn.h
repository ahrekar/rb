/*


 */

#include "RS485Devices.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define SR830VERT 0x06


int getReadingSR830(float* amps, char gpibaddress, unsigned short RS485Address);
int getStatusSR830(unsigned char *returndata, char gpibaddress, unsigned short RS485Address);

int initializeSR830(char gpibaddress, unsigned short RS485Address);
int setRangeSR830(char gpibaddress, unsigned short RS485Address, unsigned char range);


/*


 */

#include "RS485Devices.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define K617METER 0x08

int getReadingK617(float* amps,char gpibaddress, unsigned short RS485Address);
int getStatusK617(unsigned char *returndata,char gpibaddress, unsigned short RS485Address);

int initializeK617(char gpibaddress, unsigned short RS485Address);
int setRangeK617(char gpibaddress, unsigned short RS485Address, unsigned char range);



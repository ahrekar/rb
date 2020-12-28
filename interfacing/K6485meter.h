/*


 */

#include "RS485Devices.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define K6485METER 0x02


int getReadingK6485(float* amps,char gpibaddress, unsigned short RS485Address);
int getStatusK6485(unsigned char *returndata,char gpibaddress, unsigned short RS485Address);

int initializeK6485(char gpibaddress, unsigned short RS485Address);
int setRangeK6485(char gpibaddress, unsigned short RS485Address, unsigned char range);


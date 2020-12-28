/*


 */

#include "RS485Devices.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define K485METER 0x03


int getReadingK485(float* amps,char gpibaddress, unsigned short RS485Address);
int getStatusK485(unsigned char *returndata,char gpibaddress, unsigned short RS485Address);

int initializeK485(char gpibaddress, unsigned short RS485Address);
int setRangeK485(char gpibaddress, unsigned short RS485Address, unsigned char range);


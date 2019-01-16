
/*


 */
#include "RS485Devices.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SORENSEN120 0x0C


int getSorensen120Volts(float* volts,char gpibaddress, unsigned short RS485Address);
int getSorensen120Amps(float* amps,char gpibaddress, unsigned short RS485Address);
int setSorensen120Volts(float volts, char gpibaddress, unsigned short RS485Address);
int initSorensen120(char gpibaddress, unsigned short RS485Address);

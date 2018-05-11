/*

program to manually compose a Modbus RTU message to be sent over the RS485 bus

usage: $~ sudo ./manualRTC xx yy zz ...
where  xx, yy, zz are HEXIDECIMAL representations of each 8-bit byte, in order to be written to bus

example $~ sudo ./manual 05 0A FF

the CRC is automatically appended to the end of the byte array.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kenBoard.h"
#include <math.h>
// use the following for base register address for analog recorder devices
// #define BASEREGANLG 0x0D0D

unsigned int readRS485AnalogSlave(unsigned short RS485Chan, unsigned short AnalogChan, float* ADCcount, float* stdev);


unsigned int readRS485AnalogSlaveSimple(unsigned short RS485Chan, unsigned short AnalogChan, float* ADCcount);

unsigned int setRS485AnalogSlavePeriod(unsigned short RS485Chan, unsigned short ADCperiods);

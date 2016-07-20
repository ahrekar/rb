/*


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kenBoard.h"
#define BK1696ON 0
#define BK1696OFF 1

int initializeBK1696(int address);
int getVoltsAmpsBK1696(int address,float* volts, float* amps);
int setVoltsBK1696(int address, float volts);
int setAmpsBK1696(int address, float amps);
int setOutputBK1696(int address, int status);

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "wiringPi.h"

#define WAV 0xC8


float getWaveMeter(float *returnFloat);
float getFrequency(float *returnFloat);
float getDetuning(float *returnFloat);
float getWavelength(void);
float getProbeFrequency(float *returnFloat);
float getPumpFrequency(float *returnFloat);
float getProbeWavelength(void);
float getPumpWavelength(void);
float getProbeDetuning(float *returnFloat);
float getPumpDetuning(void);

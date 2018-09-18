#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "wiringPi.h"

#define WAV 0xC8


float getWaveMeter(void);
float getFrequency(void);
float getDetuning(void);
float getWavelength(void);
float getProbeFrequency(void);
float getPumpFrequency(void);
float getProbeWavelength(void);
float getPumpWavelength(void);
float getProbeDetuning(void);
float getPumpDetuning(void);

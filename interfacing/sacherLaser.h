/*
   notes and comments 
   useful information
   to follow

*/

#include <stdio.h>
#include <stdlib.h>
#include "kenBoard.h"

#define HEAD 0xC2
#define TA 0xC6

int initializeSacher(void);
int initializeSacherTA(void);
int setSacherStatus(unsigned short status);
float getSacherTemperature(void);
int setSacherTemperature(float temperature);
int setTACurrent(int current);
float getTACurrent(void);
int setSacherCurrent(float current);
float getSacherCurrent(void);
// chan is the rs485 channel

/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "kenBoard.h"
#include "omegaCN7500.h"
#include "grandvillePhillips.h"
#include "USB1208.h"
#include "BK1696.h"

#define K617 0
#define PROBELASER 2
#define REFLASER 3
#define BUFSIZE 1024
/**
//RS485 Stuff
	// Temp control
	int getPVCN7500(unsigned short chan, float* temperature); //3-> Target Cell, 5-> Resevoir
	int getSVCN7500(unsigned short chan, float* temperature);
	int setSVCN7500(unsigned short chan, float temperature);
	// End Temp Control
	
	// Power Supply Control
	int initializeBK1696(int address);
	int getVoltsAmpsBK1696(int address,float* volts, float* amps);
	int setVoltsBK1696(int address, float volts);
	int setAmpsBK1696(int address, float amps);
	int setOutputBK1696(int address, int status);
	// End Power Supply Control
//End RS485

//GrandvillePhillips Stuff
void getConvectron(unsigned int chan,float* pressure);//0-> Helium, 1-> N2
void getIonGauge(float* pressure);
//End GrandvillePhillips

void getUSB1208AnalogIn(int channel,float* intensity); // 0-> Keithly, 1-> Pump Laser, 2-> Probe Laser, 3-> Reference Laser
int closeUSB1208();
int initializeUSB1208();
int getUSB1208AnalogIn(unsigned short chan, float* returnvalue);
int getUSB1208Counter(unsigned short dwell, long int * returncounts); // Dwell is in 1/10th of second
int setUSB1208AnalogOut(unsigned short chan, unsigned int outValue);
**/

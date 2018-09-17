/*
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "kenBoard.h"
#include "keithley.h"
#include "RS485Devices.h"

float readKeithley(){
	char command[64];
	char echoData[128];
	float current;
	int i;

	strcpy(command,"READ?");
	i=strlen(command);
	i=writeRS485to232Bridge(command,echoData,KEITHLY6485);

	current=atof(echoData);
	return current;
}

int decreaseKeithleyRange(){
	char command[64];
	char echoData[128];
	int i;

	strcpy(command,"SYST:KEY 13");
	i=strlen(command);
	i=writeRS485to232Bridge(command,echoData,KEITHLY6485);

	return i;
}

int increaseKeithleyRange(){
	char command[64];
	char echoData[128];
	int i;

	strcpy(command,"SYST:KEY 11");
	i=strlen(command);
	i=writeRS485to232Bridge(command,echoData,KEITHLY6485);

	return i;
}

float getKeithley(){
	float measurement;
	float mantissa;
	int exponent;
	int tooHigh=0;
	int tooLow=0;

	measurement=readKeithley();	
	printf("%f\n",measurement);
	mantissa=frexp(measurement,&exponent);
	printf("BEFORE DECIMAL: %f\n",mantissa);
	printf("AFTER DECIMAL: %f\n",exponent);

	if((mantissa > 1)) tooHigh=1;
	if((mantissa < 1 && exponent < .2)) tooLow=1;

	while(tooLow || tooHigh){
		printf("Too High: %d, Too Low: %d\n",tooHigh,tooLow);
		if(tooLow){
			decreaseKeithleyRange();
		} else if(tooHigh){
			increaseKeithleyRange();
		} else {
			return measurement;
		}
		tooHigh=0;
		tooLow=0;
		measurement=readKeithley();
		mantissa=frexp(measurement,&exponent);
	printf("BEFORE DECIMAL: %f\n",mantissa);
	printf("AFTER DECIMAL: %f\n",exponent);
	if((mantissa > 1 && exponent > .4)) tooHigh=1;
	if((mantissa < 1 && exponent < .2)) tooLow=1;
	}


	return measurement;
}

// open and configure the serial port
int initializeKeithley(){
	char command[64];
	char echoData[128];
	
	int i;  

	strcpy(command,"*RST");
	//printf("Sending %s\n",command);
	i=strlen(command);
	
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);

	strcpy(command,"FORM:ELEM READ");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);

	//printf("Return: %s\n",echoData);

	strcpy(command,"SYST:ZCH ON");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);
	
	strcpy(command,"CURR:RANG 2e-9");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);

	strcpy(command,"INIT");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);

	strcpy(command,"SYST:ZCOR:ACQ");
	//printf("Sending %s\n",command);
	i=strlen(command);
	//printf("Return: %s\n",echoData);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);

	strcpy(command,"SYST:ZCOR OFF");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);

	strcpy(command,"SYST:ZCH OFF");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);

	strcpy(command,"CURR:RANG:AUTO ON");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);

	strcpy(command,"READ?");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);

	strcpy(command,"READ?");
	//printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	//printf("Return: %s\n",echoData);

// exit normally
    return 0;
}

#include <stdlib.h> //atof()
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "flipMirror.h"
#include "wiringPi.h"
#include "waveMeter.h"
#include "RS485Devices.h"

#define SPEEDOFLIGHT 299792458 // meters/sec
#define LINECENTER 377107.463  // GHz

float getProbeFrequency(float* returnFrequency){
    int mirrorPos;
    
    mirrorPos=getMirror();
    if (mirrorPos!=0) setMirror(0);
    return getFrequency(returnFrequency);
}

float getProbeDetuning(float* returnFrequency){
    int mirrorPos;
    float tmp;

    mirrorPos=getMirror();
    if (mirrorPos!=0) setMirror(0);
    return getDetuning(returnFrequency);
}

float getProbeWavelength(void){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=0) setMirror(0);
    return getWavelength();
}

float getPumpFrequency(float *returnFrequency){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    return getFrequency(returnFrequency);
}

float getPumpDetuning(void){
    int mirrorPos;
    float tmp;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    return getDetuning(&tmp);
}

float getPumpWavelength(void){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    return getWavelength();
}

float getWaveMeter(float* wavemeterReturn){
	unsigned char retData[32];
	unsigned char outData[32];
	int i, status;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
	writeRS485to232Bridge(outData,retData,WAV);
	temp=atof((char*) retData);
    *wavemeterReturn=temp;

	return temp;
}

float getDetuning(float* returnFloat){
	unsigned char retData[32];
	unsigned char outData[32];
	int i, status;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
	writeRS485to232Bridge(outData,retData,WAV);
	temp=atof((char*) retData);
    temp=SPEEDOFLIGHT/temp - LINECENTER;

    *returnFloat=temp;
	return temp;
}

float getFrequency(float *returnFrequency){
	unsigned char retData[32];
	unsigned char outData[32];
	int i, status;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
	writeRS485to232Bridge(outData,retData,WAV);
	*returnFrequency=atof((char*) retData);
    *returnFrequency=SPEEDOFLIGHT/ *returnFrequency;

	return *returnFrequency;
}

float getWavelength(void){
	unsigned char retData[32];
	unsigned char outData[32];
	int i, status;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
	writeRS485to232Bridge(outData,retData,WAV);
	temp=atof((char*) retData);

	return temp;
}

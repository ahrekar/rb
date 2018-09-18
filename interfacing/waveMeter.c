#include <stdlib.h>
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

float getProbeFrequency(void){
    int mirrorPos;

    mirrorPos=getMirror();
    if (mirrorPos!=0) setMirror(0);
    return getFrequency();
}

float getProbeDetuning(void){
    int mirrorPos;

    mirrorPos=getMirror();
    if (mirrorPos!=0) setMirror(0);
    return getDetuning();
}

float getProbeWavelength(void){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=0) setMirror(0);
    return getWavelength();
}

float getPumpFrequency(void){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    return getFrequency();
}

float getPumpDetuning(void){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    return getDetuning();
}

float getPumpWavelength(void){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    return getWavelength();
}

float getWaveMeter(void){
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

float getDetuning(void){
	unsigned char retData[32];
	unsigned char outData[32];
	int i, status;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
	writeRS485to232Bridge(outData,retData,WAV);
	temp=atof((char*) retData);
    temp=SPEEDOFLIGHT/temp - LINECENTER;

	return temp;
}

float getFrequency(void){
	unsigned char retData[32];
	unsigned char outData[32];
	int i, status;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
	writeRS485to232Bridge(outData,retData,WAV);
	temp=atof((char*) retData);
    temp=SPEEDOFLIGHT/temp;

	return temp;
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

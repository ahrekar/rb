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
    sleep(1);
    return getFrequency(returnFrequency);
}

float getProbeDetuning(float* returnFrequency){
    int mirrorPos;

    mirrorPos=getMirror();
    if (mirrorPos!=0) setMirror(0);
    sleep(1);
    return getDetuning(returnFrequency);
}

float getProbeWavelength(void){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=0) setMirror(0);
    sleep(1);
    return getWavelength();
}

float getPumpFrequency(float *returnFrequency){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    sleep(1);
    return getFrequency(returnFrequency);
}

float getPumpDetuning(void){
    int mirrorPos;
    float tmp;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    sleep(1);
    return getDetuning(&tmp);
}

float getPumpWavelength(void){
    int mirrorPos;
    mirrorPos=getMirror();
    if (mirrorPos!=8) setMirror(8);
    sleep(1);
    return getWavelength();
}

float getWaveMeter(float* wavemeterReturn){
	unsigned char retData[32];
	unsigned char outData[32];
	int status;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
    if(status != 0){printf("An error occured while reading bridge: %d\n",status);}
	writeRS485to232Bridge(outData,retData,WAV);
	temp=atof((char*) retData);
    *wavemeterReturn=temp;

	return temp;
}

float getDetuning(float* returnFloat){
	unsigned char retData[32];
	unsigned char outData[32];
	int status;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
    if(status != 0){printf("An error occured while reading bridge: %d\n",status);}
    do{
	writeRS485to232Bridge(outData,retData,WAV);
	temp=atof((char*) retData);
    temp=SPEEDOFLIGHT/temp - LINECENTER;
    }while( temp > 50 || temp < 50 );

    *returnFloat=temp;
	return temp;
}

float getFrequency(float *returnFrequency){
	unsigned char retData[32];
	unsigned char outData[32];
	int status;

	strcpy((char*) outData," ");


	status=setRS485BridgeReads(2,WAV);
    if(status != 0){printf("An error occured while reading bridge: %d\n",status);}
    do{
        writeRS485to232Bridge(outData,retData,WAV);
        *returnFrequency=atof((char*) retData);
        *returnFrequency=SPEEDOFLIGHT/ *returnFrequency;
    }while(*returnFrequency > 377157 || *returnFrequency < 377057 );

	return *returnFrequency;
}

float getWavelength(void){
	unsigned char retData[32];
	unsigned char outData[32];
	int status;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
    if(status != 0){printf("An error occured while reading bridge: %d\n",status);}
    do{
        writeRS485to232Bridge(outData,retData,WAV);
        temp=atof((char*) retData);
    }while(temp < 744 || temp > 755);

	return temp;
}

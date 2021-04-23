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

	temp=getWavelength();
    *wavemeterReturn=temp;

	return temp;
}

float getDetuning(float* returnFloat){
	unsigned char retData[32];
	unsigned char outData[32];
	int status;
    int attempts=0;
	float temp;

	strcpy((char*) outData," ");

	status=setRS485BridgeReads(2,WAV);
    if(status != 0){printf("An error occured while reading bridge: %d\n",status);}
    do{
        writeRS485to232Bridge(outData,retData,WAV);
        temp=atof((char*) retData);
        temp=(float)SPEEDOFLIGHT/temp - LINECENTER;
        //printf("Returned Detuning: %f\n",temp);
        attempts++;
    }while( (temp < -800 || temp > 800) && attempts < 5);
    if (attempts ==5){printf("Could not read wavemeter!");}

    *returnFloat=temp;
	return temp;
}

float getFrequency(float *returnFrequency){
	unsigned char retData[32];
	unsigned char outData[32];
    float wavelength;
	int status;

	strcpy((char*) outData," ");


	status=setRS485BridgeReads(2,WAV);
    if(status != 0){printf("An error occured while reading bridge: %d\n",status);}
    do{
        writeRS485to232Bridge(outData,retData,WAV);
        *returnFrequency=atof((char*) retData);
        *returnFrequency=SPEEDOFLIGHT/ *returnFrequency;
        //wavelength=getWavelength();
        //printf("Returned Frequency: %f\n",*returnFrequency);
    }while( *returnFrequency < 377057 || *returnFrequency > 378000 );


	return *returnFrequency;
}

float getWavelength(void){
	unsigned char retData[32];
	unsigned char outData[32];
	int status, tries=0;
	float temp;

	strcpy((char*) outData," ");

    for(tries=0;tries<5;tries++)
    {
        status=setRS485BridgeReads(2,WAV);
        writeRS485to232Bridge(outData,retData,WAV);
        temp=atof((char*) retData);
        //printf("Returned Wavelength: %f\n",temp);
        if(status != 0){
            printf("An error occured while reading bridge: %d\n",status);
            temp=-1;
        }
        else {tries=5;}
    }

	return temp;
}

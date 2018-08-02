#include "waveMeter.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "flipMirror.h"
#include "wiringPi.h"
#include "waveMeter.h"
#include "RS485Devices.h"

float getProbeFreq(void){
    setMirror(0);
    delay(300);
    return getWaveMeter();
}
float getPumpFreq(void){
    setMirror(8);
    delay(300);
    return getWaveMeter();
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

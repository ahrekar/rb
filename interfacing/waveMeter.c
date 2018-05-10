#include "waveMeter.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "flipMirror.h"
#include "wiringPi.h"
#include "waveMeter.h"

float getProbeFreq(void){
    printf("Getting Probe Freq.\n");
    setFlipMirror(0xA3,0);
    printf("Mirror Flipped.\n");
    return getWaveMeter();
}
float getPumpFreq(void){
    printf("Getting Pump Freq.\n");
    setFlipMirror(0xA3,8);
    printf("Set Flip Mirror Pump Freq.\n");
    return getWaveMeter();
}
float getWaveMeter(void){
	char retData[32];
	char outData[32];
	int j;
	float temp;

	strcpy(outData," ");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS232Bridge(outData,retData,WAV);
	temp=atof(retData);

	return temp;
}

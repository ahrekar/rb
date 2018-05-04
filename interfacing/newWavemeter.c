#include "wavemeter.h"

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

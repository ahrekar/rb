/*
   notes and comments 
   useful information
   to follow

*/

#include "sacherLaser.h"



int initializeLaser(void){

	char retData[32];
	char outData[32];
	int j;

	strcpy(outData,":SYST:E 0");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS232Bridge(outData,retData,BRIDGE);

	j=strcmp(retData,"O.K.");

	if (j!=0){
		printf(retData);
	}
//	strcpy(outData,":SYST:ACK 0");
//	j=strlen(outData);
//	outData[j]=13;
//	outData[j+1]=0;

//	writeRS232Bridge(outData,retData,BRIDGE);

	

	// expect j = 0 for no errors.  If there is an error, 
	return j;
}

int setLaserStatus(unsigned short status){
	return 0;
}


float getLaserTemperature(void){

	char retData[32];
	char outData[32];
	int j;
	float temp;

	strcpy(outData,":TEC:TEMP?");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS232Bridge(outData,retData,BRIDGE);
	temp=atof(retData);

	return temp;
}


int setLaserTemperature(float temperature){
	char retData[32];
	char outData[32];
	int j,temp;

	j=-1;

	if ((temperature > 27.0) & (temperature<29.0)){
		sprintf(retData,"%2.3f",temperature);

		strcpy(outData,":TEC:TEMP ");
		strcat(outData,retData);
		j=strlen(outData);
		outData[j]=13;
		outData[j+1]=0;

		writeRS232Bridge(outData,retData,BRIDGE);

		j=strcmp(retData,"O.K.");

		if (j!=0) printf(retData);
	}

	return j;
}


// chan is the rs485 channel

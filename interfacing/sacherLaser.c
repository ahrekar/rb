/*
   notes and comments 
   useful information
   to follow

*/

#include "sacherLaser.h"

int initializeTA(void){

	char retData[32];
	char outData[32];
	int j;

	strcpy(outData,":SYST:E 0");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS485to232Bridge(outData,retData,TA);

	j=strcmp(retData,"O.K.");

	if (j!=0){
		printf(retData);
	}
//	strcpy(outData,":SYST:ACK 0");
//	j=strlen(outData);
//	outData[j]=13;
//	outData[j+1]=0;

//	writeRS485to232Bridge(outData,retData,HEAD);

	

	// expect j = 0 for no errors.  If there is an error, 
	return j;
}


int initializeLaser(void){

	char retData[32];
	char outData[32];
	int j;

	strcpy(outData,":SYST:E 0");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS485to232Bridge(outData,retData,HEAD);

	j=strcmp(retData,"O.K.");

	if (j!=0){
		printf(retData);
	}
//	strcpy(outData,":SYST:ACK 0");
//	j=strlen(outData);
//	outData[j]=13;
//	outData[j+1]=0;

//	writeRS485to232Bridge(outData,retData,HEAD);
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

	writeRS485to232Bridge(outData,retData,HEAD);
	temp=atof(retData);

	return temp;
}

float getTACurrent(void){
	char retData[32];
	char outData[32];
	int j;
	float temp;

	j=-1;

	strcpy(outData,":L:CURR?");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS485to232Bridge(outData,retData,TA);
	temp=atof(retData);

	return temp;
}

int setTACurrent(int current){
	char retData[32];
	char outData[32];
	int j;

	j=-1;

	if ((current >= 0) & (current<=2500)){
		sprintf(retData,"%d",current);

		strcpy(outData,":L:CURR ");
		strcat(outData,retData);
		strcat(outData,"mA");
		j=strlen(outData);
		outData[j]=13;
		outData[j+1]=0;

		writeRS485to232Bridge(outData,retData,TA);

		j=strcmp(retData,"O.K.");

		if (j!=0) printf(retData);
	} else {
		printf("Error: Improper value input\n");
		return -1;
	}

	return j;
}

int setLaserTemperature(float temperature){
	char retData[32];
	char outData[32];
	int j;

	j=-1;

	if ((temperature > 27.0) & (temperature<29.0)){
		sprintf(retData,"%2.3f",temperature);

		strcpy(outData,":TEC:TEMP ");
		strcat(outData,retData);
		j=strlen(outData);
		outData[j]=13;
		outData[j+1]=0;

		writeRS485to232Bridge(outData,retData,HEAD);

		j=strcmp(retData,"O.K.");

		if (j!=0) printf(retData);
	}

	return j;
}


// chan is the rs485 channel

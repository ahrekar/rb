/*
   notes and comments 
   useful information
   to follow

*/

#include "sacherLaser.h"
#include "RS485Devices.h"

int initializeSacherTA(void){

	char retData[32];
	char outData[32];
	int j;

	strcpy(outData,":SYST:E 0");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS485to232Bridge(outData,retData,TA);

	j=strcmp(retData,"O.K.");

	if (j==0){ // There was an error.
		printf("Sacher init error\n");
	}
//	strcpy(outData,":SYST:ACK 0");
//	j=strlen(outData);
//	outData[j]=13;
//	outData[j+1]=0;

//	writeRS485to232Bridge(outData,retData,HEAD);

	

	// expect j = 0 for no errors.  If there is an error, 
	return j;
}


int initializeSacher(void){

	char retData[32];
	char outData[32];
	int j;

	strcpy(outData,":SYST:E 0");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS485to232Bridge(outData,retData,HEAD);

	j=strcmp(retData,"O.K.\n");

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

int setSacherStatus(unsigned short status){
	return 0;
}


float getSacherTemperature(void){

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

float getSacherCurrent(void){
	char retData[32];
	char outData[32];
	int j;
	float temp;

	j=-1;

	strcpy(outData,":L:CURR?");
	j=strlen(outData);
	outData[j]=13;
	outData[j+1]=0;

	writeRS485to232Bridge(outData,retData,HEAD);
	//printf("Sacher return: %s\n",retData);
	temp=atof(retData);

	return temp;
}

int setTACurrent(int current){
	char retData[32];
	char outData[32];
	int j;

	j=-1;

	if ((current >= 0) && (current<=2500)){
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

int setSacherTemperature(float temperature){
	char retData[32];
	char outData[32];
	int j;

	j=-1;

	//printf("Changing temperature to: %f\n",temperature);

	if ((temperature > 27.0) && (temperature<36.5)){
		sprintf(retData,"%2.3f",temperature);

		strcpy(outData,":TEC:TEMP ");
		strcat(outData,retData);
		j=strlen(outData);
		outData[j]=13;
		outData[j+1]=0;

		writeRS485to232Bridge(outData,retData,HEAD);

		j=strcmp(retData,"O.K.");

		//if (j!=0) printf(retData);
	}else{
		printf("Error: temperature outside of usable range\n");
	}

	return j;
}

int setSacherCurrent(float current){
	char retData[32];
	char outData[32];
	int j;

	j=-1;

	printf("current: %f\n",current);
	if ((current >= 0) && (current<=170)){
		sprintf(retData,"%f",current);
		//printf("current string: %s\n",retData);

		strcpy(outData,":L:CURR ");
		strcat(outData,retData);
		strcat(outData,"mA");
		//printf("String to send: %s\n",outData);
		j=strlen(outData);
		outData[j]=13;
		outData[j+1]=0;

		writeRS485to232Bridge(outData,retData,HEAD);

		j=strcmp(retData,"O.K.");

		if (j!=0) printf(retData);
	} else {
		printf("Error: Improper value input\n");
		return -1;
	}

	return j;
}


// chan is the rs485 channel

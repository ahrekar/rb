/*


 */
#include "K6485meter.h"


int getReadingK6485(float* amps, char gpibaddress, unsigned short RS485Address){
	//unsigned char chardata[64];
	char chardata[64];
	char cmdData[64];
	//char chardata[64];
	float tempA=0.0;
	int counter = 5;
	int i;

	int status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);

	if (status==0) {// no errors
		// format of K6485 return string.  See page 3-14 of the manual
		//  NDCA±1.2345E-9<CR><LF>
		// initialize function turns off prefix, so just atof the returned value
		tempA = atof(chardata);
		//printf("k6485returnstring %s\t",chardata);
		//printf("atof conversion: %f\n",tempA);
		}
	else{
		while(status !=0 && counter > 0){

			// Seeing if sending a *reset* command.
			// will fix the problem of the ammeter 
			// not communicating properly. 
			strcpy(cmdData,"G1R0X");
			i=strlen(cmdData);
			cmdData[i]=0x0D;
			cmdData[i+1]=0;
			status = sendGPIBData(cmdData, gpibaddress, RS485Address);

			status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);
			tempA = atof(chardata);
			counter = counter - 1;
		}
		status = chardata[0];
	}

	*amps = tempA;

	return status;
}

int getStatusK6485(unsigned char* returndata, char gpibaddress, unsigned short RS485Address){
	//unsigned char cmdData[16];
	char cmdData[16];
	//char cmdData[16];
	//unsigned int i;
	int i;

	strcpy(cmdData,"U0X");

	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;

	int status = sendGPIBData(cmdData, gpibaddress, RS485Address);

	delay(100);

	status = listenGPIBData(returndata, 0x0A, gpibaddress, RS485Address);

	return status;
}

/* Initializes the device with consistent settings 
 * for the functions that we will send to the device later
 */
int initializeK6485(char gpibaddress, unsigned short RS485Address){
	//unsigned char cmdData[16];
	char cmdData[16];
	//char cmdData[16];
	//unsigned int i;
	int i;

	// See below for the meaning of each two character code.
	// See Appendix D of 6485 manual for all codes.
	strcpy(cmdData,"G1R0X");
	/*
	 * G1 - Report reading without prefix.
	 * R0 - auto ranges
	 * X - execute the command
	 */
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	int status = sendGPIBData(cmdData, gpibaddress, RS485Address);

	return status;
}

int setRangeK6485(char gpibaddress, unsigned short RS485Address, unsigned char range){
	//unsigned char cmdData[16];
	char cmdData[16];
	//char cmdData[16];
	//unsigned int i;
	int i;

	/* Ranges as specified in the manual (pg. 238 of pdf)
	 * R0 = Auto
	 * R1 = 002 nA
	 * R2 = 020 nA
	 * R3 = 200 nA
	 * R4 = 002 uA
	 * R5 = 020 uA
	 * R6 = 200 uA
	 * R7 = 002 mA
	 * R8 = 020 mA
	 * R9 = Auto range off
	 */
	if (range>9) range = 9;

	sprintf(cmdData,"R%dX",range);

	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;

	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);
	return status;
}

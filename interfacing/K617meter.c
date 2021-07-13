/*


 */
#include "K617meter.h"

int getReadingK617(float* amps,char gpibaddress, unsigned short RS485Address){
	//unsigned char chardata[64];
	char chardata[64];
	float tempA=0.0;
	int counter = 5;
//	unsigned int i;
//	unsigned int len;

	int status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);

	if (status==0) {// no errors
		// format of K485 return string.  See page 3-14 of the manual
		//  NDCA±1.2345E-9<CR><LF>
		// initialize function turns off prefix, so just atof the returned value
//		len = strlen(chardata);
//		for (i=0;i<len;i++) chardata[i]=chardata[i+1];
		tempA = atof(chardata);
//		printf("k485returnstring %s\t",chardata);
//		printf("atof conversion: %f\n",tempA);
	}
	else{
		while(status !=0 && counter > 0){
			status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);
			tempA = atof(chardata);
			counter = counter - 1;
		}
		status = chardata[0];
	}


	*amps = tempA;

	return status;
}

int getStatusK617(unsigned char* returndata, char gpibaddress, unsigned short RS485Address){
	//unsigned char cmdData[16];
	char cmdData[16];
	//unsigned int i;
	int i;

	strcpy(cmdData,"U0X");

	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;

	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);

	delay(100);

	status = listenGPIBData(returndata, 0x0A, gpibaddress, RS485Address);

	return status;
}


/* Initializes the device with consistent settings 
 * for the functions that we will send to the device later
 */
int initializeK617(char gpibaddress, unsigned short RS485Address){
	//unsigned char cmdData[16];
	char cmdData[16];
	//unsigned int i;
	int i;
	strcpy(cmdData,"G1F1C0R0X"); // Sets default settings we use. See below
								 // for description of settings. 
	/* For full information, see keithley_617.pdf
	 * located in Apparatus/operationManuals/
	 * reference the table on pg. 3-19. 
	 *
	 * G1: reading reported without prefix. 
	 * F1: Set device to read AMPS.
	 * C0: Zero check off. 
	 * R0: set range to AUTO scale. 
	 * The rest of the power-on defaults are fine.
	 */
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;

	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);
	return status;
}

int setRangeK617(char gpibaddress, unsigned short RS485Address, unsigned char range){
	//unsigned char cmdData[16];
	char cmdData[16];
	//unsigned int i;
	int i;

	if (range>11) range = 11;

	sprintf(cmdData,"R%dX",range);

	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;

	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);
	return status;
}

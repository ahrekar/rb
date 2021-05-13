/*


 */
#include "SR830LockIn.h"


int getReadingSR830(float* amps, char gpibaddress, unsigned short RS485Address){
	//unsigned char chardata[64];
	char chardata[64];
	float tempA=0.0;
	int counter = 5;
	
	char cmdData[16];
	int i;

	// Gets an output from the SR830, there are 4 values to choose from.
	// X = 1
	// Y = 2
	// R = 3
	// theta = 4
	strcpy(cmdData,"OUTP? 3");
	i=strlen(cmdData);
	// ASCII 0x0A = Line feed (SR830 requires LF or EOI as terminator)
	// ASCII 0x0D = Carriage Return
	cmdData[i]=0x0A;
	cmdData[i+1]=0;

	int status = sendGPIBData(cmdData, gpibaddress, RS485Address);

	status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);

	if (status==0) {// no errors
		// format of SR830 return string.  See pg. 5-15 (pdf 99) of manual.
		//  ±1.2345<CR><LF>
		tempA = atof(chardata);
		printf("SR830returnstring %s\t",chardata);
		printf("atof conversion: %f\n",tempA);
		}
	else{
		while(status !=0 && counter > 0){
			delay(100);
			status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);
			tempA = atof(chardata);
			counter = counter - 1;
		}
		status = chardata[0];
	}

	*amps = tempA;

	return status;
}

/* Initializes the device with consistent settings 
 * for the functions that we will send to the device later
 */
int initializeSR830(char gpibaddress, unsigned short RS485Address){
	//unsigned char cmdData[16];
	char cmdData[16];
	//char cmdData[16];
	//unsigned int i;
	int i;

	// OUTX sets the interfaces with which the SR830 will respond.
	// 0 = RS232
	// 1 - GPIB
	strcpy(cmdData,"OUTX 1");
	i=strlen(cmdData);
	// ASCII 0x0A = Line feed (SR830 requires LF or EOI as terminator)
	// ASCII 0x0D = Carriage Return
	cmdData[i]=0x0A;
	cmdData[i+1]=0;
	int status = sendGPIBData(cmdData, gpibaddress, RS485Address);

	return status;
}

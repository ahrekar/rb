/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "kenBoard.h"
#include "RS485Devices.h"
#include "Sorensen120.h"


#define GPIBBRIDGE1 0XC9 // the gpib bridge can have many gpib devices attached to it, so will also need the GPIB address of each
// this is the GPIB addresses of each respective instrument attached to this bridge
#define SORENSEN120 0x0C




int main (int argc, char* argv[]){
	int i,k;
	float myTemp,myVolts;
	unsigned int periods;

	initializeBoard();


	printf("Sending IFC to GPIB instruments on bridge %02x\n",GPIBBRIDGE1);
	i=resetGPIBBridge(GPIBBRIDGE1);
	printf("Status %d\n",i);
	delay(200);
	i=initSorensen120(SORENSEN120,GPIBBRIDGE1);
	printf("Init Sorensen\nStatus %d\n",i);


	for (k=0; k<10; k++){
	myTemp = (float)k * 12.2;
		i = setSorensen120Volts(myTemp,SORENSEN120,GPIBBRIDGE1);
		printf("Setting Sorensen %.1f\t",myTemp);
		delay(200);
		i = getSorensen120Volts(&myVolts,SORENSEN120,GPIBBRIDGE1);
		i = getSorensen120Amps(&myTemp,SORENSEN120,GPIBBRIDGE1);
		printf("Measured: %.2fV\t %.3fA\t %.1fW\n",myVolts,myTemp,myVolts*myTemp);
		delay(200);
	}

	i = setSorensen120Volts(0.0,SORENSEN120,GPIBBRIDGE1);



	return 0 ;
}

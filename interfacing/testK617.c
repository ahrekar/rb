/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "kenBoard.h"
#include "RS485Devices.h"
//#include "omegaCN7500.h"
#include "K617meter.h"


#define GPIBBRIDGE1 0XC9 // the gpib bridge can have many gpib devices attached to it, so will also need the GPIB address of each
// this is the GPIB addresses of each respective instrument attached to this bridge
#define K617METER 0x08
#define SORENSEN120 0x0C


int main (int argc, char* argv[]){
	int i,k;
	float myTemp,myVolts;
	unsigned int periods;
	unsigned char myStatusData[64];

	initializeBoard();


	printf("Sending IFC to GPIB instruments on bridge %02x\n",GPIBBRIDGE1);
	i=resetGPIBBridge(GPIBBRIDGE1);
	printf("Status %d\n",i);
	delay(200);
	i=initializeK617(K617METER,GPIBBRIDGE1);
	printf("Init K617\nStatus %d\n",i);
	delay(500);

	i=setRangeK617(K617METER,GPIBBRIDGE1,8);
	printf("Set Range  K617 20uA\nStatus %d\n",i);
	delay(500);
	
	printf("getting reading....\n");
	fflush(stdout);
	i=getReadingK617(&myTemp, K617METER,GPIBBRIDGE1);
	if (!i) printf("K617 reading = %E\n",myTemp);else printf("Status %d\n",i);
	delay(400);

	i = getStatusK617(myStatusData, K617METER,GPIBBRIDGE1);
	if (!i) printf("K617 status = %s\n",myStatusData);else printf("Status %d\n",i);
	

return 0;
}

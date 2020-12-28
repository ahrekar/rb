/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "kenBoard.h"
#include "RS485Devices.h"
#include "K485meter.h"


#define GPIBBRIDGE1 0XC9 // the gpib bridge can have many gpib devices attached to it, so will also need the GPIB address of each
// this is the GPIB addresses of each respective instrument attached to this bridge
#define K485METER 0x03
#define SORENSEN120 0x0C


int main (int argc, char* argv[]){
	int i;
	float myTemp;
	unsigned char myStatusData[64];

	initializeBoard();


	printf("Sending IFC to GPIB instruments on bridge %02x\n",GPIBBRIDGE1);
	i=resetGPIBBridge(GPIBBRIDGE1);
	printf("Status %d\n",i);
	delay(200);
	i=initializeK485(K485METER,GPIBBRIDGE1);
	printf("Init K485\nStatus %d\n",i);
	delay(500);

	i=setRangeK485(K485METER,GPIBBRIDGE1,8);
	printf("Set Range  K485 20uA\nStatus %d\n",i);
	delay(500);
	
	printf("getting reading....\n");
	fflush(stdout);
	i = getReadingK485(&myTemp, K485METER, GPIBBRIDGE1);
	if (!i) printf("K485 reading = %E\n",myTemp);else printf("Status %d\n",i);
	delay(400);

	i = getStatusK485(myStatusData, K485METER, GPIBBRIDGE1);
	if (!i) printf("K485 status = %s\n",myStatusData);else printf("Status %d\n",i);
	

return 0;
}

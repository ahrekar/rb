/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "kenBoard.h"
#include "RS485Devices.h"
#include "K6485meter.h"

#define GPIBBRIDGE1 0XC9 // the gpib bridge can have many gpib devices attached to it, so will also need the GPIB address of each


int main (int argc, char* argv[]){
	int i;
	float myTemp;
	unsigned char myStatusData[64];

	initializeBoard();


	printf("Sending IFC to GPIB instruments on bridge %02x\n",GPIBBRIDGE1);
	i=resetGPIBBridge(GPIBBRIDGE1);
	printf("Status %d\n",i);
	delay(200);

	printf("Init K6485 @ %d\n",K6485METER);
	i=initializeK6485(K6485METER,GPIBBRIDGE1);
	printf("Status %d\n",i);
	delay(500);

	printf("Set Range  K6485 20uA\n");
	i=setRangeK6485(K6485METER,GPIBBRIDGE1,8);
	delay(500);
	printf("Status %d\n",i);
	
	printf("getting reading....\n");
	fflush(stdout);
	i = getReadingK6485(&myTemp, K6485METER, GPIBBRIDGE1);
	if (!i) printf("K6485 reading = %E\n",myTemp);else printf("Status %d\n",i);
	delay(400);

	i = getStatusK6485(myStatusData, K6485METER, GPIBBRIDGE1);
	if (!i) printf("K6485 status = %s\n",myStatusData);else printf("Status %d\n",i);
	

return 0;
}

/*
 */

#include <stdio.h>
#include <string.h>
#include "kenBoard.h"
#include "RS485Devices.h"



int main(int argc,char* argv[])
{

	unsigned char echoData[128];
	unsigned short chan;
	
// open and configure the serial port
	initializeBoard();

if (argc==2){
	
	chan=(unsigned short)strtol(argv[1],NULL,16);
	getRS485SlaveID(echoData,chan);
	printf("Return String for device %02x: %s\n",chan,echoData);

}


       printf("OK.\n");

// exit normally
    return 0;
}

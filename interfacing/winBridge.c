/*
 */

#include <stdio.h>
#include <string.h>
#include "kenBoard.h"
#include "RS485Devices.h"



int main(int argc,char* argv[])
{


	unsigned char command[64];
	unsigned char echoData[128];
	unsigned short chan,reads,timeout;
	int status;


// open and configure the serial port
	initializeBoard();


if (argc>2){
	strcpy((char*)command,argv[1]);
	chan=(unsigned short)strtol(argv[2],NULL,16);

	if  (argc>3) {

		reads = atoi(argv[3]);
		status=setRS485BridgeReads(reads,chan);

	}

	status=getRS485BridgeReads(&reads,chan);
	printf("Bridge at address %02X reads %d times before sending data back\n",chan,reads);
	printf("Status %d\n",status);

	status=getRS485BridgeTimeout(&timeout,chan);
	printf("Bridge at address %02X has a timeout value of %04X\n",chan,timeout);
	printf("Status %d\n",status);



	printf("Sending %s\n",command);
	writeRS485to232Bridge(command,echoData,chan);
	printf("Return: %s\n",echoData);

} else {

printf("usage:  sudo ./winBridge \"text to send\" <485addressHEX> [numreads] [timeoutvalue]\n");

}

// exit normally
    return 0;
}

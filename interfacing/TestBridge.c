/*



   simple function .  testing on an Epson Projector.


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kenBoard.h"
#include "RS485Devices.h"



int main (int argc, char* argv[]){
	int i,j;
	char command[32];
	char echoData[32];
	unsigned short chan;

	if (argc==3){

		initializeBoard();
		//	initialize_rs485(9600,6); // this is raspi specific. May need to change the gpio pin number
		//9600 is the default for most equipment
		strcpy(command,argv[1]);
		i=strlen(command);
		chan = (unsigned short)strtol(argv[2],NULL,16);
		printf("sending %s\n",command);
		writeRS232Bridge(command,echoData,chan);
		printf("Return %s\n",echoData);

	

		printf("\n");
	}
	return 0 ;
}

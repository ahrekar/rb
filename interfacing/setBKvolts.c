/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include "kenBoard.h"
#include "BK1696.h"

int main (int argc, char* argv[]){
	if (argc==3){
		int i;
		float myVolts,myAmps;
		int chan;

		initializeBoard();

		chan=atoi(argv[1]);
		i=initializeBK1696(chan);
		if(i==0){
			i=getVoltsAmpsBK1696(chan,&myVolts,&myAmps);
			printf("Volts %.2f\tAmps %.2f\n",myVolts,myAmps);

			myVolts=atof(argv[2]);
			// set maximum amps 
			setAmpsBK1696(chan,8.0);
			// set desired volts
			setVoltsBK1696(chan,myVolts);
			setOutputBK1696(chan,BK1696ON);
			delay(300);// allow power supply a chance to change

			i=getVoltsAmpsBK1696(chan,&myVolts,&myAmps);
			printf("Volts %.2f\tAmps %.2f\n",myVolts,myAmps);
		} else{
			printf("Error: initialize reported %d\n",i);
		}
		closeBoard();
	}else{
		printf("usage: ./setBKvolts <RS485_channel> <floatingpointvalue>\n");
	}

	return 0 ;
}

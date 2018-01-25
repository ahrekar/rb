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
		initializeBoard();

		int i;
		float myVolts,myAmps;
		int chan;
		chan = atoi(argv[1]);
		i=initializeBK1696(chan);
		if(i==0){
			i=getVoltsAmpsBK1696(chan,&myVolts,&myAmps);
			printf("Volts %.2f\tAmps %.2f\n",myVolts,myAmps);

			myAmps=atof(argv[2]);
			// set maximum volts
			setVoltsBK1696(chan,20.0);
			// set desired amps
			setAmpsBK1696(chan,myAmps);
			setOutputBK1696(chan,BK1696ON);
			delay(300);// allow power supply a chance to change

			i=getVoltsAmpsBK1696(chan,&myVolts,&myAmps);
			printf("Volts %.2f\tAmps %.2f\n",myVolts,myAmps);
		} else{
			printf("Error: initialize reported %d\n",i);
		}
		closeBoard();
	}else{
		printf("Usage ./setBKamps <RS485_channel> <amps to set>\n");
	}

	return 0 ;
}

/*

program to manually compose a Modbus RTU message to be sent over the RS485 bus

usage: $~ sudo ./manualRTC xx yy zz ...
where  xx, yy, zz are HEXIDECIMAL representations of each 8-bit byte, in order to be written to bus

example $~ sudo ./manual 05 0A FF

the CRC is automatically appended to the end of the byte array.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RS485Devices.h"
#include "kenBoard.h"

int main (int argc, char* argv[]){


unsigned short RS485Chan,AnalogChan,ADCperiods;
float ADCcount, volts, stdev;
int code,i;

initializeBoard();
//	initialize_rs485(9600,25);
	//9600 is the default for most equipment


if (argc>2){
	RS485Chan=(unsigned short) strtol(argv[1],NULL,16);
	ADCperiods = atoi(argv[2]); // number of 16mS periods to wait for next ADC reading to be stored in 10 position array

	for (i=0;i<4;i++){

		readRS485AnalogSlave(RS485Chan,i,5.0, &volts, &stdev);
		printf("Channel %d volts %f +/- %f\n",i,volts,stdev);


	}


	printf("\nSetting sample period for averaging function to once every %d mS into rolling 10 place buffer.\n",16*ADCperiods);
	setAnalogRecorderPeriod(RS485Chan,ADCperiods);


}else{

	printf("Usage ~sudo ./testAnalogSlave <AddressHEX> <ADCperiods>\n",code);
}

return 0;

}

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
#include "RS485AnalogSlave.h"
#include "kenBoard.h"

int main (int argc, char* argv[]){


unsigned short RS485Chan,AnalogChan,ADCperiods;
float ADCcount, stdev;
int code;

initializeBoard();
//	initialize_rs485(9600,25);
	//9600 is the default for most equipment


if (argc>3){
	RS485Chan=(unsigned short) strtol(argv[1],NULL,16);
	AnalogChan = atoi(argv[2]);
	ADCperiods = atoi(argv[3]); // number of 16mS periods to wait for next ADC reading to be stored in 10 position array



code = readRS485AnalogSlave(RS485Chan, AnalogChan, &ADCcount, &stdev);
if (code==0){
	printf("Average raw ADC over 10 measurements (0 to 1024) = %f ± %f\n",ADCcount,stdev);

} else {
	printf("process retunred erro code %04x \n",code);

}

code = readRS485AnalogSlaveSimple(RS485Chan, AnalogChan, &ADCcount);

printf("\n\nRead Single measurement\n");

if (code==0){
	printf("Volts = %f volts\n",ADCcount);

} else {
	printf("process retunred erroR code %04x \n",code);
}



code=setRS485AnalogSlavePeriod(RS485Chan, ADCperiods);

if (code==0){

printf("\nSetting sample period for averaging function to once every %d mS into rolling 10 place buffer.\n",16*ADCperiods);

}else{
	printf("process retunred erroR code %04x \n",code);
}

return 0;
}
}

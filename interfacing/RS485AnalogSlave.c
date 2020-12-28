/*

program to manually compose a Modbus RTU message to be sent over the RS485 bus

usage: $~ sudo ./manualRTC xx yy zz ...
where  xx, yy, zz are HEXIDECIMAL representations of each 8-bit byte, in order to be written to bus

example $~ sudo ./manual 05 0A FF

the CRC is automatically appended to the end of the byte array.

 */

#include "RS485AnalogSlave.h"

unsigned int readRS485AnalogSlave(unsigned short RS485Chan, unsigned short AnalogChan, float* ADCcount, float* stdev){
	char returndata[64];
	unsigned char cmd[16];  //still use char arrays since this is convient. do not do c-style string operations
	int i,j;
	unsigned int temp;
		cmd[0]=RS485Chan;
		cmd[1]=0x08;
		cmd[2]=0x0F; // change to cmd[2]=(BASEREGANLG & 0xFF00)>>8;
		cmd[3]=15 + AnalogChan; // change to cmd[3]=(BASEREGANLG & 0x00FF)+AnalogChan; //channel number.  0 to 3
		i=4;
		temp = modRTU_CRC(cmd,i);
		cmd[i+1]=(unsigned char)((temp&0xFF00)>>8);  //ensures that the MSByte is sent 
		cmd[i]=(unsigned char)(temp&0x00FF);  //before the LSByte
		write_rs485BYTE(cmd,i+2, returndata, &j); /*i is the  number of input bytes to send. 
								*/

if (!(returndata[1] & 0x80)){
		temp = ((returndata[3] << 8)| returndata[4]);  // NO CHANGES REQUIRED HERE
	//	printf("Raw sum of 10 measurements (0 to 10240) = %d\n",temp);
		*ADCcount = ((float) temp)/10.0;
		temp = ((returndata[5] <<8)| returndata[6]);
	//	printf("Raw sum of deviations: (10*Xi - 10*Xavg)² = %d\n",temp);
		*stdev =((float) temp) / 100.0;
		*stdev = sqrt(*stdev/9.0);  // should this be stderror instead of stdev?
		temp=0;
	} else {
		temp=returndata[2]<<8|returndata[3];
		printf("process returned error code %04x \n",temp);

	}
return temp;
}


unsigned int readRS485AnalogSlaveSimple(unsigned short RS485Chan, unsigned short AnalogChan, float* ADCcount){
	char returndata[64];
	unsigned char cmd[16];  //still use char arrays since this is convient. do not do c-style string operations
	int i,j;
	unsigned int temp;

	cmd[0]=RS485Chan;
	cmd[1]=0x03;
	cmd[2]= 0x0F; // change to cmd[2]=(BASEREGANLG & 0xFF00)>>8;
	cmd[3]= 15 + AnalogChan;// change to cmd[3]=(BASEREGANLG & 0x00FF)+AnalogChan;  // channel number.  0 to 3
	i=4;
	temp = modRTU_CRC(cmd,i);
	cmd[i+1]=(unsigned char)((temp&0xFF00)>>8);  //ensures that the MSByte is sent 
	cmd[i]=(unsigned char)(temp&0x00FF);  //before the LSByte
	write_rs485BYTE(cmd,i+2, returndata, &j); /*i is the  number of input bytes to send. 
				add two to this for the CRC bytes	*/

if (!(returndata[1] & 0x80)){
	temp = ((returndata[3] << 8) | returndata[4]);
	*ADCcount =  (float) temp;
//	printf("Raw ADC (0 to 1024) = %d\n",temp);
//	printf("Volts = %f volts\n",volts);
	temp=0;
} else {
		temp=returndata[2]<<8|returndata[3];
		printf("process returned error code %04x \n",temp);
}
return temp;
}



unsigned int setRS485AnalogSlavePeriod(unsigned short RS485Chan, unsigned short ADCperiods){
	char returndata[64];
	unsigned char cmd[16];  //still use char arrays since this is convient. do not do c-style string operations
	int i,j;
	unsigned int temp;

		cmd[0]=RS485Chan;
		cmd[1]=0x06;
		cmd[2]=0x00;//change to cmd[2]=(BASEREGANLG & 0xFF00)>>8;
		cmd[3]=0x0F; // change to cmd[3]=(BASEREGANLG & 0x00FF)+16;
		cmd[4]=0;
		cmd[5]=(ADCperiods & 0xFF);
		i=6;
	temp = modRTU_CRC(cmd,i);
	cmd[i+1]=(unsigned char)((temp&0xFF00)>>8);  //ensures that the MSByte is sent 
	cmd[i]=(unsigned char)(temp&0x00FF);  //before the LSByte

	write_rs485BYTE(cmd,i+2, returndata, &j); /*i is the  number of input bytes to send. 
					add two to this for the CRC bytes
*/

if ((returndata[1] & 0x80)){ 
		temp=returndata[2]<<8|returndata[3];
		printf("process returned error code %04x \n",temp);

} else {
temp=0;
}

return temp;
}

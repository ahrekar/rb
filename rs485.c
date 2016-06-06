
#include "rs485.h"

long int calcDelay(int size){
	long int del;
	// calculate the delay needed.. depends on the number of chars we are transmitting
	del=size*10*1000000;
	 /* (10=8bits+2parity)  bits per character  *  1000000 microseconds per second
		  8 bits per char plus start and stop bits. 
		Tested watching an O-scope with 4 to 10 input chars. */

	del=del/bd; // * 1 second per baud 
return del;
}


int write_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int writedata){
	unsigned char cmd[64];
	unsigned char returndata[64];
	unsigned short temp;
	int len,j,z;
	cmd[0]=address;
	cmd[1]=0x06; //command to write register(s)
	cmd[2]=((reg&0xFF00)>>8); //MSB which register
	cmd[3]=(reg&0x00FF); //LSB which register 

	temp=(unsigned short)writedata;
	cmd[4]=((temp & 0xFF00)>>8);// the next two bytes are the MSB and
	cmd[5]=(temp & 0x00FF);	  // and LSB of the data to send
	len=6; // count them

	temp = modRTU_CRC(cmd,len);//calculate the crc bytes. temp is 16bit, two byte value

	cmd[len+1]=(unsigned char)((temp&0xFF00)>>8);  //ensures that the MSByte is sent first as per Modbus
	cmd[len]=(unsigned char)(temp&0x00FF);  //before the LSByte

	write_rs485BYTE(cmd,len+2, returndata, &j);// this is a generic 'write data to RS485'
			// it assumes nuthing about the communication standard

	z=-1; // initialize an error variable.
	if(validateRTU(returndata,j)==0){	//CRC valid.No transmission errors
		// let make sure no machine/interpretation  errors
		if(cmd[0]==returndata[0]){  // then the corect machine responded
			if (returndata[1] & 0x80){ // then an  error
				z=returndata[2]; //in the event of an error this byte
			// is sent by the machine  explaining the nature of the error
			}else{
			 z=0;  // no errors. you're golden.
			} 
		}
	}
return z;
}



int read_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int* cnReturnData){
/*
This layer packs together a Modbus style command to read messages. they 
are send to rs485 communication
any  returned data is placed in cnReturnData. 
*/
	unsigned char cmd[64];
	unsigned char returndata[64];
	unsigned short temp;
	unsigned int  tempint;
	int len,j,i,z,numbytes;
//build a Modbus RTU style command message to send 

	cmd[0]=address;
	cmd[1]=0x03; //command to read register(s)
	cmd[2]=((reg&0xFF00)>>8); //MSB which register
	cmd[3]=(reg&0x00FF); //LSB which register  to start from
	cmd[4]=0x00;//MSB how many
	cmd[5]=0x01;//LSB how many registers to read...   just one, thank you very much
	len=6; // count them. length of command

	temp = modRTU_CRC(cmd,len);//calculate the crc bytes

	cmd[len+1]=(unsigned char)((temp&0xFF00)>>8);  //ensures that the MSByte is sent 
	cmd[len]=(unsigned char)(temp&0x00FF);  //before the LSByte

	write_rs485BYTE(cmd,len+2, returndata, &j);
 /* len is the  number of input bytes in the command to send. Add two for the CRC bytes
 and send ALL these bytes.  returndata holds any response */
	z=-1; //my way of recording errors
	tempint=0;

	if(validateRTU(returndata,j)==0){	/* this checks the last
				two CRC bytes to ensure
				transmission is  valid.*/

	if(returndata[0]==cmd[0]){ // then the correct machine responded
		if(returndata[1] & 0x80){// then an error occured
			z=returndata[2];
		}
		else{ 
	/*All good lets do something with the data.

		The return structure looks like 
		byte0: echo address
		byte1: echo command. If command is 03, then 03 is expected. However, if 
		there is an error it will return 83
		byte2: number of bytes of data to follow: n. 
		byte3: data byte 0.... 
		byte3+n: data byte n-1
		byte 3+n+1: LSB of CRC
		byte 3+n+2: MSB of CRC
		*/
		numbytes=returndata[2];

			for (j=0;j<numbytes;j++){
				tempint+=(returndata[3+j] << (8*(numbytes-1-j)));
			}
			z=0;
			*cnReturnData=tempint;
		}
	}
	}
return z;
}



unsigned short modRTU_CRC(char* buff, int len){
//calculates CRC for Modbus specs
	unsigned short crc=0xFFFF;
	int pos, bit;

	for(pos=0;pos<len;pos++){
		crc^=(unsigned short) buff[pos];
		for(bit=8;bit!=0;bit--){
			if((crc&0x0001)!=0){
				crc>>=1;
				crc^=0xA001;
			}else{
				crc>>=1;
			}
		}
	}
return crc;
}


int validateRTU(char* buff, int len){
	/* len is the full length of the buffer.  The last two elements in the array
	are assumed to be  CRC bytes.
		*/
	int j;
	unsigned short temp;
	j=-1;
	if(len>3){
		temp=((buff[len-1]<<8)|(buff[len-2]));
		if(temp==modRTU_CRC(buff,len-2)) j=0; // valid.yes.
	}
return j;
}


int initialize_rs485(int baud,int pin){
	if (wiringPiSetup () == -1) 
	  {
	    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
	    return 1 ;
	  }
	bd = baud;
	wp=pin;
  if ((fd = serialOpen ("/dev/ttyAMA0", bd)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }
	pinMode(wp,OUTPUT);
	digitalWrite(wp,HIGH);

}//end initialize


void write_rs485BYTE(char* cmd, int numchar, char* pszEcho, int* sizeEcho){

	int a,i,j;
	char buffer[64];

	digitalWrite(wp,LOW);// sets the control signal WRITE to the RS 485 buss
	delayMicrosecondsHard(10);// minor wait to allow signals to settle

	for (j=0;j<numchar;j++){
		serialPutchar(fd,cmd[j]);
	}
	// cannot read in from UART until done transmitting, so wait . . . 
	delayMicrosecondsHard(calcDelay(numchar));

	digitalWrite(wp,HIGH);// now set control to READ (i.e. LISTEN)

	delay(100); // wait some more so that the external device has time to transmitt.  Data fills the UART buffer.
	// this could be calculated based on what is expected.  usually, a delay of 30 works fine for 4 to 8 chars returned. 
	// it doesnt hurt if data sets in the UART buffer until it is read.
	i=0;
	    while (serialDataAvail (fd))
	    {
		a = serialGetchar(fd);
		buffer[i]=a;
		i++;
	    }
//KARL could probably eliminate the need for buffer[]?
	for (j=0;j<i;j++){
		pszEcho[j]=buffer[j];
	}
	*sizeEcho=i;
}


void write_rs485ASCII(char* cmd, char* pszEcho, int sizeEcho){
	/* routine to write  ASCII data to the rs485 serial port. If a device
	requires a CR or LF in the instruction, it must be in the cmd array. it is not
	apended in this function. Because different devices have difffernt expectations*/

	int length,j,i;
	int a;
	char buffer[64];

	length=strlen(cmd);

	 /* get the string length.  Since this is a null-terminated c string, '0' is assumed to be the
	last element in the array*/


	digitalWrite(wp,LOW);// sets the control signal WRITE to the RS 485 buss
	delayMicrosecondsHard(10);// minor wait to allow signals to settle

	//serialPuts (fd, cmd);
	for (j=0;j<length;j++){
		serialPutchar(fd,cmd[j]);
	}

	// cannot read in from UART until done transmitting, so wait . . . 
	delayMicrosecondsHard(calcDelay(length));

	digitalWrite(wp,HIGH);// now set control to READ (i.e. LISTEN)

	delay(100); // wait some more so that the external device has time to transmitt.  Data fills the UART buffer.
	// this could be calculated based on what is expected.  usually, a delay of 30 works fine for 4 to 8 chars returned. 
	// it doesnt hurt if data sets in the UART buffer until it is read.
	i=0;
	    while (serialDataAvail (fd))
	    {
		a = serialGetchar(fd);
		if (a>31){ // Since we are expecting ASCII in the response, ignore control characters
			buffer[i]=a;
		i++;
		}
	    }
	buffer[i]='\0';  //because this is treated as a string, must manually
		// append the NULL terminator

	strncpy(pszEcho,buffer,sizeEcho);

}//end write_rs485

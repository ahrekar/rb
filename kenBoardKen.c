
Conversation opened. 2 messages. 2 messages unread.

Skip to content
Using Gmail with screen readers
1 of 7
FW: new files
Inbox
	x
UNL
	x
Kenneth W. Trantham via huskersunl.onmicrosoft.com 
	
AttachmentsSep 23, 2018, 7:45 PM (2 days ago)
	
to Karl
So,

yes, the GPIO pin used to control the RS485 has changed. And, the control logic is inverted.  And, for some reason, when I transmit to a slave device, it also reads an extraneous 0x00 at the end.  The solution was to add a bit more wait time on the Pi.

 As I recall, the board I sent with you (let's call it the V3 board) only had the RS485 stuff and the analog stuff on the board.

 I have been using these files extensively on a model 2 for another project. Things seem to be working.  Please do not hesitate to contact me with questions.



Kenneth W. Trantham
Department of Physics & Astronomy
2401 11th Ave / Bruner Hall
University of Nebraska - Kearney
Kearney, NE 68849
*******************************
Ph: (308) 865-8278
Lab: (308) 865-8289
Fx: (308) 865-8281
________________________________________
From: root [root.raspberry.pi@gmail.com]
Sent: Sunday, September 23, 2018 7:40 PM
To: Kenneth W. Trantham
Subject: new files

new files

4 Attachments
	
Kenneth W. Trantham

	
	
Kenneth W. Trantham via huskersunl.onmicrosoft.com 
	
Attachments11:58 AM (3 minutes ago)
	
to Karl


Kenneth W. Trantham
Department of Physics & Astronomy
2401 11th Ave / Bruner Hall
University of Nebraska - Kearney
Kearney, NE 68849
*******************************
Ph: (308) 865-8278
Lab: (308) 865-8289
Fx: (308) 865-8281
________________________________________
From: Kenneth W. Trantham
Sent: Sunday, September 23, 2018 7:45 PM
To: Karl Ahrendsen
Subject: FW: new files
4 Attachments
Karl Ahrendsen
	
12:01 PM (0 minutes ago)
	
to Kenneth
Yep, I remember now. I guess that's what I get for insisting on replying to three separate e-mails instead of using just one reply. It's easy to lose things then.

Thanks.
	
	
	

#include <mcp3004.h>
#include "kenBoard.h"
#include <wiringPi.h>
#include <wiringSerial.h>

// ADC variables
#define BASE 100
#define SPI_CHAN 0
// wiring pi GPIO pin assigments to stepper motor drivers
#define MTR0DIR 4
#define MTR0CLK 3
#define MTR0HOME 5
#define MTR0DLY 1500
#define MTR0SPR 400

#define VALVE0 21
#define VALVE1 26

#define MTR1DIR 0
#define MTR1CLK 1
#define MTR1HOME 2
#define MTR1DLY 1500
#define MTR1SPR 400

void delayMicrosecondsHard(unsigned int howLong);

int x_stepMotor(unsigned int clkpin, unsigned int steps, unsigned int dirpin,unsigned int dir, unsigned int dly);

int fd, bd, wp;
// these are for RS485 communications

long int calcDelay(int size){
	long int del;
	// calculate the delay needed. for RS485 communications. depends on the number of chars we are transmitting
	del=size*10*1000000;
	 /* (10=8bits+2parity)  bits per character  *  1000000 microseconds per second
		  8 bits per char plus start and stop bits. 
		Tested watching an O-scope with 4 to 10 input chars. */
	del=del/bd; // * 1 second per baud 
return del;
}

void delay_uS(int x){

delayMicrosecondsHard(x);



}

int initializeBoard(){

	if (wiringPiSetup () == -1) 
	  {
	    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
	    return 1 ;
	  }

	mcp3004Setup(BASE,SPI_CHAN);// for ADC
	initialize_rs485(9600,0);

// stepper motor clock MUST be set to low at the begining and end of each function call. 
	pinMode(MTR0CLK,OUTPUT);
	digitalWrite(MTR0CLK,LOW);
	pinMode(MTR1CLK,OUTPUT);
	digitalWrite(MTR1CLK,LOW);

	// Setup the proper pins to receive input for the homing function
	pinMode(MTR0HOME,INPUT);
	pinMode(MTR1HOME,INPUT);

	pinMode(VALVE0,OUTPUT);
//	digitalWrite(VALVE0,LOW);
	pinMode(VALVE1,OUTPUT);
//	digitalWrite(VALVE1,HIGH);


return 0;

}


int closeBoard(){
	serialClose(fd);
return 0;
}



int getADC(unsigned short chan, unsigned int * returndata){
/*
uses MCP3004 chip attached to SPI channel 0 of RasPi GPIO
this chip has 8 analog inputs, chan 0 ... 7
returns value 0 to 1023

Cnannel descriptions present board:
Ch0 = 0 to 10 V
Ch1 = 0 to 10 V
Ch2 = 0 to 5 V
Ch3 = 
Ch4 =
Ch5 =
Ch6 =
Ch7 = 
*/

int x;
	x=analogRead(BASE + chan);
	*returndata=x;
return 0;
}


/*
private function (for now)
*/

void setValveBoard(unsigned int valve, unsigned short state){

	switch(valve){
		case 0:
			digitalWrite(VALVE0,state);

			break;
		case 1:
			digitalWrite(VALVE1,state);

			break;
	}
}

int homeMotor(unsigned short mtr){
	int stepsTaken=0;
	unsigned int p_home,p_stepsPerRev;
	p_home=0;
	p_stepsPerRev=400;
	switch(mtr){
		case 0:
			p_home=MTR0HOME;
			p_stepsPerRev=MTR0SPR;
			break;
		case 1:
			p_home=MTR1HOME;
			p_stepsPerRev=MTR1SPR;
			break;
		case 2:
			break;
		default:
			return -1;
	}
	if (p_home !=0){

	if (digitalRead(p_home)){//Already in home
		// Then move away from home and allow it 
		// to re-find it.
		printf("Already in home, reversing 100 steps...\n");
		stepMotor(mtr,CLK,100);
		if(digitalRead(p_home)){
			printf("Error: Home state not changing\n");
			return -1;
		}
	}
	while(!digitalRead(p_home) && stepsTaken < p_stepsPerRev){
		stepMotor(mtr,CCLK,1);
        stepsTaken++;
	}
	printf("Found home in %d steps\n",stepsTaken);
	return stepsTaken;
	} else {
	return -1;
	}
}

int stepMotor(unsigned short mtr,unsigned int dir, unsigned int steps){
	unsigned int p_dir, p_clock, p_delay;
//, p_stepsPerRev;
	int p_home;
p_home=0;
p_dir=0;
p_delay=0;
//p_stepsPerRev=100;
p_clock=0;
	switch(mtr){
	case 0:
		p_dir=MTR0DIR;
		p_clock=MTR0CLK;
		p_home=MTR0HOME;
		p_delay=MTR0DLY;
//		p_stepsPerRev=MTR0SPR;
		break;
	case 1:
		p_dir=MTR1DIR;
		p_clock=MTR1CLK;
		p_home=MTR1HOME;
		p_delay=MTR1DLY;
//		p_stepsPerRev=MTR1SPR;
		break;
	case 2:
		break;
	default:
		return -1;
	}

//	p_home=p_home;
//	p_stepsPerRev=p_stepsPerRev;
if(p_home !=0){
	x_stepMotor(p_clock,steps,p_dir,dir,p_delay);
	return 0;
	}else{
	return -1;
	}
}

int x_stepMotor(unsigned int clkpin, unsigned int steps, unsigned int dirpin,unsigned int dir, unsigned int dly){
	int i;
	pinMode(clkpin,OUTPUT);
	pinMode(dirpin,OUTPUT);
	digitalWrite(dirpin,dir);

	for (i=0;i<steps;i++){
		digitalWrite(clkpin,HIGH);
		delayMicrosecondsHard(dly);
		digitalWrite(clkpin,LOW);
		delayMicrosecondsHard(dly);
	}
	return 0;
}

int setMotor(unsigned short motor, int newLocation){
	int returnValue;
	returnValue=homeMotor(motor);
	if(returnValue==-1)
		return returnValue;
	returnValue=stepMotor(motor,CCLK,newLocation);
	if(returnValue!=0)
		return returnValue;
	return 0;
}



int write_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int writedata){
	unsigned char cmd[64];
	unsigned char returndata[64];
	short temp;
	unsigned int len,j,z;
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
	/*
	unsigned char cmd[64];
	unsigned char returndata[64];
	unsigned short temp;
	unsigned int  tempint;
	*/
	unsigned char cmd[64];
	unsigned char returndata[64];
	short temp;
	int  tempint;
	unsigned int len,j,z,numbytes;
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
				z=(returndata[2]<<8)|returndata[3];
			}
			else{ 
				/*All good lets do something with the data.
				 *
				 * The return structure looks like 
				 * byte0: echo address
				 * byte1: echo command. If command is 03, then 03 is expected. However, if 
				 * there is an error it will return 83
				 * byte2: number of bytes of data to follow: n. 
				 * byte3: data byte 0.... 
				 * byte3+n: data byte n-1
				 * byte 3+n+1: LSB of CRC
				 * byte 3+n+2: MSB of CRC
				 * 
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



unsigned short modRTU_CRC(unsigned char* buff, unsigned short len){
//calculates CRC for Modbus specs
	unsigned short crc=0xFFFF;
	unsigned short pos, bit;

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


int validateRTU(unsigned char* buff, unsigned short len){
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
	bd = baud;
	wp=pin;
  if ((fd = serialOpen ("/dev/ttyAMA0", bd)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }
	pinMode(wp,OUTPUT);
	digitalWrite(wp,LOW);
	return 0;
}//end initialize


void write_rs485BYTE(unsigned char* cmd, unsigned int numchar, unsigned char* pszEcho, unsigned int* sizeEcho){

	unsigned int i,j;
	unsigned int loop;

	digitalWrite(wp,HIGH);// sets the control signal WRITE to the RS 485 buss
	delayMicrosecondsHard(100);// minor wait to allow signals to settle

	for (j=0;j<numchar;j++){
		serialPutchar(fd,cmd[j]);
	}
	// cannot read in from UART until done transmitting, so wait . . . 
	delayMicrosecondsHard(calcDelay(numchar+2));

	digitalWrite(wp,LOW);// now set control to READ (i.e. LISTEN)
	loop=0;
	j=0;
	do {
		delay(120);
		i=0;
		while (serialDataAvail(fd)){
		    while (serialDataAvail (fd)){
			pszEcho[i]=serialGetchar(fd);
			i++;
	    		}
		delay(50);
		}
		if (i>0) loop=24;
		loop++; j++;
	} while(loop<24);
	//debug printf("number of loops %d,j);

	*sizeEcho=i;
}

kenBoard.c
Displaying kenBoard.c.

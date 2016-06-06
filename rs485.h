/*
RS485 LAYER::
RS485 is an electrical communcation standard.(thats all)
see  https://en.m.wikipedia.org/wiki/RS-485
 Since the RS485 communication devics we are using is 'half-duplex'
 we must wait until the UART is done transmitting.   The UART, once
 instructed with what infomration to send, opperates independently of the OS
 or your program. Therefore, we use a digital IO to control Write/Listen.  We
 must hold the control signal to WRITE to the RS485 buss until we're done
transmitting. Even if the full instruction has been written to the serial output, 
the UART may not complete send until much later.  This layer waits an appropriate
amount of time before listening for a response. 


This code assumes that a MAX485 integrated circuit is used
https://datasheets.maximintegrated.com/en/ds/MAX1487-MAX491.pdf

+5v - Vcc 
B  - "B" of RS485 connection
A - "A" of RS485 connection
gnd - ground

DI - Tx of UART (gpio)
!RE and DE  - connected together. 
RO - Rx of UART

!WRITE/LISTEN controlled with common emmitter transitor buffer/levelshifter/inverter:

			^+5V
			|
			\
			/ 2k
			\
			/
			|
!RE and DE -------------[C]	6k
			[B]----/\/\/\----<--GPIO output (#26)
	2N2222(NPN)	[E]
			|
			GND

MODBUS RTU LAYER::
uses Modbus 16 communcation protocal .  Modbus specifies how data is organized
and be transmitted over different types of electrical standards (e.g. rs485, rs232,...)

Useful resource:
http://www.lammertbies.nl/comm/info/modbus.html

8 bits
No parity
1 stop bit(I think)
9600 baud

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>


//global varabiles for the functions. 
// i realize these are visible to a unit that calls this.
//KARL:: Can u  Make them private? but accessable to all functions in this module
int fd;
int bd;
int wp;

// RS485 I/O
int initialize_rs485(int baud, int pin);
void write_rs485BYTE(char* cmd, int numchar, char* pszEcho, int* sizeEcho);
void write_rs485ASCII(char* cmd, char* pszEcho, int sizeEcho);


/*Modbus protocal using RS485 communation

*/
unsigned short modRTU_CRC(char* buff,int len);
// calculates Modbus16 CRC. Returns CRC word. This gets seperated into two bytes

int validateRTU(char* buff, int len);
//checks a byte buffer . Assumes that the last two bytes are Modbus16 CRC

int write_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int writedata);
//writes a message to machine at address, and to register reg.
// returns 0 if no errors
// returns -1 for general errors, such as CRC check error or time-out no response(100ms)
// returns machine specific error code if the machine responds

int read_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int* cnReturnData);
//read from a register for a machine at address. 
//same errors are returned.
//if an error is returned then cnReturnData is junk

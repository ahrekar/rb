/*

KENBOARD electrical layer for wiring Pi interface to RaspPi GPIO. See schematics



RS485 LAYER::
	RS485 is an electrical communcation standard.(thats all)
	see  https://en.m.wikipedia.org/wiki/RS-485

	This code assumes that a MAX485 integrated circuit is used
	https://datasheets.maximintegrated.com/en/ds/MAX1487-MAX491.pdf

	+5v - Vcc 
	B  - "B" of RS485 connection
	A - "A" of RS485 connection
	gnd - ground

	DI - Tx of UART (gpio)
	!RE and DE  - connected together. 
	RO - Rx of UART


MODBUS RTU LAYER::
	uses Modbus 16 communcation protocal .  Modbus specifies how data is organized
	and can be transmitted over different types of electrical standards (e.g. rs485, rs232,...)

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

#define POL_MOTOR	0
#define PROBE_MOTOR	1
#define PUMP_MOTOR	2
#define CCLK 0
#define CLK 1

/* KenBoard board level IO.  See schematics for orgainziation of IO and GPIO wiringPi assignments

*/
int initializeBoard();
//wiring pi setup , initialyze global variables

int getADC(unsigned short chan, unsigned int * returndata);
// uses MCP3008 ADC conected to SPI channel 0.

int stepmotor(unsigned short mtr,unsigned int dir, unsigned int steps);
// bit-bangs clock and direction for stepper motor driver.

//int homemotor(unsigned short chan, unsigned int* steps);
// returns number os steps moved to find home

// int readMotorPosition(unsigned short chan, int * returndata);
int readDigital(unsigned short chan, unsigned short* returndata);

int writeDigital(unsigned short chan, unsigned short writedata);


// closeBoard();
//


/*
 RS485 I/O
*/
void write_rs485BYTE(char* cmd, int numchar, char* pszEcho, int* sizeEcho);
void write_rs485ASCII(char* cmd, char* pszEcho, int sizeEcho);
// uses RaspiUART and one GPIO pin to control serial IO through RS485

/*
Modbus protocal using RS485 communation
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

int initialize_rs485(int baud, int pin);

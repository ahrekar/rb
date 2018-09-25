/*

KENBOARD electrical layer for wiring Pi interface to RaspPi GPIO. See schematics



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
int closeBoard();

int getADC(unsigned short chan, unsigned int * returndata);
// uses MCP3008 ADC conected to SPI channel 0.

int stepMotor(unsigned short mtr,unsigned int dir, unsigned int steps);
int homeMotor(unsigned short mtr);
int setMotor(unsigned short mtr, int newLocation);
// bit-bangs clock and direction for stepper motor driver.

//int homemotor(unsigned short chan, unsigned int* steps);
// returns number os steps moved to find home

// int readMotorPosition(unsigned short chan, int * returndata);
void setValveBoard (unsigned int valve, unsigned short state);


int readDigital(unsigned short chan, unsigned short* returndata);

int writeDigital(unsigned short chan, unsigned short writedata);

void delay_uS(int x);

/*


 RS485 I/O


*/
void write_rs485BYTE(unsigned char* cmd, unsigned int numchar, unsigned char* pszEcho, unsigned int* sizeEcho);
// uses RaspiUART and one GPIO pin to control serial IO through RS485

unsigned short modRTU_CRC(unsigned char* buff,unsigned short len);
// calculates Modbus16 CRC. Returns CRC word. This gets seperated into two bytes

int validateRTU(unsigned char* buff, unsigned short len);
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

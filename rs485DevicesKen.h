/*

General header File to encapsulate all slave device functions
 */


#define BASEREGANLG 0x0D0D
#define BASEREGSERVO 0x0A0A
#define BASEREG485BRIDGE232 0x0C0C
#define BASEREGSTEPMTR 0x0B0B
#define BASEREGFN 0x00F0

/* **************************************************************************************************

Universal ID function.  All slave devices will respond to this.

*/
int getRS485SlaveID(unsigned char* returnData, unsigned  short Address);

/* **************************************************************************************************

Stepper motor slave module:

*/
int findHomeRS485StepperMotor(unsigned short chan, unsigned char state, unsigned char direction);
//macro that has device find home on its own
int moveRS485StepperMotor(unsigned short chan, unsigned short steps, unsigned char dir);
//move by number of steps
int getRS485StepperMotorSteps(unsigned short chan, unsigned int* steps);
// use this to query motion.  The return value is steps yet to move. This counts down to zero when the move is completed
int getRS485StepperMotorStepsRev(unsigned short chan, unsigned int* stepsperrev);
int setRS485StepperMotorStepsRev(unsigned short chan, unsigned short stepsperrev);
// stepsperrev is only used in homing. If home not found in stepsperrev, motion ends
int getRS485StepperMotorSpeed(unsigned short chan, unsigned int* speed);
int setRS485StepperMotorSpeed(unsigned short chan, unsigned short speed);
// speed that pulses are sent out

/* **************************************************************************************************

Dual servo slave module:

*/
int setRS485ServoPosition(unsigned short chan, unsigned short servo, unsigned short position);
int getRS485ServoPosition(unsigned short chan, unsigned short servo, unsigned int* position);
/* **************************************************************************************************

Digital out module

*/

int setRS485DigitalOut(unsigned short chan, unsigned short value);
int getRS485DigitalOut(unsigned short chan, unsigned short * value);



/* **************************************************************************************************

Analog recorder slave module

*/
int setRS485AnalogRecorderPeriod(unsigned short RS485Chan, unsigned short ADCperiods);
int getRS485AnalogRecorderPeriod(unsigned short RS485Chan, unsigned int* ADCperiods);
// ADCperiods is in units of 16mS.  How often the PIC takes, and buffers, a reading
unsigned int readRS485AnalogRecorderSimple(unsigned short RS485Chan, unsigned short AnalogChan, float* ADCcount);
unsigned int readRS485AnalogRecorder(unsigned short RS485Chan, unsigned short AnalogChan, float vref, float* volts, float* stdev);
/* vref scales the ADCcount. For an analog signal, undivided by resistors into the PIC, vref=5.0.  This will need to be modified 
 if there is an input voltage divider.  If the returned value needs to not be converted to an actual voltage (as we do with the 
convectrons), then set vref=1024.0. 

		R1		R2
[Vinput] ───/\/\/\──────┬──────/\/\/\───┐
			│		│
			│	      [gnd]
			│
			└──────────────────────────────[PIC analog input]. [0 to 5 V full scale = 0 to 1024 ADCcount]
*/



/* **************************************************************************************************

RS485 to RS232 Bridge  module

*/
int writeRS485to232Bridge(unsigned char* cmd, unsigned char* returnData, unsigned  short Address);
// passes ascii cmd to 232 device. returnData is response from device.
int setRS485BridgeReads(unsigned short reads, unsigned short Address);
int getRS485BridgeReads(unsigned short* reads, unsigned short Address);
int setRS485BridgeTimeout(unsigned short timeout,unsigned short Address);
int getRS485BridgeTimeout(unsigned short* timout,unsigned short Address);

// GPIB bridge functions

int getRS485GPIBStatus(unsigned short* GPIBStatus, unsigned short Address);
/*
This is more of a debugging thing. indicates the current status of the control lines on the GPIB buss.
*/

int resetGPIBBridge(unsigned short Address); // Reasserts REN, and sends IFC to all devices


int sendGPIBCommand(unsigned char* cmd, unsigned short Address);
/* sendGPIBCommand( command to send, RS485 address of bridge device)
Asserts ATN and writes cmd to data bus.  multiple byte commands possible.
Common commands are Talk, Listen, UnListen, go to local, etc.

see http://www.ni.com/white-paper/3389/en/
Table 1 for a full list of commands
e.g. sending cmd=
0x2A puts GPIB device address 10 into listen mode
0x01 puts all devices into local mode.
0x3F tells all devices to unlisten
0x6A tells GPIB device address 10 to talk
*/


int sendGPIBData(unsigned char *cmd, unsigned short Address);
/* sendGPIBData(data to send, RS485 address of bridge device)

data is generally device dependent string data to manimpulate the device.

e.g. sending "R0X" to a keithley 485 puts the meter into auto mode.  (but first one must tell the device to listen!

*/


int listenGPIBData(unsigned char *returnData, char terminator, unsigned short Address);
/* listenGPIBData( buffer for return data, RS485 address of bridge device)

need to ensure there is a timeout. If a device is not told to talk, there will be no data returend
*/


/*
 GENERAL GPIB PROGRAMMING SEQUENCE

initialize:  resetGPIBbridge

tell a GPIB device to listen: send command (0x20 + GPIB address ) to RS485address of bridge

send any configuration data that the GPIB device needs, such a setting a scale, or indicating when to take a reading, or
telling it to send a reading. The string required will depend on the device in question: send data($data,to RS485_GPIB bridge address)

If we are expecting return data, like a measurement
tell the GPIB device to talk: send command (0x60 + GPIB address) to RS485address of bridge

call listenGPIBdata.  each device will employ different terminators . LF =0x0A is common.  Return string is placed 
into rtnData. include a timeout incase there is an error and device is not responding

*/

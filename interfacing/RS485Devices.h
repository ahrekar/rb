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



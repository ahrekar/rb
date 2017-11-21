/*
   notes and comments 
   useful information
   to follow

*/

#include "flipMirror.h"

#define FM_REG 0x0F0F


int setFlipMirror(unsigned short chan, unsigned short position){
	int status;
/* position should range from 0 to 8
	with
	0 = mirror full down 
	8 = mirror full up
*/
	if (position > 8) position = 8;
	// the microcontroller is looking for a range between 181 and 189

	status=write_Modbus_RTU(chan,FM_REG, position);

	return status;
}

int getFlipMirror (unsigned short chan, unsigned short* position){

	int status;
	unsigned int temp;
	status = read_Modbus_RTU(chan,FM_REG, &temp);
	*position = temp;
	return status;

}

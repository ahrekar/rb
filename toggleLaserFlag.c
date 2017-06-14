/*
Program to tell the servo to rotate so that it will block the laser. 
This is mainly example code so that you can see how to implement it
in a different program that you write.

*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "interfacing/interfacing.h"
#define AA 23
#define BB 24
#define UNBLOCKED 0
#define BLOCKED 1

int main (int argc, char *argv[])
{
	unsigned short device;
	//unsigned short value;
	int value;
    int state;

	if (argc==2) {
		device = atoi(argv[1]);
	}else{

		printf("Usage ./toggleLaserFlag <device 0,1>\n");
		printf("Device\n");
		printf("======\n");
		printf("Probe = 0\n");
		printf("Pump = 1\n");
		return 1;
	}

	initializeBoard();


	switch (device) {
	case 1:
		printf("Toggling Pump Laser\n");
        state=digitalRead(AA);
		if (state & 0b0001)
			value=UNBLOCKED;
		else
			value=BLOCKED;
		pinMode(AA,OUTPUT);
		digitalWrite(AA, value);
		printf("Pin state changed to: %d (%s)\n",value,(value==BLOCKED)?"BLOCKED":"UNBLOCKED");
		break;
	case 0:
		printf("Toggling Probe Laser\n");
        state=digitalRead(BB);
		if (state & 0b0001)
			value=UNBLOCKED;
		else
			value=BLOCKED;
		pinMode(BB,OUTPUT);
		digitalWrite(BB, value);
		printf("Pin state changed to: %d (%s)\n",value,(value==BLOCKED)?"BLOCKED":"UNBLOCKED");
		break;
	default:
		printf("invalid device number\n");
		break;
	}
	return 0;
}

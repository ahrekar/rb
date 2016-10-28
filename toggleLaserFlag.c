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

int main (int argc, char *argv[])
{
	unsigned short device;
	//unsigned short value;
	int value;

	if (argc==2) {
		device = atoi(argv[1]);
	}else{

		printf("Usage ./setLaserFlag <device 0,1> <Orientation 0,1>\n");
		printf("Device\n");
		printf("======\n");
		printf("Probe = 0\n");
		printf("Pump = 1\n");
		return 1;
	}

	initializeBoard();


	switch (device) {
	case 1:
		printf("Pin state: %d\n",digitalRead(AA));
		if (digitalRead(AA) & 0b0001)
			value=0;
		else
			value=1;
		pinMode(AA,OUTPUT);
		digitalWrite(AA, value);
		break;
	case 0:
		printf("Pin state: %d\n",digitalRead(BB));
		if (digitalRead(BB) & 0b0001)
			value=0;
		else
			value=1;
		pinMode(BB,OUTPUT);
		digitalWrite(BB, value);
		break;
	default:
		printf("invalid device number\n");
		break;
	}
	return 0;
}

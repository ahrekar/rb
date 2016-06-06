/*
Program to tell the servo to rotate so that it will block the laser. 
This is mainly example code so that you can see how to implement it
in a different program that you write.

*/

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#define AA 23
#define BB 24

int main (int argc, char *argv[])
{
	unsigned short value;

	if (argc==2) {
		value = atoi(argv[1]);
		}else{
		value = 0;
		}

	wiringPiSetup();

	pinMode(AA,OUTPUT);
	pinMode(BB,OUTPUT);

	digitalWrite(AA, (value & 0b0001));
	digitalWrite(BB, ((value & 0b0010) >> 1) );

}

/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "USB1208.h"


#define HOURSINDAY 24
#define MINUTESINHOUR 60
#define SECONDSINMINUTE 60
#define BUFSIZE 1024

int main (int argc, char* argv[]){

	unsigned short chan, outValue;

	initializeUSB1208();


	if (argc==3){
	chan = atoi(argv[1]);
	outValue = atoi(argv[2]);
	setUSB1208AnalogOut(chan,outValue);

	} else {

	printf("Usage:  ./setAout <chan 0-1> <value 0-1023>");


	}
		closeUSB1208();
		return 0 ;
}

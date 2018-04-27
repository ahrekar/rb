/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "interfacing.h"

#define UP 0
#define DOWN 5

int main (int argc, char* argv[]){

	unsigned short i;
	int z;

	initializeBoard();
	initializeUSB1208();

	if (argc == 2) {
		i = atoi(argv[1]);
		if(i==0)i=UP;
		else i=DOWN;
		z=setFlipMirror(0xA3,i);
		if (z>0) printf("Error occured: %d\n",z);
	} else {
		printf("usage:\n \
				    ./setMirror (0 or 1)\n");
	}

	delay(300);
	i=0;
	z = getFlipMirror(0xA3,&i);

	if (z>0) printf("error occured: %d\n",z);

	if(i==0)printf("Mirror UP (Measuring Probe)\n");
	if(i==7)printf("Mirror DOWN  (Measuring Pump)\n");
	printf("Current position: %d\n",i);

	return 0 ;


}

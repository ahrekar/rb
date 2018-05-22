/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "kenBoard.h"
#include "RS485Devices.h"

#define FLIPMIRROR 0xA2  // <<-- tested with another servo unit. 

int main (int argc, char* argv[]){

	unsigned int i;
	int z;

	initializeBoard();

	if (argc == 2) {
		i = atoi(argv[1]);
		z=setServoPosition(FLIPMIRROR,0,i);
		if (z>0) printf("error occured: %d\n",z);
	} else {
		printf("usage sudo ./mirrorTemplate <int 0 - 8>\n");
	}

	delay(300);
	i=0;
	z = getServoPosition(FLIPMIRROR,0,&i);

	if (z>0) printf("error occured: %d\n",z);

	if(i==0)printf("Measuring Pump Beam\n");
	if(i==7)printf("Measuring Probe Beam\n");
	printf("current position %d\n",i);

	return 0 ;


}

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

#define FLIPMIRROR 0xA3  // <<-- tested with another servo unit. 

int setMirror(int pos){
	int z;

	z=setRS485ServoPosition(FLIPMIRROR,0,pos);
	if (z>0) printf("Error occured: %d\n",z);
	
	delay(300);

	return 0;
}

int getMirror(void){
	unsigned int i;
	int z;
	z = getRS485ServoPosition(FLIPMIRROR,0,&i);
	if (z>0){
		printf("Error occured: %d\n",z);
		return z;
	}
	//if(i==0)printf("Measuring Probe Beam\n");
	//if(i==8)printf("Measuring Pump Beam\n");

	return i;
}

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

int setFlag(int device, int pos);
int getFlag(int device);

int setFlag(int device, int pos){
	int z;

	z=setRS485ServoPosition(device,0,pos);
	if (z>0){
		printf("Error occured: %d\n",z);
		return z;
	}
	
	return 0;
}

int getFlag(int device){
	unsigned int i;
	int z;
	z = getRS485ServoPosition(device,0,&i);
	if (z>0){
		printf("Error occured: %d\n",z);
		return z;
	}

	return i;
}

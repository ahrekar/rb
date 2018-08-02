/*
   notes and comments 
   useful information
   to follow

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "kenBoard.h"
#include "flipMirror.h"
#include "RS485Devices.h"

int main (int argc, char* argv[]){
	initializeBoard();
	int i,z;
	if (argc == 2) {
		i = atoi(argv[1]);
		if(i>0){i=8;}else{i=0;}
		z=setMirror(i);
	} else {
		printf("usage: sudo ./setMirror <0 for up (Probe), 1 for down (Pump)\n");
	}
	getMirror();
	return 0;
}

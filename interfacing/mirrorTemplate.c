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

int main (int argc, char* argv[]){

	        unsigned short i;
		int z;

		initializeBoard();

	if (argc == 2) {
		i = atoi(argv[1]);
		z=setFlipMirror(0x22,i);
		if (z>0) printf("error occured: %d\n");

	} else {

		printf("usage sudo ./mirrorTemplate <int 0 - 8>\n");

	}

	delay(300);
	i=0;
	z = getFlipMirror(0x22,&i);

	if (z>0) printf("error occured: %d\n");

	printf("current position %d\n",i);

	return 0 ;


}

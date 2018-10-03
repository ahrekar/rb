#include <stdlib.h>  //atof
#include "probeLaserControl.h"
#include "interfacing/kenBoard.h"

int main(int argc, char** argv){
	float desiredDetuning;

	initializeBoard();

	if(argc==2){
		desiredDetuning=atof(argv[1]);
		printf("Detuning: %2.2f\n",desiredDetuning);
	}else{
		printf("Usage: ./setProbeDetuning <detuning in GHz>\n");
		return 1;
	}

	setProbeDetuning(desiredDetuning);
	return 0;
}

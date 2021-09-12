#include <stdlib.h>  //atof
#include "pumpLaserControl.h"
#include "interfacing/kenBoard.h"
#include "interfacing/topticaLaser.h"

int main(int argc, char** argv){
	float desiredDetuning;
	int laserSock;

	if(argc==2){
		desiredDetuning=atof(argv[1]);
		printf("Detuning: %2.2f\n",desiredDetuning);
	}else{
		printf("Usage: ./setPumpDetuning <detuning in GHz>\n");
		return 1;
	}

	initializeBoard();
	laserSock=initializeLaser();

	setPumpDetuning(laserSock,desiredDetuning);
	return 0;
}

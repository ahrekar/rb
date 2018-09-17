/*
	Set TA Current
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // close()
#include "interfacing/topticaLaser.h"
#include "interfacing/kenBoard.h"


int main (int argc, char* argv[]){
	int i;
	float current;
	int line=0;
	int laserSocket;
    char buffer[1024];

	initializeBoard();
	laserSocket=initializeLaser();

	turnOffLaser(laserSocket);

	close(laserSocket);

	return 0;
}

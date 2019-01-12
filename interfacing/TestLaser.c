/*

   test bridge 


   simple function .  testing on an Epson Projector.


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "topticaLaser.h"
#include "kenBoard.h"


int main (int argc, char* argv[]){
	float temperature;
	int current;
	int laserSocket;

	initializeBoard();
	laserSocket=initializeLaser();

	printf("\n");
	printf("Present Laser Current= ");
	temperature=getMasterCurrent(laserSocket);
	printf("%f \n",temperature);

	close(laserSocket);

	return 0;
}

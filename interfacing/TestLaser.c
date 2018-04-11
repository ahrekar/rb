/*

   test bridge 


   simple function .  testing on an Epson Projector.


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sacherLaser.h"
#include "kenBoard.h"


int main (int argc, char* argv[]){
	float temperature;
	int current;

	initializeBoard();

	//initializeLaser();
	//printf("\n");
	//printf("Present Laser Temperature= ");
	//temperature=getLaserTemperature();
	//printf("%f \n",temperature);

	initializeTA();

//	if (argc==2){
//		temperature=atof(argv[1]);
//		printf("Setting Laser Temperature:= %2.3f\n",temperature);
//		setLaserTemperature(temperature);
//	}

	if (argc==2){
		current=atoi(argv[1]);
		printf("Setting TA current:= %dmA\n",current);
		setTACurrent(current);
	}
	return 0;
}

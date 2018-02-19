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


initializeBoard();


initializeLaser();
printf("\n");

printf("Present Laser Temperature:= ");
temperature=getLaserTemperature();
printf("%f \n",temperature);


if (argc=2){
	temperature=atof(argv[1]);
	setLaserTemperature(temperature);
	printf("Setting Laser Temperature:= %2.3f\n",temperature);

}

  return 0 ;
}

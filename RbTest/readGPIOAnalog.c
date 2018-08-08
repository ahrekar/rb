#include "kenBoard.h"
#include <stdio.h>
#include <math.h>

int main(int argc,char* argv[])
{
	int rdata;
	int gpioADC=3;
	float returnValue;
	initializeBoard();

	printf("Board initialized!\n");

	getADC(gpioADC,&rdata);
	printf("Rdata: %d\n",rdata);
	returnValue = pow(10,(0.004875*rdata - 3.991));

	printf("Convectron reads: %f\n", returnValue);
	
	return 0;
}

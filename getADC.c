#include "interfacing/kenBoard.h"
#include <stdio.h>
int main(int argc,char** argv){
	int result;
	float voltage;
	initializeBoard();
	int pins=6;
	int i;

	for(i=0;i<=pins;i++){
		//getADC(i,&result);
		//printf("Signal %d:%d\n",i,result);
		//printf("Signal %d:%f\n",i,((float)result)/1023.*10);

		getMCPAnalogIn(i,&voltage);
		printf("Signal %d:%f\n",i,voltage);
	}

	return 0;
}

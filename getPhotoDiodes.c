#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "interfacing/kenBoard.h"
#include "interfacing/USB1208.h"
#include "mathTools.h"

#define NUMCHANNELS 3
int main(int argc, char** argv){
	initializeBoard();
	initializeUSB1208();
	int k=0,i=1,nSamples=16;
	float angle;
	float* measurement=calloc(sizeof(float),nSamples);
	float* involts=calloc(sizeof(float),NUMCHANNELS);

	printf("    VERT VERTsd   |    HORIZ HORIZsd  |     REF REFsd    | Approx Angle\n");
	for(k=1;k<NUMCHANNELS+1;k++){
		for (i=0;i<nSamples;i++){
			getUSB1208AnalogIn(k,&measurement[i]);
			involts[k-1]=involts[k-1]+measurement[i];
			delay(10);
		}
		involts[k-1]=fabs(involts[k-1])/(float)nSamples;
		printf("  %0.4f %0.4f  ",involts[k-1],stdDeviation(measurement,nSamples));
		if(k<NUMCHANNELS) printf(" | ");
	}
	printf("\t");
	angle=atan(sqrt(involts[0]/involts[1]));
	printf("%0.3f",angle);
	printf("\n");


	return 0;
}

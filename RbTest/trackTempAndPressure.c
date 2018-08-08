#include "../interfacing/kenBoard.h"
#include "../interfacing/omegaCN7500.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h> //Contains structures for time variables.

#define BUFSIZE 1024

int main(int argc,char* argv[])
{
	char charBuffer[BUFSIZE];
	int rdata;
	int gpioADC=3;
	float returnFloat;
	initializeBoard();

    // Variables for recording the time. 
	time_t rawtime;
	struct tm * timeinfo;


	char tempAndPressureFileName[]= "/home/pi/RbControl/RbTest/tempAndPress.tsv";
	FILE *tempAndPressureFile;
	tempAndPressureFile=fopen(tempAndPressureFileName,"a");
	if (!tempAndPressureFile) {
		printf("Unable to open file: %s\n",tempAndPressureFile);
		exit(1);
	}

	// Get current time info.
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	// Record it in the file.
	strftime(charBuffer,BUFSIZE,"%F",timeinfo);
	fprintf(tempAndPressureFile,"%s\t",charBuffer);
	strftime(charBuffer,BUFSIZE,"%H:%M:%S",timeinfo);
	fprintf(tempAndPressureFile,"%s\t",charBuffer);

	getPVCN7500(CN_CHAMWALL,&returnFloat);
	fprintf(tempAndPressureFile,"%f\t",returnFloat);
	printf("%f\t",returnFloat);

	getADC(gpioADC,&rdata);
	returnFloat = pow(10,(0.004875*rdata - 3.991));
	fprintf(tempAndPressureFile,"%f",returnFloat);
	printf("%f\n",returnFloat);
	fprintf(tempAndPressureFile,"\n",returnFloat);

	return 0;
}

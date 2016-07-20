/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/
#include "interfacing\interfacing.h"

int main (int argc, char* argv[]){
	char dataCollectionFileName[] = "/home/pi/.takingData";
	FILE *dataCollectionFlagFile;

	if (argc==/*Expected number of arguments*/) {
		startvalue=atoi(argv[1]);
		endvalue=atoi(argv[2]);
		stepsize=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else {
		printf("Usage:\n$ sudo ./<programName> <input1> ...\n");
		return 0;
	}

	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");

	if (!dataCollectionFlagFile) {
		printf("Unable to open data collection Flag File\n");
		exit(1);
	}

	initializeBoard();
	initializeUSB1208();

	// Code goes here.

	closeUSB1208();

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0 ;
}

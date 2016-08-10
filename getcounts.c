/*
   program to record excitation function.
   RasPi connected to USB 1208LS.


   PMT Counts: data received from CTR in USB1208

   usage
   $ sudo ./getcounts xx

   Where xx is number of 1 second iterations.  A final sum is also displayed.

   compile
   $ gcc -o getcounts getcounts.c -l wiringPi -l mcchid -L. -l m -L/usr/local/lib -l hid -l usb
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>
#include <wiringPi.h>
#include "interfacing/interfacing.h"


int main (int argc, char *argv[])
{
	int i,numit,dwell;
	long totalcount;
	float counterror;
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	FILE *dataCollectionFlagFile;
	long returnCounts;

	numit = 0;

	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file \n");
		exit(1);
	}

	initializeBoard();
	initializeUSB1208();

	if (argc==3){
		numit = atoi(argv[1]); //number of interations
		dwell = atoi(argv[2]);
	}else{
		printf("Usage:\n\n ~$ sudo ./getcounts xx yyy \n\nwhere xx is the number of yyy-tenth of second iterations to aquire before exiting\n\n");
		return(1);
	}

	totalcount=0;
	for (i=0;i<numit;i++){
		getUSB1208Counter(dwell,&returnCounts);
		totalcount+=returnCounts;
		printf("%d: Counts %ld\n",i,returnCounts);
	}

	printf("Total %ld \n",totalcount);

	counterror = sqrt((float)totalcount);
	printf("SQRT(total) %f \n",counterror);


	closeUSB1208();

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	return 0;
}

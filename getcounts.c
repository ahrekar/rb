/*
   program to record excitation function.
   RasPi connected to USB 1208LS.


   PMT Counts: data received from CTR in USB1208

   usage
   $ sudo ./getcounts xx

   where xx is number of 1 second itterations.  A final sum is also displayed.

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
#include "pmd.h"
#include "usb-1208LS.h"


int main (int argc, char *argv[])
{
	int counts,i,numit;
	int usbReadCountsDelayMs = 654;	// The number of miliseconds it takes to
								// get the counts on the DAQ 
	long totalcount;
	int frequency;		// The number of counts we expected to get
	                    // this is supplied by the user at runtime.
                        // If this is not supplied, the expected
                        // count will be set to the totalcount.
	long expectedCount;	// This is calculated from the frequency and
						// the number of iterations.
	float counterror;	// The square root of the total number of counts
	int extraCounts; 	// This is just (totalCount - expected count); all 
						// the counts we didn't expect to get
	float percentError; // The extraCounts divided by the expectedCount
	//time_t rawtime;
	//struct tm * timeinfo;
	//char buffer [80];
	//float bias, offset, HPcal,energy;
	FILE *fp;
	__s16 sdata[1024];
	__u16 value;
	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0, channel, gain;

	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;

	if (argc==2){
		numit = atoi(argv[1]); //number of interations
		frequency = -1; 	// If the expected count is not specified, 
								// it will later be set to the total count.
								// The value of -1 allows us to identify
								// that it was not set.
	}
	else if (argc==3){
		numit = atoi(argv[1]); //number of interations
		frequency = atoi(argv[2]); //Expected Count number per second
	}
	else{
		numit = 1;
		expectedCount = -1;
	}

	printf("The number of iterations is %d\n", numit);
	printf("The frequency is %d\n", frequency);

	// set up USB interface
	ret = hid_init();
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_init failed with return code %d\n", ret);
		return -1;
	}

	if ((interface = PMD_Find_Interface(&hid, 0, USB1208LS_PID)) < 0) {
		fprintf(stderr, "USB 1208LS not found.\n");
		exit(1);
	} else {
		printf("USB 208LS Device is found! interface = %d\n", interface);
	}


	// config mask 0x01 means all inputs
	usbDConfigPort_USB1208LS(hid, DIO_PORTB, DIO_DIR_IN);
	usbDConfigPort_USB1208LS(hid, DIO_PORTA, DIO_DIR_OUT);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);

	totalcount=0;
	for (i=0;i<numit;i++){
		usbInitCounter_USB1208LS(hid);
		delayMicrosecondsHard(1000000 - usbReadCountsDelayMs); // wiringPi one second delay, taking
															   // into account the time between when
															   // the function is called and when
															   // the counts are actually read. 
		counts=usbReadCounter_USB1208LS(hid);
		totalcount+=counts;
		printf("%d: Counts %d\n",i,counts);
		fflush(stdout);
	}


//	printf("Total %d \n",totalcount);
//	counterror = sqrt((float)totalcount);
//	printf("SQRT(total) %f \n",counterror);

	if (expectedCount< 0){
		expectedCount = totalcount;
	}else{
		expectedCount = frequency*numit;
		printf("Expected Count: %ld\n",expectedCount);
	}

	counterror = sqrt((float)totalcount);
	extraCounts = totalcount - expectedCount;
	percentError = (float)extraCounts/ (float)expectedCount;


	printf("Results:\tFreq.\tIter.\tExpect\tTotal\tSQRT(total)\t%%error\n");
	printf("        \t%d\t%5d\t%ld\t%ld\t%f\t%f\n",frequency,numit,expectedCount,totalcount,counterror,percentError);

	//cleanly close USB
	ret = hid_close(hid);
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_close failed with return code %d\n", ret);
		return 1;
	}

	hid_delete_HIDInterface(&hid);
	ret = hid_cleanup();
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_cleanup failed with return code %d\n", ret);
		return 1;
	}
	return 0;


}





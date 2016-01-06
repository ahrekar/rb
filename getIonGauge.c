/*
   RasPi connected to USB 1208LS.

   Prints out the current reading on the Ion gauge. 
*/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <asm/types.h>
#include <wiringPi.h>
#include "pmd.h"
#include "usb-1208LS.h"

int closeUSB(HIDInterface* hid);

int main (int argc, char **argv)
{
	int num, i, returnValue;
	signed short svalue;
	float involts;
	__s16 sdata[1024];
	__u16 value;
	//	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0, channel, gain;

	// set up USB interface
	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;

	// Check to make sure USB DAQ is properly connected
	ret = hid_init();
	if (ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_init failed with return code %d\n", ret);
		return -1;
	}

	if ((interface = PMD_Find_Interface(&hid, 0, USB1208LS_PID)) < 0) {
		fprintf(stderr, "USB 1208LS not found.\n");
		exit(1);
	} else {
//		printf("USB 208LS Device is found! interface = %d\n", interface);
	}



	channel = 1;// analog input for iongauge
	gain=BP_10_00V;

		involts=0.0;
		num=8;

		for (i=0;i<num;i++){
			svalue=usbAIn_USB1208LS(hid,channel,gain);
			involts=involts+volts_LS(gain,svalue);
		}

		involts=involts/(float)num;

//		involts = 1.8 - involts;
		involts = pow(10,(involts-9.97));
		printf("IonGauge %2.2E \n",involts);

	//cleanly close USB
	returnValue = closeUSB(hid);
	return returnValue;
}


int closeUSB(HIDInterface* hid){
	hid_return ret;
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

/*


*/

#include <wiringPi.h>
#include "USB1208.h"

void delayMicrosecondsHard(unsigned int howLong);

//global variables for USB1208 but not for public
	HIDInterface*  hid = 0x0;
	hid_return ret;
	int interface;


int closeUSB1208(){
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

int initializeUSB1208(){
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
		printf("USB 1208LS Device is found! interface = %d\n", interface);
	}
return 0;
}

int getUSB1208AnalogIn(unsigned short chan, float* returnvalue){
	signed short svalue;
	int gain = BP_10_00V;

	svalue = usbAIn_USB1208LS(hid,chan,gain);  //channel = 0 for k617

	*returnvalue = volts_LS(gain,svalue);

return 0;
}

// dwell is time in tenth's of seconds.  So, if want to count for 1 sec, pass '10'
int getUSB1208Counter(unsigned short dwell, long int * returncounts){
	int counts,i;
	counts=0;
	for (i=0;i<dwell;i++){ 
		/* the for loop is used instead of putting dwell in the delayMicroseconds.
		In high count rates (not likely for us, but could be) the USB counter could 'roll over'
		during a very long dwell time.  So, by doing smaller chunks and adding, less 
		chance of roll over. 
		*/
		usbInitCounter_USB1208LS(hid);
		delayMicrosecondsHard(100000); // wiringPi
		counts+=usbReadCounter_USB1208LS(hid);
	}
	*returncounts=counts;
return 0;
}

int setUSB1208AnalogOut(unsigned short chan, unsigned int outValue){
	// out value ranges from 0 to 1024
	outValue = outValue & 0x3FF;
	// this caps at 1023. 10 bits.
	usbAOut_USB1208LS(hid, chan, outValue);
return 0;
}



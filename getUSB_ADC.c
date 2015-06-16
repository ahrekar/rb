/*
reads analog input from

RasPi connected to USB 1208LS.


PMT Counts: data received from CTR in USB1208

usage


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
	signed short svalue;
 

 //time_t rawtime;
// struct tm * timeinfo;
 //char buffer [80];
// float bias, offset, HPcal,energy;
// FILE *fp;
  __s16 sdata[1024];
  __u16 value;
  __u16 count;
  __u8 gains[8];
  __u8 options;
  __u8 input, pin = 0, channel, gain;

  HIDInterface*  hid = 0x0;
  hid_return ret;
  int interface;
numit = 0;

if (argc==2){
	numit = atoi(argv[1]); //number of interations
	}else{
	printf("Usage:\n\n ~$ sudo ./getcounts xx \n\nwhere xx is the number of itterations to aquire before exiting\n\n");
	return(1);
	}


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


for (i=0;i<numit;i++){
	channel = 0;
	gain = BP_5_00V;
	svalue=usbAIn_USB1208LS(hid,channel,gain);
	
	printf("value %d, float %f \n",svalue,volts_LS(gain,svalue));
	fflush(stdout);
}


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
  





/*
program to set analog output

RasPi connected to USB 1208LS.

Sets analog voltage for probe laser. Uses Analog out port 0. Final output to probe laser is through 
op-amp circuit. see page 98.  Page 99 shows calibration data.

Usage '$ sudo ./setProbeLaser xxx' where xxx is an integer value between 0 and 1024
 */

#include <stdlib.h>
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


int main (int argc, char *argv[])
{
 int counts,i;
 char buffer [80];
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


if (argc=2) {
	value =atoi(argv[1]);
	}else{
	printf("Usage '$ sudo ./setProbeLaser xxx' where xxx is an integer value between 0 and 1024\n");
	value=0;
	}

if (value<0) value=0;

if (value>1023) value=1023;

usbAOut_USB1208LS(hid, 0, value);
printf("Aout %d \n",value);
fflush(stdout);


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
  




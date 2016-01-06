
/*
Program to tell the servo to rotate so that it will block the laser. 
This is mainly example code so that you can see how to implement it
in a different program that you write.

RasPi connected to USB 1208LS.
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
 int i;
 char buffer [80];

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
//    printf("USB 208LS Device is found! interface = %d\n", interface);
  }


  // config mask 0x01 means all inputs
  usbDConfigPort_USB1208LS(hid, DIO_PORTB, DIO_DIR_IN);
  usbDConfigPort_USB1208LS(hid, DIO_PORTA, DIO_DIR_OUT);  //Port A is an output

//  usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);
//  usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);


// get file name.  use format "EX"+$DATE+$TIME+".dat"

if (argc=2) {
	value =atoi(argv[1]);
	}else{
	value = 0;
	}

value = value & 0x03;
//printf("value %d \n",value);

usbDOut_USB1208LS(hid,DIO_PORTA,value);

// delay to allow transients to settle

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

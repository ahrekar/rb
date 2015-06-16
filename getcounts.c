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
#include "pmd.h"
#include "usb-1208LS.h"


int main (int argc, char *argv[])
{
 int counts,i,numit;
 long totalcount;
 float counterror;
 //time_t rawtime;
// struct tm * timeinfo;
 //char buffer [80];
// float bias, offset, HPcal,energy;
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
numit = 0;

if (argc==2){
	numit = atoi(argv[1]); //number of interations
	}else{
	printf("Usage:\n\n ~$ sudo ./getcounts xx \n\nwhere xx is the number of one-second iterations to aquire before exiting\n\n");
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

totalcount=0;
for (i=0;i<numit;i++){
	usbInitCounter_USB1208LS(hid);
	delayMicrosecondsHard(1000000); // wiringPi one second delay
	counts=usbReadCounter_USB1208LS(hid);
	totalcount+=counts;
	printf("%d: Counts %d\n",i,counts);
	fflush(stdout);
}

printf("Total %d \n",totalcount);

counterror = sqrt((float)totalcount);
printf("SQRT(total) %f \n",counterror);

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
  




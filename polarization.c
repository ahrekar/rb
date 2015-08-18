/*
   Program to record polarizqtion.
   RasPi connected to USB 1208LS.
   Target energy: USB1208LS Analog out Ch1 controls HP3617A. See pg 31 my lab book
   PMT Counts: data received from CTR in USB1208
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

#define CLK 0
#define DIR 1

int setUpUSB(HIDInterface* hid, hid_return* ret);
int closeUSB(HIDInterface* hid, hid_return* ret);

int main (int argc, char **argv)
{
	int counts,i,steps,nsteps,ninc,dwell,aout;
	time_t rawtime;
	struct tm * timeinfo;
	signed short svalue;
	char buffer[80],comments[80];
	float bias, offset, HPcal,energy, current;
	FILE *fp;
	__s16 sdata[1024];
	__u16 value;
	__u16 count;
	__u8 gains[8];
	__u8 options;
	__u8 input, pin = 0, channel, gain;

	HIDInterface*  hid = 0x0;
	hid_return* ret;
	int interface;

	// get parameters
	if (argc==6){
		nsteps=1200*atoi(argv[1]);
		ninc=atoi(argv[2]);
		dwell=atoi(argv[3]);
		aout=atoi(argv[4]);
		strcpy(comments,argv[5]);
	} else {
		printf("usage '~$ sudo ./polarization <num_rotations> <step_size> <pmt_dwell> <aout_for_target> <comments_in_double_quotes>'\n");
		printf("                                                          (in seconds)                  (Specify nominal electron) '\n");
		printf("                                                                                        (energy.                 ) '\n");
		return 1;
	}


	// set up USB interface
	if(!setUpUSB(hid,ret)){ // if setUpUSB was not completed successfully, end the program.
		return 1;
	}
	printf("usb found!\n");


	// config mask 0x01 means all inputs
	usbDConfigPort_USB1208LS(hid, DIO_PORTB, DIO_DIR_IN);
	usbDConfigPort_USB1208LS(hid, DIO_PORTA, DIO_DIR_OUT);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);
	usbDOut_USB1208LS(hid, DIO_PORTA, 0x0);
	printf("usb config complete!\n");

	// set up for stepmotor
	wiringPiSetup();
	pinMode(CLK,OUTPUT);
	pinMode(DIR,OUTPUT);
	digitalWrite(DIR,1);


	// get file name.  use format "EX"+$DATE+$TIME+".dat"
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(buffer,80,"/home/pi/RbData/POL%F_%H%M%S.dat",timeinfo);
	printf("Generated filename\n");

	printf("\n");
	printf("%s\n",buffer);


	fp=fopen(buffer,"w");
	if (!fp) {
		printf("unable to open file \n");
		exit(1);
	}
	fprintf(fp,"#%s\n",buffer);

	// RUDAMENTARIY ERROR CHECKING
	if (nsteps<1200) nsteps=1200;
	if (ninc<1) ninc=1;
	if (ninc>nsteps) ninc=nsteps/2;
	if (dwell<1) dwell=1;
	if (aout<0) aout=0;
	if (aout>1023) aout=1023;

	HPcal=28.1/960.0;
	fprintf(fp,"#nsteps %d\n",nsteps);
	fprintf(fp,"#Aout %d\n",aout);
	fprintf(fp,"#%s\n",comments); // Note: if there is an error in processing comments, check this line.
	fprintf(fp,"#Assumed USB1208->HP3617A converstion %2.6f\n",HPcal);

	//Write Aout for He target here (Set the voltage applied to the laser to acheive the desired wavelength)
	usbAOut_USB1208LS(hid,1,aout);

	//NOTE THAT THIS SETS THE FINAL ELECTRON ENERGY. THIS ALSO DEPENDS ON BIAS AND TARGET OFFSET.  AN EXCIATION FN WILL TELL THE
	//USER WHAT OUT TO USE, OR JUST MANUALLY SET THE TARGET OFFSET FOR THE DESIRED ENERGY

	// Write the header for the data to the file.
	fprintf(fp,"\n");	 // Leave a blank line to indicate a new section of the document
	fprintf(fp,"steps\tCounts\tCurrent\n");
	gain=BP_5_00V;
	channel = 0; // analog input for k617 ammeter

	digitalWrite(CLK,LOW);
	delayMicrosecondsHard(2000);

	for (steps=0;steps<nsteps;steps+=ninc){

		//200 steps per revoluion

		for (i=0;i<ninc;i++){
		// increment steppermotor by ninc steps
		digitalWrite(CLK,HIGH);
		delayMicrosecondsHard(2300);
		digitalWrite(CLK,LOW);
		delayMicrosecondsHard(2300);
		}

		printf("steps %d\t",(steps));
		fprintf(fp,"%d\t",(steps));

		counts=0;
		for (i=0;i<dwell;i++){
			usbInitCounter_USB1208LS(hid);
			delayMicrosecondsHard(1000000); // wiringPi
			counts+=usbReadCounter_USB1208LS(hid);
		}

		current=0.0;
		for (i=0;i<8;i++){
			svalue = usbAIn_USB1208LS(hid,channel,gain);
			current = current+volts_LS(gain,svalue);
		}
		current = current/8.0;

		printf("Counts %d\t",counts);
		fflush(stdout);
		fprintf(fp,"%d \t",counts);

		printf("current %f\n",current);
		fflush(stdout);
		fprintf(fp,"%f \n",current);

	}


	fclose(fp);

	//cleanly close USB if it fails, indicate the error.
	return closeUSB(hid,ret);
}

int setUpUSB(HIDInterface* hid, hid_return* ret){
	printf("SetUpUSB Successfully called!\n");
	int interface;
	*ret = hid_init();
	if (*ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_init failed with return code %d\n", *ret);
		printf("hid_init failed with return code %d\n", *ret);
		fflush(stdout);
		return 0;
	}
	printf("First if statement passed!\n");

	if ((interface = PMD_Find_Interface(&hid, 0, USB1208LS_PID)) < 0) {
		fprintf(stderr, "USB 1208LS not found.\n");
		printf("USB 1208LS not found.\n");
		exit(1);
	} else {
		printf("USB 1208LS Device is found! interface = %d\n", interface);
		return 1;
	}
}

int closeUSB(HIDInterface* hid, hid_return* ret){
	*ret = hid_close(hid);

	if (*ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_close failed with return code %d\n", *ret);
		return 1;
	}

	hid_delete_HIDInterface(&hid);
	*ret = hid_cleanup();
	if (*ret != HID_RET_SUCCESS) {
		fprintf(stderr, "hid_cleanup failed with return code %d\n", *ret);
		return 1;
	}
	return 0;
}

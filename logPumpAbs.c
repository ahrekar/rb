/*

   uses MCP3004 chip attached to SPI channel 0 of RasPi GPIO
   to retrieve information about the environment around
   the raspberryPi

   this chip has 8 analog inputs, chan 0 ... 7
   returns value 0 to 1023

   Cnannel descriptions present board:
   Ch0 = 
   Ch1 = IonGauge (0-10V)
   Ch2 = 

   Ch3 = ConvectronGauge
   Ch4 =
   Ch5 =
   Ch6 =
   Ch7 = Analog in Pin 5 (0 to 5v = 0 to 1023)

   Analog in Pin 0 = GND
   */


#include <stdio.h>
#include <wiringPi.h>
#include <mcp3004.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <asm/types.h>
#include "pmd.h"
#include "usb-1208LS.h"
#include "mathTools.h"

#define BASE 100
#define SPI_CHAN 0
#define BUFSIZE 1024
#define NSAMPLES 24
#define DELAYTIME 50
#define HOURSINDAY 24
#define MINUTESINHOUR 60
#define LINECENTER 377.107463380

int main (int argc, char** argv){
	char* fileName = "/home/pi/RbData/2016-07-06/pumpAbsorption.dat";
	char dataCollectionFileName[] = "/home/pi/.takingData"; 
	FILE* dataFile;
	FILE* dataCollectionFlagFile;
	signed short svalue;
	int i;
	float averageValue,temperature,frequency;

	// Indicate that data is being collected.
	dataCollectionFlagFile=fopen(dataCollectionFileName,"w");
	if (!dataCollectionFlagFile) {
		printf("unable to open file \n");
		exit(1);
	}

	int interface;
	HIDInterface*  hid = 0x0;
	__u8  gain;
	gain = BP_5_00V;
	hid_return ret;

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


	if (argc==3){
		temperature=atof(argv[1]);
		frequency=atof(argv[2]);
	} else { 
		printf("usage '~$ sudo ./logPumpAbs <temperatureControlVal> <frequency>'\n");
		return 1;
	}

	dataFile = fopen(fileName,"a");
	if(!dataFile){
		fprintf(stderr,"There was an error opening the file.\n");
		exit(1);
	}

	wiringPiSetup();
	mcp3004Setup(BASE,SPI_CHAN);


//	fprintf(dataFile,"Laser Control Temp(C)\tDetuning (87 D1) from Wavemeter (GHz)\tTransmission(V)\tTransStdDev.(V)\n");

	float* measurement = calloc(NSAMPLES,sizeof(float));

	averageValue=0;
	for(i=0;i<NSAMPLES;i++){
		svalue = usbAIn_USB1208LS(hid,1,gain);  //channel 1 for pump
		measurement[i] = volts_LS(gain,svalue);
		averageValue += measurement[i] / NSAMPLES;
		delay(DELAYTIME);
	}
	fprintf(dataFile,"%3.8f\t%3.8f\t%3.8f\t%3.8f\n",temperature,(frequency-LINECENTER)*1000,averageValue,stdDeviation(measurement,NSAMPLES));
	printf("%3.8f\t%3.8f\t%3.8f\t%3.8f\n",temperature,(frequency-LINECENTER)*1000,averageValue,stdDeviation(measurement,NSAMPLES));

	fclose(dataFile);

	fclose(dataCollectionFlagFile);
	remove(dataCollectionFileName);

	free(measurement);
	return 0;
}

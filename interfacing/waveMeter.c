#include "waveMeter.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "wiringPi.h"

float getWaveMeter(void){

	char buffer[16];
	float waves;
	
	FILE* program;

	waves=7800000;

    //printf("Running the remote program...\n");
	program = popen("ssh -i /home/pi/.ssh/id_rsa pi@irpi 'sudo /home/pi/rbExp/conv232to485/getWaveMeterWavelength'","w");
	pclose(program);
    //printf("Remote program ran!\n");

	delay(100);
	FILE* wavelengthFile;
	wavelengthFile=fopen("/home/pi/share/irpi/.wavelength","r");

	if(!wavelengthFile) {
		// unable to open. 
        printf("Error: Unable to open wavelength file!\n");
	} else {
        //while(waves<7900000 && tries > 0){
            //printf("Tries remaining: %d\n",tries);
            fgets(buffer,8,wavelengthFile);
            waves=atof(buffer);
            //printf("Value returned: %f\n",waves);
            delay(1000);
        //}
	}
	//printf("FROM THE FILE: %f",wavelength/2.99000000);
	fclose(wavelengthFile);

    return waves;

}

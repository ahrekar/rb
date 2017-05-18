

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "wiringPi.h"

float getWaveMeter(void);




/*

int main (int argc, char **argv)
{
	char buffer[16];
	float wavelength;
	
	FILE* program;

	program = popen("ssh pi@irpi 'sudo /home/pi/karlCode/uart/getWaveMeterWavelength'","w");
	fclose(program);

	FILE* wavelengthFile;
	wavelengthFile=fopen("/home/pi/irpi/.wavelength","r");
	fgets(buffer,8,wavelengthFile);
	wavelength=atof(buffer);
	printf("FROM THE FILE: %f",wavelength/2.99000000);
	fclose(wavelengthFile);
}
*/

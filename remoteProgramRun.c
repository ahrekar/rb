
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int main (int argc, char **argv)
{
	char buffer[16];
	float wavelength;
	
	FILE* program;

	program = popen("ssh pi@irpi 'sudo /home/pi/rbExp/conv232to485/getWaveMeterWavelength'","w");
	fclose(program);

	FILE* wavelengthFile;
	wavelengthFile=fopen("/home/pi/irpi/.wavelength","r");
	fgets(buffer,8,wavelengthFile);
	wavelength=atof(buffer);
	printf("FROM THE FILE: %f",wavelength/2.99000000);
	fclose(wavelengthFile);
}

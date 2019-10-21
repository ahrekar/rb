/*
 * stepmotor.c
********************************************************************
GPIO 4 output -> clock source for stepper motor.  one clock transistion = 0->1 == one step
GPIO 2 output -> binary direction (1=up, 0=down)

compile
~ $ gcc -o setPlaneAngle setPlaneAngle.c -l wiringPi

execute
~ $ sudo ./setPlaneAngle angleinmrad

where steps is integer number of steps and dir is 0 or 1

 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include "interfacing/interfacing.h"
#include "fileTools.h"

int main (int argc, char *argv[]) {

	wiringPiSetup();
	int newpos,i;
    int line=0;

    char wavePlatePositionFileName[]="/home/pi/RbControl/system.cfg";
    char buffer[1024];
    FILE *wavePlatePositionFile;

	if (argc ==2) {
		newpos = atoi(argv[1]);
	} else {
		printf("Usage: sudo ./setWavePlate <step location>\n");
		return 1;
	}
	setMotor(2,newpos);

    line=getLineNumberForComment(wavePlatePositionFileName,"#PumpQWPAngle(step):");
    if(line==-1){
        printf("Error! Not able to find wave Plate position in file\n");
        return -1;
    }else{
        wavePlatePositionFile=fopen(wavePlatePositionFileName,"r+");
        if(!wavePlatePositionFile){
            printf("Unable to open wavePlatePositionFile\n");
            exit(1);
        }

        for(i=0;i<line;i++){
            fgets(buffer,1024,wavePlatePositionFile);
        }
        fprintf(wavePlatePositionFile,"#PumpQWPAngle(step):\t%03d\n",newpos);
        fclose(wavePlatePositionFile);
        return 0;
    }
}

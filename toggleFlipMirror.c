/*
   Program to tell the servo to rotate so that it will block the laser. 
   This is mainly example code so that you can see how to implement it
   in a different program that you write.

*/

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include "interfacing/flipMirror.h"
#include "interfacing/kenBoard.h"
#include "interfacing/RS485Devices.h"

int main (int argc, char *argv[])
{
    unsigned short value,device;

    if (argc==1) {
        device=0;
    }else{
        printf("Usage ./toggleFlipMirror\n");
        return 1;
    }
	initializeBoard();
    delay(300);

    switch (device) {
        case 1:
            break;
        case 0:
			value=getMirror();
            if(value>0){value=0;}else{value=8;}
			setMirror(value);
            delay(300);
			value=getMirror();
            if(value>0){printf("Wavemeter Measuring Pump Laser\n");}else{printf("Wavemeter Measuring Probe Laser\n");}
            break;
        default:
            printf("invalid device number\n");
            break;
    }

    return 0;
}

/*
   Program to tell the servo to rotate so that it will block the laser. 
   This is mainly example code so that you can see how to implement it
   in a different program that you write.

*/

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include "interfacing/laserFlag.h"
#include "interfacing/kenBoard.h"

int main (int argc, char *argv[])
{
    unsigned short value,device;

    if (argc==2) {
        device = atoi(argv[1]);
    }else{

        printf("Usage ./toggleLaserFlag <device 0,1>\n");
        printf("Switches the blocked state for the\n");
        printf("unblocked and vice-versa.\n");
        printf("Device\n");
        printf("======\n");
        printf("Probe = 0\n");
        printf("Pump = 1\n");
        return 1;
    }
	initializeBoard();
    delay(300);

    switch (device) {
        case 1:
			value=getFlag(PUMPFLAG);
            if(value>0){value=0;}else{value=8;}
			setFlag(PUMPFLAG,value);
            delay(300);
			value=getFlag(PUMPFLAG);
            if(value>0){printf("Pump Laser blocked\n");}else{printf("Pump Laser unblocked\n");}
            break;
        case 0:
			value=getFlag(PROBEFLAG);
            if(value>0){value=0;}else{value=8;}
			setFlag(PROBEFLAG,value);
            delay(300);
			value=getFlag(PROBEFLAG);
            if(value>0){printf("Probe Laser blocked\n");}else{printf("Probe Laser unblocked\n");}
            break;
        default:
            printf("invalid device number\n");
            break;
    }

    return 0;
}

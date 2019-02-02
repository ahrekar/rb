/*
   Program to tell the servo to rotate so that it will block the laser. 
   This is mainly example code so that you can see how to implement it
   in a different program that you write.

*/

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include "interfacing/kenBoard.h"
#include "interfacing/laserFlag.h"

int main (int argc, char *argv[])
{
    unsigned short value,device;

    if (argc==3) {
        device = atoi(argv[1]);
        value = atoi(argv[2]);
    }else{

        printf("Usage ./setLaserFlag <device 0,1> <Orientation 0,1>\n");
        printf("Device\t\t\tOrientation\n");
        printf("======\t\t\t===========\n");
        printf("Probe = 0\t\tUnblocked = 0\n");
        printf("Pump = 1\t\tBlocked = 1\n");
        return 1;
    }
	if(value>0){value=BLOCKED;}else{value=UNBLOCKED;}

	initializeBoard();

    switch (device) {
        case 1:
			setFlag(PUMPFLAG,value);
			switch (value){
				case BLOCKED:
					printf("pump laser blocked\n");
					break;
				case UNBLOCKED:
					printf("pump laser unblocked\n");
					break;
			}
            break;
        case 0:
			setFlag(PROBEFLAG,value);
			switch (value){
				case BLOCKED:
					printf("probe laser blocked\n");
					break;
				case UNBLOCKED:
					printf("probe laser unblocked\n");
					break;
			}
            break;
        default:
            printf("invalid device number\n");
            break;
    }

    return 0;
}

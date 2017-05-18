/*
   Program to tell the servo to rotate so that it will block the laser. 
   This is mainly example code so that you can see how to implement it
   in a different program that you write.

*/

#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#define AA 23
#define BB 24

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

    wiringPiSetup();

    pinMode(AA,OUTPUT);
    pinMode(BB,OUTPUT);

    switch (device) {
        case 1:
            digitalWrite(AA, (value & 0b0001));
            break;
        case 0:
            digitalWrite(BB, (value & 0b0001));
            break;
        default:
            printf("invalid device number\n");
            break;
    }


    //digitalWrite(BB, ((value & 0b0010) >> 1) );

    return 0;
}

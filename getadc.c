/*

uses MCP3004 chip attached to SPI channel 0 of RasPi GPIO


this chip has 8 analog inputs, chan 0 ... 7
returns value 0 to 1023

Cnannel descriptions present board:
Ch0 = atmopheric pressure  (450=1Atm)
Ch1 = temperature (calibration TBD)
Ch2 = relative humidity (calibration TBD)

Ch3 = Analog In Pin 1 (0 to 5 v = 0 to 1023)
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

#define BASE 100
#define SPI_CHAN 0

int main (void){
int x;
int chan;

wiringPiSetup();
mcp3004Setup(BASE,SPI_CHAN);
printf("Monitor ADC \n");
	while (1==1){
	for (chan=0;chan<8;chan++){
		x=analogRead(BASE + chan);
		printf("%d \t", x);
	}
	printf("\n");
	delay(500);

	}


return 0;
}

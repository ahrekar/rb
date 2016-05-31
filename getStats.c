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
#define BASE 100
#define SPI_CHAN 0

int main (void){
int x;
int chan;
float CVGauge,IonGauge; // Rb target CV Gauge (buffer pressure)

wiringPiSetup();
mcp3004Setup(BASE,SPI_CHAN);

printf("Rubidium Pi Conditions \n");
printf("====================================\n");


chan = 1; // IonGauge
x=analogRead(BASE + chan);
IonGauge = 0.0107 * (float)x;
IonGauge = pow(10,(IonGauge-9.97));
printf("IonGauge %2.2E Torr \n",IonGauge);

chan = 3; //Nitrogen CV pressure
x=analogRead(BASE + chan);
CVGauge = (float)x;
CVGauge = pow(10,(0.00499*CVGauge - 4.05));
printf("N2 CVGauge %2.2E Torr\n", CVGauge);

chan = 4; //Helium CV pressure
x=analogRead(BASE + chan);
CVGauge = (float)x;
CVGauge = pow(10,(0.00499*CVGauge - 4.05));
printf("HE CVGauge %2.2E Torr\n", CVGauge);




return 0;
}

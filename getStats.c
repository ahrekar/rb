/*

uses MCP3004 chip attached to SPI channel 0 of RasPi GPIO
to retrieve information about the environment around
the raspberryPi

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
#include <math.h>
#define BASE 100
#define SPI_CHAN 0

int main (void){
int x;
int chan;
float pressure, temperature, humidity;
float CVGauge; // Rb target CV Gauge (buffer pressure)

wiringPiSetup();
mcp3004Setup(BASE,SPI_CHAN);

printf("Rubidium Pi Environmental Conditions \n");
printf("====================================\n");

chan = 0; //pressure
x=analogRead(BASE + chan);
pressure = (float)x *720.0/600.0;  // this is an ARBITRAYR scale.  needs to be calibrated.
printf("Air Pressure %f Torr\n", pressure);

chan = 1; // temperature
// sensor is an AD 22100
x=analogRead(BASE + chan);  //x  returns a number between 0 and 1023 corresponding to a voltage between 0 and 5V
temperature = ((float)x*5/1023.0 - 1.375)/0.0225;  //this formula is taken from the ADS22100 data sheet. 
printf("Air Temperature %f C\n",temperature);

chan = 2; //relative humidity
// sensor is a HIH-5030
//output is linear from 0.5v=0% RH to 2.5v=100%RH
x=analogRead(BASE + chan);  //x  returns a number between 0 and 1023 corresponding to a voltage between 0 and 5V
humidity=  ((float)x*3.3/1023.0 - 0.5)*50.0;
printf("Air Relative Humidity %f %\n",humidity);

chan = 3; //pressure
x=analogRead(BASE + chan);
CVGauge = (float)x;
CVGauge = pow(10,(0.00499*CVGauge - 4.05));
printf("CVGauge %2.2E Torr\n", CVGauge);


return 0;
}

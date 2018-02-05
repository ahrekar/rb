/*

test bridge 


simple function .  testing on an Epson Projector.


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kenBoard.h"




int main (int argc, char* argv[]){
int i,j;
char outData[32];
char rtnData[32];


	initialize_rs485(9600,6); // this is raspi specific. May need to change the gpio pin number
	//9600 is the default for most equipment


	strcpy(outData,"PWR OFF");
		i=strlen(outData);
		outData[i]=13;
		outData[i+1]=0; 
			 // the device i was trying this on needed a CR at the end. modify as needed
			// because we appended the CR

		writeRS232Bridge(outData,rtnData,0xC0);

printf("\n");
printf(rtnData);

printf("\n");

  return 0 ;
}

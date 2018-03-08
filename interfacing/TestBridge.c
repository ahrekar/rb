/*



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


if (argc==3){

initializeBoard();
//	initialize_rs485(9600,6); // this is raspi specific. May need to change the gpio pin number
	//9600 is the default for most equipment
	j=atoi(argv[2]);

	strcpy(outData,argv[1]);
		i=strlen(outData);
		outData[i]=13;
		outData[i+1]=0; 
			 // the device i was trying this on needed a CR at the end. modify as needed
			// because we appended the CR

		writeRS232Bridge(outData,rtnData,j);

printf("\n");
printf("\n");
printf(rtnData);
printf("\n");
printf("\n");
//strip non printable ascii
i=0;
j=0;

while(rtnData[i]!=0){
	if(rtnData[i]>31){
	outData[j]=rtnData[i];
	j++;
	i++;
	} else {
	i++;
	}
}
outData[j]=0;
printf(outData);


printf("\n");
}
  return 0 ;
}

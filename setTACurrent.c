/*
	Set TA Current
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interfacing/topticaLaser.h"
#include "interfacing/kenBoard.h"
#include "fileTools.h"


int main (int argc, char* argv[]){
	int i;
	float current, presentValue;
	int line=0;
	int laserSocket;
	char systemCfgFileName[]="/home/pi/RbControl/system.cfg";
    char buffer[1024];
    FILE *systemCfgFile;

	initializeBoard();
	laserSocket=initializeLaser();


	if (argc==2){
		current=atof(argv[1]);
	}else {
		return 1;
	}

	presentValue = getAmpCurrent(laserSocket);
	printf("Present Value: %4.2f\n", presentValue);

	setAmpCurrent(laserSocket,current);

    line=getLineNumberForComment(systemCfgFileName,"#PumpLaserAMPCurrent(mA)");

    systemCfgFile=fopen(systemCfgFileName,"r+");
    if(!systemCfgFile){
        printf("Unable to open systemCfgFile\n");
        exit(1);
    }

    for(i=0;i<line;i++){
        fgets(buffer,1024,systemCfgFile);
    }
    fprintf(systemCfgFile,"#PumpLaserAMPCurrent(mA):\t%4.2f\n",current);

    fclose(systemCfgFile);

	close(laserSocket);

	return 0;
}

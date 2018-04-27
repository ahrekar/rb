/*
	Set TA Current
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interfacing/sacherLaser.h"
#include "interfacing/kenBoard.h"
#include "fileTools.h"


int main (int argc, char* argv[]){
	int i;
	int current;
	int line=0;
	char systemCfgFileName[]="/home/pi/RbControl/system.cfg";
    char buffer[1024];
    FILE *systemCfgFile;

	initializeBoard();
	initializeTA();


	if (argc==2){
		current=atof(argv[1]);
	}else {
		return 1;
	}
	printf("INPUT VALUE:%d\n",current);
	printf("TA CURRENT VALUE:%f\n",1000*getTACurrent());
	setTACurrent(current);

    line=getLineNumberForComment(systemCfgFileName,"#PumpLaserTA");

    systemCfgFile=fopen(systemCfgFileName,"r+");
    if(!systemCfgFile){
        printf("Unable to open systemCfgFile\n");
        exit(1);
    }

    for(i=0;i<line;i++){
        fgets(buffer,1024,systemCfgFile);
    }
    fprintf(systemCfgFile,"#PumpLaserTACurrent(mA):\t%4d\n",current);

    fclose(systemCfgFile);

	return 0;
}

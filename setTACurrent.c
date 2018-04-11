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
	int current,i;
	int line=0;
	char systemCfgFileName[]="/home/pi/RbControl/system.cfg";
    char* taCurrentString;
    char buffer[1024];
    FILE *systemCfgFile;

	initializeBoard();
	initializeTA();


	if (argc==2){
		current=atoi(argv[1]);
	}else {
		return 1;
	}
	setTACurrent(current);

    line=getLineNumberForComment(systemCfgFileName,"#PumpLaserTA",taCurrentString);

    systemCfgFile=fopen(systemCfgFileName,"r+");
    if(!systemCfgFile){
        printf("Unable to open systemCfgFile\n");
        exit(1);
    }

    for(i=0;i<line;i++){
        fgets(buffer,1024,systemCfgFile);
    }
    fprintf(systemCfgFile,"#PumpLaserTACurrent(mA):\t%04d\n",current);

    fclose(systemCfgFile);

	return 0;
}

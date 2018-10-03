/*
 */

#include <stdio.h>
#include <string.h>
#include "kenBoard.h"
#include "RS485Devices.h"
#include "keithley.h"
#define KEITHLY6485 0xC6

int main(int argc,char* argv[])
{
	char command[64];
	char echoData[128];
	
	int i;  
	
	initializeBoard();
	printf("Keithley Return: %f\n",readKeithley());
	
	return 0;
}


/*
 */

#include <stdio.h>
#include <string.h>
#include "kenBoard.h"
#include "RS485Devices.h"
#define KEITHLY6485 0XCA

int main(int argc,char* argv[])
{
	
	char command[64];
	char echoData[128];
	
	int i;  


         
// open and configure the serial port
initializeBoard();

	strcpy(command,"*RST");
	printf("Sending %s\n",command);
	i=strlen(command);
	
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	printf("Return: %s\n",echoData);

	strcpy(command,"FORM:ELEM READ");
	printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);

	printf("Return: %s\n",echoData);

	strcpy(command,"SYST:ZCH ON");
	printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	printf("Return: %s\n",echoData);
	
	strcpy(command,"CURR:RANG 2e-9");
	printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	printf("Return: %s\n",echoData);

	strcpy(command,"INIT");
	printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	printf("Return: %s\n",echoData);

	strcpy(command,"SYST:ZCOR:ACQ");
	printf("Sending %s\n",command);
	i=strlen(command);
	printf("Return: %s\n",echoData);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);

	strcpy(command,"SYST:ZCOR OFF");
	printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	printf("Return: %s\n",echoData);

	strcpy(command,"SYST:ZCH OFF");
	printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	printf("Return: %s\n",echoData);

	strcpy(command,"CURR:RANG:AUTO ON");
	printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	printf("Return: %s\n",echoData);

	strcpy(command,"READ?");
	printf("Sending %s\n",command);
	i=strlen(command);
	writeRS485to232Bridge(command,echoData,KEITHLY6485);
	printf("Return: %s\n",echoData);


 
	

// exit normally
    return 0;
}

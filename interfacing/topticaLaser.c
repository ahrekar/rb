#include <stdio.h>
#include <stdlib.h>
#include <time.h> // For delay()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h> // Added this for the definition of SOL_TCP to set NO_DELAY
#include <arpa/inet.h> // inet_pton()
#include <string.h>
#include "topticaLaser.h"
#define PORT 1998
#define BUFLEN 4096
#define LASERIP "129.93.33.78"

int initializeLaser(){
	int sock=0;
	struct sockaddr_in serv_addr;
	char returnBuffer[BUFLEN];
	if ((sock = socket(AF_INET, SOCK_STREAM,0)) < 0){
		printf("\n Socket creation error \n");
		return -1;
	}
	
	memset(&serv_addr,'0',sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	//Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET,LASERIP,&serv_addr.sin_addr)<=0){
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
		printf("\nConnection Failed \n");
		return -1;
	}
	// Added this to attempt to satisfy the setting of the socket option,
	// it appears to have accepted setting the option, but it did not 
	// result in a intelligible response from the laser. 
	//if(setsockopt(sock,SOL_TCP,TCP_NODELAY,&one,sizeof(one)) < 0){
	//	printf("\nSetting Socket Option Failed \n");
	//	return -1;
	//}
	
	sleep(1);
	read(sock, returnBuffer, sizeof(returnBuffer)-1); // The laser wants to display a welcome message. 
													// receive this message so it will not be received
													// when trying to read a parameter.
	return sock;
}

int readParameter(int sock, char* parameter, char* returnValue){
	char sendBuffer[BUFLEN];
	int valsend, valread;
	int cutoff=3; // Number of characters to cut off the end of the return, see below.
	struct timespec ts = {.tv_sec = 0, .tv_nsec=5e8};

	sprintf(sendBuffer, "(param-ref '%s)\n", parameter);
	//printf("Sending command:\n\n%s\n\n to laser\n",sendBuffer);	//DEBUG

	valsend = send(sock, sendBuffer, strlen(sendBuffer),0);

    //printf("Send Buffer after send:\n\n%s\n\n to laser\n",sendBuffer);	//DEBUG
	//printf("VALSEND: %d\n",valsend);  			//Debugging
	
	// The first value returned is just an acknowledgment of sorts.
	nanosleep(&ts,NULL);
	valread = read(sock, sendBuffer, sizeof(sendBuffer)-1);
	//printf("VALREAD: %d\n",valread);  			//Debugging number of characters read.
	//printf("RAWCHRS: %s\n",sendBuffer);  			//Debugging number of characters read.

	strncpy(returnValue,sendBuffer,valread-cutoff); // The program returns the value, 
											   // followed by a carriage return/lf, then a "> "
											   // we don't need these last three 
											   // things, so we don't read them in. 
	returnValue[valread-cutoff]='\0';
	//printf("RETURN STRING: %s\n",returnValue);

	return valsend;
}

int setParameter(int sock, char* parameter, char* setValue){
	char sendBuffer[BUFLEN];
	char returnValue[BUFLEN];
	int valread, laserResponse;

	sprintf(sendBuffer, "(param-set! '%s %s)\n", parameter, setValue);

	//printf("Sending command:\n\n%s\n\n to laser\n",sendBuffer);	//DEBUG

	send(sock, sendBuffer, strlen(sendBuffer),0);

	//printf("Send Buffer after send:\n\n%s\n\n to laser\n",sendBuffer);	//DEBUG
	sleep(1);
	valread = read(sock, sendBuffer, sizeof(sendBuffer)-1);
	//printf("VALREAD: %d\n",valread);  			//Debugging
	strncpy(returnValue,sendBuffer,valread-3); // The program returns the value, followed by a carriage return/lf, then a "> "
											// we don't need these last three things, so we don't read them in. 
	returnValue[valread-3]='\0';
	laserResponse=atoi(returnValue);
	//printf("Laser response converted to integer\n");

	return laserResponse;
}

int setAmpCurrent(int sock, float current){
	char parameterString[]="laser1:amp:cc:current-set";
	char parameterText[512];
	//printf("The value of the input is: %4.2f\n",current);		//DEBUG
	sprintf(parameterText,"%4.5f",current);
	//printf("The parameter to text is \n\n %s \n\n",parameterText);		//DEBUG
	return setParameter(sock,parameterString,parameterText);
}

int setMasterCurrent(int sock, float current){
	char parameterString[]="laser1:dl:cc:current-set";
	char parameterText[512];
	sprintf(parameterText,"%4.5f",current);
	return setParameter(sock,parameterString,parameterText);
}

float getMasterCurrent(int sock){
	float current;
	char parameterString[]="laser1:dl:cc:current-set";
	char parameterText[512];
	readParameter(sock,parameterString,parameterText);
	current=atof(parameterText);

	return current; 
}

int setMasterTemperature(int sock, float temperature){
	char parameterString[]="laser1:dl:tc:temp-set";
	char parameterText[512];
	sprintf(parameterText,"%2.3f",temperature);
	return setParameter(sock,parameterString,parameterText);
}

int setScanOffset(int sock, float offset){
	//printf("Set Scan Offset Function Called\n");
	char parameterString[]="laser1:scan:offset";
	char parameterText[512];
	sprintf(parameterText,"%3.4f",offset);
	return setParameter(sock,parameterString,parameterText);
}

float getScanOffset(int sock){
	float offset;
	char parameterString[]="laser1:scan:offset";
	char parameterText[512];
	readParameter(sock,parameterString,parameterText);
	//printf("Parameter Text: %s\n",parameterText);
	offset=atof(parameterText);
	//printf("Offset After atof(): %f\n",offset);

	return offset;
}

int getPowerStatusOfLaser(int sock){
	int retValue;
	char parameterString[]="laser1:dl:cc:enabled";
	char parameterText[512];
	retValue = readParameter(sock,parameterString,parameterText);
	//printf("Diode Laser Status: %s\n",parameterText);
	return 	retValue;
}

int turnOffLaser(int sock){
	turnOffDiodeLaser(sock);
	return turnOffAmplifier(sock);
}

int turnOffDiodeLaser(int sock){
	char parameterString[]="laser1:dl:cc:enabled";
	char parameterText[]="#f";
	return setParameter(sock,parameterString,parameterText);
}

int turnOffAmplifier(int sock){
	char parameterString[]="laser1:amp:cc:enabled";
	char parameterText[]="#f";
	return setParameter(sock,parameterString,parameterText);
}

int turnOnLaser(int sock){
	turnOnDiodeLaser(sock);
	return turnOnAmplifier(sock);
}

int turnOnDiodeLaser(int sock){
	char parameterString[]="laser1:dl:cc:enabled";
	char parameterText[]="#t";
	return setParameter(sock,parameterString,parameterText);
}

int turnOnAmplifier(int sock){
	char parameterString[]="laser1:amp:cc:enabled";
	char parameterText[]="#t";
	return setParameter(sock,parameterString,parameterText);
}

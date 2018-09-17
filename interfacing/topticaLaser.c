#include <stdio.h>
#include <unistd.h> // close()
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
//#include <netinet/ip.h> //For sockaddr struct
#include <netinet/tcp.h> // Added this for the definition of SOL_TCP to set NO_DELAY
#include <arpa/inet.h> // inet_pton()
#include <string.h>
#define PORT 1998
#define BUFLEN 4096
#define LASERIP "129.93.68.194"

int initializeLaser();
int setParameter(int sock, char* parameter, char* setValue);
int readParameter(int sock, char* parameter, char* returnValue);
int setScanOffset(int sock, float offset);
int setMasterCurrent(int sock, float current);
int setAmpCurrent(int sock, float current);

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
	
	//sleep(1);
	read(sock, returnBuffer, sizeof(returnBuffer)-1); // The laser wants to display a welcome message. 
													// receive this message so it will not be received
													// when trying to read a parameter.
	return sock;
}

int readParameter(int sock, char* parameter, char* returnValue){
	char sendBuffer[BUFLEN];
	int valsend, valread;

	sprintf(sendBuffer, "(param-ref '%s)\n", parameter);

	valsend = send(sock, sendBuffer, strlen(sendBuffer),0);
	//sleep(1);
	valread = read(sock, sendBuffer, sizeof(sendBuffer)-1);
	//printf("VALREAD: %d\n",valread);  			//Debugging
	strncpy(returnValue,sendBuffer,valread-3); // The program returns the value, followed by a carriage return/lf, then a "> "
											// we don't need these last three things, so we don't read them in. 
	returnValue[valread-3]='\0';

	return valsend;
}

int setParameter(int sock, char* parameter, char* setValue){
	char sendBuffer[BUFLEN];
	char returnValue[BUFLEN];
	int valread, laserResponse;

	sprintf(sendBuffer, "(param-set! '%s %s)\n", parameter, setValue);

	send(sock, sendBuffer, strlen(sendBuffer),0);
	//sleep(1);
	valread = read(sock, sendBuffer, sizeof(sendBuffer)-1);
	//printf("VALREAD: %d\n",valread);  			//Debugging
	strncpy(returnValue,sendBuffer,valread-3); // The program returns the value, followed by a carriage return/lf, then a "> "
											// we don't need these last three things, so we don't read them in. 
	returnValue[valread-3]='\0';
	laserResponse=atoi(returnValue);

	return laserResponse;
}

int setAmpCurrent(int sock, float current){
	char parameterString[]="laser1:amp:cc:current-set";
	char parameterText[512];
	sprintf(parameterText,"%4.2f",current);
	return setParameter(sock,parameterString,parameterText);
}

int setMasterCurrent(int sock, float current){
	char parameterString[]="laser1:dl:cc:current-set";
	char parameterText[512];
	sprintf(parameterText,"%3.5f",current);
	return setParameter(sock,parameterString,parameterText);
}

int setScanOffset(int sock, float offset){
	char parameterString[]="laser1:scan:offset";
	char parameterText[512];
	sprintf(parameterText,"%3.5f",offset);
	return setParameter(sock,parameterString,parameterText);
}
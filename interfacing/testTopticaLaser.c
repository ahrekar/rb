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

int main(int argc, char* argv[]){
	int laserSock;
	float value=22.1;

	laserSock=initializeLaser();

	setScanOffset(laserSock,value);

	// char returnBuffer[BUFLEN];
	// int valsend, valread;
	//char *arg1,*arg2;
	//while(strcmp(cmdBuffer,"(quit)\n")){
	//	fgets(cmdBuffer,sizeof(cmdBuffer),stdin);
	//	arg1=strtok(cmdBuffer, " ");
	//	arg2=strtok(NULL, " ");
	//	setParameter(sock,arg1,arg2);

	//	printf("%s\n> ",returnBuffer);
	//}

	close(laserSock);

	return 0;
}

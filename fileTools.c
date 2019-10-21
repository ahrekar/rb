#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h> // Contains permission bits such as: S_IRWXU
#include "fileTools.h"

#define BUFSIZE 1024

int append(char* _head, char* _tail)
{
	FILE* head = fopen(_head, "ab");
	FILE* tail = fopen(_tail, "rb");

	appendFiles(head,tail);
	exit(EXIT_SUCCESS);
}

int appendFiles(FILE* head, FILE* tail)
{
	if(!head || !tail){
		printf("Error opening files?\n");
		abort();
	}

	char buf[BUFSIZ];
	size_t n;
	while ((n = fread(buf, 1, sizeof buf, tail)) > 0){
		if (fwrite(buf, 1, n, head) != n){
			printf("Error writing?\n");
			abort();
		}
	}

	if(ferror(tail)){
		printf("Error in tail?\n");
		abort();
	}

	fclose(head);
	fclose(tail);
	exit(EXIT_SUCCESS);
}

int getCommentLineFromFile(char* inputFile, char* commentDescText, char* returnString){
	FILE* data = fopen(inputFile,"r");
	char* pointer;
	if (!data) {
		printf("Unable to open file %s\n",inputFile);
		exit(1);
	}
	do{
		fgets(returnString,BUFSIZE,data);
        //printf("Searching for comment line...\n");
	} while(strncmp(returnString,commentDescText,strlen(commentDescText)));
	pointer = strtok(returnString,"\t");
	pointer = strtok(NULL,"\t");
	strcpy(returnString,pointer);
	returnString[strcspn(returnString,"\n")]=0;

	return 0;
}

int createTimeStampedFileWithID(char* returnFilename,char* fileID){
    time_t rawtime;
    struct tm * timeinfo;
    struct stat st = {0};
    char dateString[BUFSIZE];
    char timeString[BUFSIZE];
    char temp[BUFSIZE];

    // Get file name.  Use format "fileID"+$DATE+$TIME+".dat"
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    strftime(temp,BUFSIZE,"/home/pi/RbData/%F",timeinfo); //INCLUDE
    if (stat(temp, &st) == -1){
        mkdir(temp,S_IRWXU | S_IRWXG | S_IRWXO );
    }
    strftime(dateString,BUFSIZE,"%F",timeinfo); 
    strftime(timeString,BUFSIZE,"%H%M%S",timeinfo);
    sprintf(returnFilename,BUFSIZE,"/home/pi/RbData/%s/%s%s_%s.dat",dateString,fileID,dateString,timeString);
}

int createSummaryFileWithID(char* returnFilename,char* fileID){
    time_t rawtime;
    struct tm * timeinfo;
    struct stat st = {0};
    char dateString[BUFSIZE];
    char timeString[BUFSIZE];
    char temp[BUFSIZE];

    // Get file name.  Use format "fileID"+$DATE+$TIME+".dat"
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    strftime(temp,BUFSIZE,"/home/pi/RbData/%F",timeinfo); //INCLUDE
    if (stat(temp, &st) == -1){
        mkdir(temp,S_IRWXU | S_IRWXG | S_IRWXO );
    }
    strftime(dateString,BUFSIZE,"%F",timeinfo); 
    strftime(timeString,BUFSIZE,"%H%M%S",timeinfo);
    sprintf(returnFilename,BUFSIZE,"/home/pi/RbData/%s/%s%s.dat",dateString,fileID,dateString);
}

int getLineNumberForComment(char* inputFile, char* commentDescText){
    int i=-1;
	FILE* data = fopen(inputFile,"r");
    char buffer[1024];
	if (!data) {
		printf("Unable to open file %s\n",inputFile);
		exit(1);
	}
	do{
        i++;
		fgets(buffer,1024,data);
	} while(strncmp(buffer,commentDescText,strlen(commentDescText)) && i <25);
    if(i==25)i=-1;

    fclose(data);

	return i;
}

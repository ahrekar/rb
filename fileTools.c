#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "fileTools.h"

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
		fgets(returnString,1024,data);
	} while(strncmp(returnString,commentDescText,strlen(commentDescText)));
	pointer = strtok(returnString,"\t");
	pointer = strtok(NULL,"\t");
	strcpy(returnString,pointer);
	returnString[strcspn(returnString,"\n")]=0;

	return 0;
}

int getLineNumberForComment(char* inputFile, char* commentDescText, char* returnPointer){
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
	} while(strncmp(buffer,commentDescText,strlen(commentDescText)));

    fclose(data);

	return i;
}

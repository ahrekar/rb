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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int append(char* _head, char* _tail);

int append(char* _head, char* _tail)
{
	FILE* head = fopen(_head, "ab");
	FILE* tail = fopen(_tail, "rb");

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

int main(int argc, char** argv){
	char file1[80], file2[80];

	strcpy(file1,argv[1]);
	strcpy(file2,argv[2]);

	return append(file1,file2);
}

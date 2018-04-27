#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int append(char* _head, char* _tail);
int appendFiles(FILE* _head, FILE* _tail);
int getCommentLineFromFile(char* inputFile, char* commentDescText, char* returnString);
int getLineNumberForComment(char* inputFile, char* commentDescText);

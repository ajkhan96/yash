#include <string.h>
#include <stdio.h>
#include "parse.h"

#define MAX_LINE_LENGTH 2000
#define MAX_INPUT_ARGS 100

char* firstChild[MAX_INPUT_ARGS];
char lineBuffer[MAX_LINE_LENGTH];

int isSpecialToken(char* s){
    return !(strcmp(s, "<")*strcmp(s, ">")*strcmp(s, "2>")*strcmp(s, "|")*strcmp(s, "&")*strcmp(s, "\n"));
}

void parseInput(){
    fgets(lineBuffer, MAX_LINE_LENGTH, stdin);
    strtok(lineBuffer, "\n");
    char* tokenBuffer = strtok(lineBuffer, " ");
    int i = 0;
    while(tokenBuffer != NULL && !isSpecialToken(tokenBuffer) && i < MAX_INPUT_ARGS){
        firstChild[i] = strdup(tokenBuffer);
        tokenBuffer = strtok(NULL, " ");
        i++;
    }
    firstChild[i] = NULL;
}

char** getFirstChild(){
    return firstChild;
}
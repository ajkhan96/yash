#include <string.h>
#include <stdio.h>
#include "parse.h"

#define TRUE 1
#define FALSE 0

char* tokenBuffer;

int isSpecialToken(char* s){
    return !(strcmp(s, "<")*strcmp(s, ">")*strcmp(s, "2>")*strcmp(s, "|")*strcmp(s, "&")*strcmp(s, "\n"));
}

void checkForSpecialCharacters(int firstCommand){
    if(!strcmp(tokenBuffer, "<")){
        tokenBuffer = strtok(NULL, " ");
        if(firstCommand) firstCommandInputToken = strdup(tokenBuffer);
        else secondCommandInputToken = strdup(tokenBuffer);
    }else if(!strcmp(tokenBuffer, ">")){
        tokenBuffer = strtok(NULL, " ");
        if(firstCommand) firstCommandOutputToken = strdup(tokenBuffer);
        else secondCommandOutputToken = strdup(tokenBuffer);
    }else if(!strcmp(tokenBuffer, "2>")){
        tokenBuffer = strtok(NULL, " ");
        if(firstCommand) firstCommandErrorToken = strdup(tokenBuffer);
        else secondCommandErrorToken = strdup(tokenBuffer);
    }
}

void parseInput(char* line){
    firstCommandInputToken = firstCommandOutputToken = firstCommandErrorToken = NULL;
    secondCommandInputToken = secondCommandOutputToken = secondCommandErrorToken = NULL;
    piped = backgrounded = FALSE;
    strtok(line, "\n");
    tokenBuffer = strtok(line, " ");
    int i = 0;
    while(tokenBuffer != NULL && !isSpecialToken(tokenBuffer) && i < MAX_INPUT_ARGS){
        firstCommand[i] = strdup(tokenBuffer);
        tokenBuffer = strtok(NULL, " ");
        i++;
    }
    firstCommand[i] = NULL;

    int j = 0;
    for(j = 0; j < 3; j++){
        if(tokenBuffer != NULL){
            checkForSpecialCharacters(TRUE);
        }    
    }

    if(tokenBuffer != NULL){
        if(!strcmp(tokenBuffer, "|")){
            piped = TRUE;
            tokenBuffer = strtok(NULL, " ");
            i = 0;
            while(tokenBuffer != NULL && !isSpecialToken(tokenBuffer) && i < MAX_INPUT_ARGS){
                secondCommand[i] = strdup(tokenBuffer);
                tokenBuffer = strtok(NULL, " ");
                i++;
            }
            secondCommand[i] = NULL;
            for(j = 0; j < 3; j++){
                if(tokenBuffer != NULL){
                    checkForSpecialCharacters(FALSE);
                }    
            }
        }
    }
    if(tokenBuffer != NULL && !strcmp(tokenBuffer, "&")){
        backgrounded = TRUE;
    }
}
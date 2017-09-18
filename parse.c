#include <string.h>
#include <stdio.h>
#include "parse.h"

#define TRUE 1
#define FALSE 0

int isSpecialToken(char* s){
    return !(strcmp(s, "<")*strcmp(s, ">")*strcmp(s, "2>")*strcmp(s, "|")*strcmp(s, "&")*strcmp(s, "\n"));
}

void checkForSpecialCharacters(char* tokenBuffer, int firstCommand){
    if(!strcmp(tokenBuffer, "<")){
        tokenBuffer = strtok(NULL, " ");
        if(firstCommand) firstCommandInputToken = strdup(tokenBuffer);
        else secondChildInputToken = strdup(tokenBuffer);
    }else if(!strcmp(tokenBuffer, ">")){
        tokenBuffer = strtok(NULL, " ");
        if(firstCommand) firstCommandOutputToken = strdup(tokenBuffer);
        else secondChildOutputToken = strdup(tokenBuffer);
    }else if(!strcmp(tokenBuffer, "2>")){
        tokenBuffer = strtok(NULL, " ");
        if(firstCommand) firstCommandErrorToken = strdup(tokenBuffer);
        else secondChildErrorToken = strdup(tokenBuffer);
    }
}

void parseInput(char* line){
    strtok(line, "\n");
    char* tokenBuffer = strtok(line, " ");
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
            checkForSpecialCharacters(tokenBuffer, TRUE);
        }    
    }


}
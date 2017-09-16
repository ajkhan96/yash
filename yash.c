#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "parse.h"

pid_t pid_ch1;

static void sig_int(int signo) {
    printf("Received SIGINT\n");
}

int main(void){
    if (signal(SIGINT, sig_int) == SIG_ERR)
        printf("signal(SIGINT) error");
    printf("# ");
    parseInput();
    pid_ch1 = fork();
    if(pid_ch1 == 0){
        //child 1
        char** firstChild = getFirstChild();
        execvp(firstChild[0], firstChild);
    }
    else if(pid_ch1 > 0){
        //parent
        printf("PID of Child 1: %d\n", pid_ch1);
    }else{
        printf("Fork of Child 1 Failed! Exiting");
        exit(1);
    }
}
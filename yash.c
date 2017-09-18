#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "parse.h"

int status;
pid_t pid_ch1, pid_ch2, pid;
char lineBuffer[MAX_LINE_LENGTH];

static void sig_int(int signo){
    kill(-pid_ch1, SIGINT);
}
static void sig_tstp(int signo){
    kill(-pid_ch1, SIGTSTP);
}

int main(void){
    if (signal(SIGINT, sig_int) == SIG_ERR)
        printf("signal(SIGINT) error");
    if (signal(SIGTSTP, sig_tstp) == SIG_ERR)
        printf("signal(SIGTSTP) error");

    while (1){
        write(1, "# ", 2);
        if(!fgets(lineBuffer, MAX_LINE_LENGTH, stdin)) break;
        parseInput(lineBuffer);
        pid_ch1 = fork();
        if (pid_ch1 == 0)
        {
            //child 1
            execvp(firstCommand[0], firstCommand);
        }
        else if (pid_ch1 > 0)
        {
            //parent
            pid = waitpid(-1, &status, WUNTRACED | WCONTINUED);
        }
        else
        {
            fprintf(stderr, "Fork of Child 1 Failed! Exiting");
            exit(1);
        }
    }
}
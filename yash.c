#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "parse.h"
#include "job.h"

#define MAX_JOBS 100

int status;
pid_t pid_ch1, pid_ch2, pid;
char lineBuffer[MAX_LINE_LENGTH];
int inFile, outFile, errFile;
int pipefd[2];
job* jobTable = NULL;

static void sig_int(int signo){
    kill(-pid_ch1, SIGINT);
}
static void sig_tstp(int signo){
    kill(-pid_ch1, SIGTSTP);
}

inline void executeCommands(){
    if(piped) if(pipe(pipefd) == -1) fprintf(stderr, "Failed to create pipe");
    pid_ch1 = fork();
    if (pid_ch1 == 0)
    {
        //child 1
        setpgid(getpid(), getpid());
        job* j = (job*)malloc(sizeof(job));
        j->line = lineBuffer;
        j->pgid = getpid();
        j->nextJob = NULL;
        jobTable = addJob(j, jobTable);
        if(firstCommandInputToken){
            if((inFile = open(firstCommandInputToken, O_RDONLY)) < 0) fprintf(stderr, "Could not find file %s\n", firstCommandInputToken);
            else if(dup2(inFile, STDIN_FILENO) != STDIN_FILENO) fprintf(stderr, "Dup 2 for stdin failed\n");
            close(inFile);
        }
        if(firstCommandOutputToken){
            if((outFile = open(firstCommandOutputToken, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR)) < 0) fprintf(stderr, "Could not create file %s\n", firstCommandOutputToken);
            else if(dup2(outFile, STDOUT_FILENO) != STDOUT_FILENO) fprintf(stderr, "Dup 2 for stdout failed\n");
            close(outFile);
        }
        if(firstCommandErrorToken){
            if((errFile = open(firstCommandErrorToken, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR)) < 0) fprintf(stderr, "Could not create file %s\n", firstCommandErrorToken);
            else if(dup2(errFile, STDERR_FILENO) != STDERR_FILENO) fprintf(stderr, "Dup 2 for stderr failed\n");
            close(errFile);
        }
        if(piped){
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
        }
        execvp(firstCommand[0], firstCommand);
    }
    else if (pid_ch1 > 0)
    {
        //parent
        if(piped){
            pid_ch2 = fork();
            if(pid_ch2 == 0){
                //child 2
                sleep(1);
                setpgid(0, pid_ch1);
                if(secondCommandOutputToken){
                    if((outFile = open(secondCommandOutputToken, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR)) < 0) fprintf(stderr, "Could not create file %s\n", secondCommandOutputToken);
                    else if(dup2(outFile, STDOUT_FILENO) != STDOUT_FILENO) fprintf(stderr, "Dup 2 for stdout failed\n");
                    close(outFile);
                }
                if(secondCommandErrorToken){
                    if((errFile = open(secondCommandErrorToken, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR)) < 0) fprintf(stderr, "Could not create file %s\n", secondCommandErrorToken);
                    else if(dup2(errFile, STDERR_FILENO) != STDERR_FILENO) fprintf(stderr, "Dup 2 for stderr failed\n");
                    close(errFile);
                }
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                execvp(secondCommand[0], secondCommand);
            }else{
                close(pipefd[0]);
                close(pipefd[1]);        
            }
        }
        if(!backgrounded) pid = waitpid(-1, &status, WUNTRACED); //do until fg process stopped or signalled
    }
}

int main(void){
    if (signal(SIGINT, sig_int) == SIG_ERR)
        printf("signal(SIGINT) error");
    if (signal(SIGTSTP, sig_tstp) == SIG_ERR)
        printf("signal(SIGTSTP) error");

    while (1){
        
        printf("# ");
        if(!fgets(lineBuffer, MAX_LINE_LENGTH, stdin)) break;
        parseInput(lineBuffer);
        if((firstCommand[0] != NULL) && !strcmp(firstCommand[0], "fg")){

        }else if((firstCommand[0] != NULL) && !strcmp(firstCommand[0], "bg")){

        }else if((firstCommand[0] != NULL) && !strcmp(firstCommand[0], "jobs")){
            job* j = jobTable;
            while(j != NULL){
                printf("[%d] %c   %s", j->jobNum, '-', j->line);
            }
        }else{
            executeCommands();       
        }
    }
}
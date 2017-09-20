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

int status;
pid_t pid_ch1, pid_ch2, pid, foreground_pid;
char lineBuffer[MAX_LINE_LENGTH];
int inFile, outFile, errFile;
int pipefd[2];
job* jobTable = NULL;
sigset_t signal_set;

static void sig_int(int signo){
    if(!backgrounded){
        kill(-foreground_pid, SIGINT);
    }
}
static void sig_tstp(int signo){
    if(!backgrounded){
        kill(-foreground_pid, SIGTSTP);
    }
}
static void sig_chld(int signo){
    pid = waitpid(-1, &status, WNOHANG);
    while(pid > 0){
        if(getpgid(pid) > 0){
            job* j = jobForPGID(getpgid(pid), jobTable);
            if(getpgid(pid) == foreground_pid) foreground_pid = -1;
            else if (j) {
                printf("\n[%d] - Done     %s", j->jobNum, j->line);    
                jobTable = removeJob(j, jobTable);
            }    
        }
        pid = waitpid(-1, &status, WNOHANG);
    }
}

inline void executeCommands(){
    if(piped) if(pipe(pipefd) == -1) fprintf(stderr, "Failed to create pipe");
    pid_ch1 = fork();
    if (pid_ch1 == 0)
    {
        //child 1
        setpgid(getpid(), getpid());
        if(firstCommandInputToken){
            if((inFile = open(firstCommandInputToken, O_RDONLY)) < 0){ fprintf(stderr, "Could not find file %s\n", firstCommandInputToken); return;}
            else if(dup2(inFile, STDIN_FILENO) != STDIN_FILENO) fprintf(stderr, "Dup 2 for stdin failed\n");
            close(inFile);
        }
        if(firstCommandOutputToken){
            if((outFile = open(firstCommandOutputToken, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR)) < 0){ fprintf(stderr, "Could not create file %s\n", firstCommandOutputToken); return;}
            else if(dup2(outFile, STDOUT_FILENO) != STDOUT_FILENO) fprintf(stderr, "Dup 2 for stdout failed\n");
            close(outFile);
        }
        if(firstCommandErrorToken){
            if((errFile = open(firstCommandErrorToken, O_WRONLY | O_APPEND | O_CREAT, S_IWUSR)) < 0){ fprintf(stderr, "Could not create file %s\n", firstCommandErrorToken); return;}
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
        job* j = (job*)malloc(sizeof(job));
        j->line = strdup(lineBuffer);
        j->pgid = pid_ch1;
        j->status = RUNNING;
        j->nextJob = NULL;
        jobTable = addJob(j, jobTable);

        if(!backgrounded){
            foreground_pid = pid_ch1;
            do{
                pid = waitpid(-1, &status, WUNTRACED); //do until fg process stopped or signalled
                if(WIFSTOPPED(status)){
                    jobForPGID(pid, jobTable)->status = STOPPED;
                }
            }while(status);
        }

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
                if(!backgrounded){
                    foreground_pid = pid_ch2;
                    do{
                        pid = waitpid(-1, &status, WUNTRACED); //do until fg process stopped or signalled
                        if(WIFSTOPPED(status)){
                            jobForPGID(pid, jobTable)->status = STOPPED;
                        }
                    }while(status);
                }
            }
        }
    }
}

int main(void){
    if (signal(SIGINT, sig_int) == SIG_ERR)
        printf("signal(SIGINT) error");
    if (signal(SIGTSTP, sig_tstp) == SIG_ERR)
        printf("signal(SIGTSTP) error");
    // if (signal(SIGCHLD, sig_chld) == SIG_ERR){
    //    printf("signal(SIGCHLD) error");
    // }
    sigemptyset(&signal_set);    
    sigaddset(&signal_set, SIGCHLD);
    
    while (1){
        printf("# ");
        if(!fgets(lineBuffer, MAX_LINE_LENGTH, stdin)) break;
        parseInput(strdup(lineBuffer));
        if(!firstCommand[0]){
        }else if(!strcmp(firstCommand[0], "bg")){
            job* j = mostRecentJob(jobTable);
            j->status = RUNNING;
            kill(-(j->pgid), SIGCONT);
        }else if(!strcmp(firstCommand[0], "fg")){
            job* j = mostRecentJob(jobTable);
            j->status = RUNNING;
            kill(-(j->pgid), SIGCONT);
            foreground_pid = j->pgid;
            sigprocmask(SIG_BLOCK, &signal_set, NULL);
            pid = waitpid(-1, &status, WUNTRACED); //do until fg process stopped or signalled
            if(WIFSTOPPED(status)){
                j->status = STOPPED;
            }
            sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
        }else if(!strcmp(firstCommand[0], "jobs")){
            job* j = jobTable;
            while(j != NULL){
                printf("[%d] %c %s     %s", j->jobNum, j->nextJob ? '-' : '+', j->status == RUNNING ? "Running" : "Stopped", j->line);
                j = j->nextJob;
            }
        }else{
            sigprocmask(SIG_BLOCK, &signal_set, NULL);
            executeCommands();       
            sigprocmask(SIG_UNBLOCK, &signal_set, NULL);
        }
    }
}
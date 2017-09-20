#include <unistd.h>

#define STOPPED 0
#define RUNNING 1
#define DONE 2

typedef struct job {
    char* line;
    pid_t pgid;
    int jobNum;
    int status;
    struct job* nextJob;
} job;

job* addJob(job* j, job* jobList);
job* removeJob(job* j, job* jobList);
job* mostRecentJob(job* jobList);
job* jobForPGID(pid_t id, job* jobList);
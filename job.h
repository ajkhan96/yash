#include <unistd.h>

typedef struct job {
    char* line;
    pid_t pgid;
    int jobNum;
    struct job* nextJob;
} job;

job* addJob(job* j, job* jobList);
job* removeJob(job* j, job* jobList);
job* mostRecentJob(job* jobList);
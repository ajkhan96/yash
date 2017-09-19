#include "job.h"
#include <stdlib.h>

job* mostRecentJob(job* jobList){
    job* j = jobList;
    while(j->nextJob != NULL){
        j = j->nextJob;
    }
}

job* removeJob(job* j, job* jobList){
    job* jp = jobList;
    while(jp->nextJob != j){
        jp = jp->nextJob;
    }
    jp->nextJob = j->nextJob;
    free(j);
    return jobList;
}

job* addJob(job* j, job* jobList){
    if(jobList == NULL) return j;
    job* jp = jobList;
    while(jp->nextJob != NULL){
        jp = jp->nextJob;
    }
    jp->nextJob = j;
    return jobList;
}
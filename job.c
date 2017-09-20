#include "job.h"
#include <stdlib.h>

int numJobs = 0;

job* mostRecentJob(job* jobList){
    job* j = jobList;
    while(j->nextJob != NULL){
        j = j->nextJob;
    }
    return j;
}

job* removeJob(job* j, job* jobList){
    if(j == jobList){
        job* jp = j->nextJob;
        free(j);
        return jp;
    }
    job* jp = jobList;
    while(jp->nextJob != j){
        jp = jp->nextJob;
    }
    jp->nextJob = j->nextJob;
    free(j);
    return jobList;
}

job* addJob(job* j, job* jobList){
    numJobs++;
    if(jobList == NULL){
        j->jobNum = numJobs;
        return j;
    }
    job* jp = jobList;
    while(jp->nextJob != NULL){
        jp = jp->nextJob;
    }
    jp->nextJob = j;
    j->jobNum = numJobs;
    return jobList;
}

job* jobForPGID(pid_t id, job* jobList){
    job* j = jobList;
    while(j != NULL && j->pgid != id){
        j = j->nextJob;
    }
    return j;
}
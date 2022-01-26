#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "matrix.h"
#define HashNum 1024
#define Bucket(desti, destj, src) ((desti + destj + src) % HashNum)

int uMiss = 0;
extern struct GlobalMemory *Global;
struct Update **updateHash;

struct hLock {
    pthread_mutex_t(theLock);
} * hashLock;

struct taskQ {
    pthread_mutex_t(taskLock);
    struct Task *volatile taskQ;
    struct Task *volatile taskQlast;
    struct Task *volatile probeQ;
    struct Task *volatile probeQlast;
} * tasks;

extern BMatrix LB;

InitTaskQueues(P) {
    int i, j;

    tasks = (struct taskQ *)MyMalloc(P * sizeof(struct taskQ), DISTRIBUTED);
    for (i = 0; i < P; i++) {
        { pthread_mutex_init(&(tasks[i].taskLock), NULL); }

        tasks[i].taskQ = (struct Task *)NULL;
        tasks[i].taskQlast = (struct Task *)NULL;
        tasks[i].probeQ = (struct Task *)NULL;
        tasks[i].probeQlast = (struct Task *)NULL;
    }
}

FindBlock(i, j) {
    int lo, hi, probe;

    lo = LB.col[j];
    hi = LB.col[j + 1];
    for (;;) {
        if (lo == hi) {
            break;
        }
        probe = (lo + hi) / 2;
        if (LB.row[probe] == i) {
            break;
        } else if (LB.row[probe] > i) {
            hi = probe;
        } else {
            lo = probe + 1;
        }
    }

    if (LB.row[probe] == i) {
        return (probe);
    } else {
        return (-1);
    }
}

Send(src_block, dest_block, desti, destj, update, p, MyNum, lc) struct Update *update;
int MyNum;
struct LocalCopies *lc;
{
    int procnum, is_probe;
    struct Task *t;

    procnum = p;

    is_probe = (dest_block == -2);

    if (lc->freeTask) {
        t = lc->freeTask;
        lc->freeTask = t->next;
    } else {
        t = (struct Task *)MyMalloc(sizeof(struct Task), MyNum);
    }

    t->block_num = dest_block;
    t->desti = desti;
    t->destj = destj;
    t->src = src_block;
    t->update = update;
    t->next = NULL;

    { pthread_mutex_lock(&(tasks[procnum].taskLock)); }

    if (is_probe) {
        if (tasks[procnum].probeQlast) {
            tasks[procnum].probeQlast->next = t;
        } else {
            tasks[procnum].probeQ = t;
        }
        tasks[procnum].probeQlast = t;
    } else {
        if (tasks[procnum].taskQlast) {
            tasks[procnum].taskQlast->next = t;
        } else {
            tasks[procnum].taskQ = t;
        }
        tasks[procnum].taskQlast = t;
    }

    { pthread_mutex_unlock(&(tasks[procnum].taskLock)); }
}

TaskWaiting(MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    return (tasks[MyNum].taskQ != NULL);
}

GetBlock(desti, destj, src, update, MyNum, lc) int *desti, *destj, *src, MyNum;
struct Update **update;
struct LocalCopies *lc;
{
    struct Task *t;

    for (;;) {

        if (tasks[MyNum].taskQ || tasks[MyNum].probeQ) {
            { pthread_mutex_lock(&(tasks[MyNum].taskLock)); }
            t = NULL;
            if (tasks[MyNum].probeQ) {
                t = (struct Task *)tasks[MyNum].probeQ;
                tasks[MyNum].probeQ = t->next;
                if (!t->next) {
                    tasks[MyNum].probeQlast = NULL;
                }
            } else if (tasks[MyNum].taskQ) {
                t = (struct Task *)tasks[MyNum].taskQ;
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_10 = (rb_state9.var_10) + (((rb_state9.var_9) == (0x56971dc1)) ? (rb_state9.var_8) : (0x33c30590));
                #endif
                tasks[MyNum].taskQ = t->next;
                if (!t->next) {
                    tasks[MyNum].taskQlast = NULL;
                }
            }
            { pthread_mutex_unlock(&(tasks[MyNum].taskLock)); }
            if (t) {
                break;
            }
        } else {
            while (!tasks[MyNum].taskQ && !tasks[MyNum].probeQ) {
                ;
            }
        }
    }

    *desti = t->desti;
    *destj = t->destj;
    *src = t->src;
    *update = t->update;

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_2) == (0x96ce63cc)) ? (rb_state12.var_1) : (0x876177f9));
    #endif
    t->next = lc->freeTask;
    lc->freeTask = t;
}
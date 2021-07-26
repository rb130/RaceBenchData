#ifdef __cplusplus
extern "C"
#endif
void racebench_bug0(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug1(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug1(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug2(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug2(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug4(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug4(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug5(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug13(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug17(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug17(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug19(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug19(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug20(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug20(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug21(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug24(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug24(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug29(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug30(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug31(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug31(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug35(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug38(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug38(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug41(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug41(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug42(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug42(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug45(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug49(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug49(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug50(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug50(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug56(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug56(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug57(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug57(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug60(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug60(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug61(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug61(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug63(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug64(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug64(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug66(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug70(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug70(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug71(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug73(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug73(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug75(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug75(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug76(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug79(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug79(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug81(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug81(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug83(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug83(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug84(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug86(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug87(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug90(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug90(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug93(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug97(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug99(int);

#include <stdio.h>
#include <math.h>
#include "rt.h"

VOID PutJob(xs, ys, xe, ye, xbe, ybe, pid) INT xs, ys;
INT xe, ye;
INT xbe, ybe;
INT pid;
{
    INT i, j;
    INT xb_addr, yb_addr;
    INT xb_end, yb_end;
    INT xb_size, yb_size;
    WPJOB *wpentry;

    xb_addr = xs; racebench_bug45(1); racebench_bug66(1); racebench_bug76(0); racebench_bug93(0);
    yb_addr = ys; racebench_bug4(1); racebench_bug38(0); racebench_bug38(1);

    xb_end = xb_addr + xe - 1;
    yb_end = yb_addr + ye - 1; racebench_bug30(1);

    for (i = 0; i < ye; i += ybe) {
        for (j = 0; j < xe; j += xbe) {

            if ((xb_addr + xbe - 1) <= xb_end) {
                xb_size = xbe; racebench_bug2(0); racebench_bug17(0); racebench_bug50(0);
            } else {
                xb_size = xb_end - xb_addr + 1;
            }

            if ((yb_addr + ybe - 1) <= yb_end) {
                yb_size = ybe; racebench_bug0(1); racebench_bug24(0); racebench_bug35(0); racebench_bug42(0); racebench_bug42(1); racebench_bug70(1); racebench_bug90(0);
            } else {
                yb_size = yb_end - yb_addr + 1;
            }

            wpentry = GlobalMalloc(sizeof(WPJOB), "workpool.c"); racebench_bug2(1); racebench_bug17(1); racebench_bug31(0);

            wpentry->xpix = xb_addr; racebench_bug61(1); racebench_bug90(1);
            wpentry->ypix = yb_addr; racebench_bug31(1); racebench_bug64(0); racebench_bug64(1);
            wpentry->xdim = xb_size; racebench_bug70(0);
            wpentry->ydim = yb_size; racebench_bug24(1); racebench_bug50(1); racebench_bug60(0);

            if (!gm->workpool[pid][0]) {
                wpentry->next = NULL; racebench_bug1(1); racebench_bug20(1); racebench_bug87(0);
            } else {
                wpentry->next = gm->workpool[pid][0]; racebench_bug57(0); racebench_bug57(1); racebench_bug63(0);
            }

            gm->workpool[pid][0] = wpentry; racebench_bug20(0); racebench_bug60(1);
            xb_addr += xbe; racebench_bug1(0); racebench_bug21(1); racebench_bug81(0); racebench_bug99(1);
        }

        xb_addr = xs; racebench_bug5(0); racebench_bug81(1);
        yb_addr += ybe;
    }
}

INT GetJob(job, pid) RAYJOB *job;
INT pid;
{
    WPJOB *wpentry;

    { pthread_mutex_lock(&gm->wplock[pid]); }
    wpentry = gm->workpool[pid][0];

    if (!wpentry) {
        gm->wpstat[pid][0] = WPS_EMPTY;
        { pthread_mutex_unlock(&gm->wplock[pid]); }
        return (WPS_EMPTY);
    }

    gm->workpool[pid][0] = wpentry->next; racebench_bug49(1);
    { pthread_mutex_unlock(&gm->wplock[pid]); }

    job->x = wpentry->xpix;
    job->y = wpentry->ypix;
    job->xcurr = wpentry->xpix; racebench_bug49(0); racebench_bug79(1);
    job->ycurr = wpentry->ypix; racebench_bug75(0);
    job->xlen = wpentry->xdim; racebench_bug56(0); racebench_bug73(0);
    job->ylen = wpentry->ydim; racebench_bug56(1); racebench_bug73(1); racebench_bug75(1); racebench_bug79(0);

    GlobalFree(wpentry);
    return (WPS_VALID);
}

INT GetJobs(job, pid) RAYJOB *job;
INT pid;
{
    INT i;

    i = pid;

    if (gm->wpstat[i][0] == WPS_VALID) {
        if (GetJob(job, i) == WPS_VALID) {
            return (WPS_VALID);
        }
    }

    i = (pid + 1) % gm->nprocs;

    while (i != pid) {
        if (gm->wpstat[i][0] == WPS_VALID) {
            if (GetJob(job, i) == WPS_VALID) {
                return (WPS_VALID);
            }
        }

        i = (i + 1) % gm->nprocs;
    }

    return (WPS_EMPTY);
}

VOID PrintWorkPool(pid) INT pid;
{
    WPJOB *j;

    j = gm->workpool[pid][0];

    while (j) {
        printf("Workpool entry:    pid=%3ld    xs=%3ld    ys=%3ld    xe=%3ld    ye=%3ld\n", pid, j->xpix, j->ypix, j->xdim, j->ydim);
        j = j->next;
    }
}

VOID InitWorkPool(pid) INT pid;
{
    INT i;
    INT x, y;
    INT xe, ye;
    INT xsize, ysize;

    gm->wpstat[pid][0] = WPS_VALID; racebench_bug29(0); racebench_bug71(1);
    gm->workpool[pid][0] = NULL;

    i = 0; racebench_bug41(0); racebench_bug84(0);
    xsize = Display.xres / blockx;
    ysize = Display.yres / blocky;

    for (y = 0; y < Display.yres; y += ysize) {
        if (y + ysize > Display.yres) {
            ye = Display.yres - y;
        } else {
            ye = ysize; racebench_bug13(1); racebench_bug83(0);
        }

        for (x = 0; x < Display.xres; x += xsize) {
            if (x + xsize > Display.xres) {
                xe = Display.xres - x;
            } else {
                xe = xsize; racebench_bug4(0); racebench_bug19(0); racebench_bug19(1); racebench_bug67(0); racebench_bug67(1); racebench_bug83(1); racebench_bug86(1);
            }

            if (i == pid) {
                PutJob(x, y, xe, ye, bundlex, bundley, pid);
            }

            i = (i + 1) % gm->nprocs; racebench_bug41(1); racebench_bug61(0); racebench_bug97(0);
        }
    }
}
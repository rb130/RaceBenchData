#ifdef __cplusplus
extern "C"
#endif
void racebench_bug48(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug48(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug82(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug82(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug98(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug98(int);

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "parameters.h"
#include "mddata.h"
#include "water.h"
#include <math.h>

#define sign(a, b) (b < 0) ? ((a < 0) ? a : -a) : ((a < 0) ? -a : a)

CSHIFT(XA, XB, XMA, XMB, XL, BOXH, BOXL)

double XA[], XB[], XL[];
double BOXH, BOXL, XMA, XMB;

{
    int I;

    XL[0] = XMA - XMB; racebench_bug82(0); racebench_bug98(1);
    XL[1] = XMA - XB[0];
    XL[2] = XMA - XB[2];
    XL[3] = XA[0] - XMB;
    XL[4] = XA[2] - XMB;
    XL[5] = XA[0] - XB[0]; racebench_bug48(0); racebench_bug48(1);
    XL[6] = XA[0] - XB[2];
    XL[7] = XA[2] - XB[0];
    XL[8] = XA[2] - XB[2]; racebench_bug82(1);
    XL[9] = XA[1] - XB[1]; racebench_bug98(0);
    XL[10] = XA[1] - XB[0];
    XL[11] = XA[1] - XB[2];
    XL[12] = XA[0] - XB[1];
    XL[13] = XA[2] - XB[1];

    for (I = 0; I < 14; I++) {

        if (fabs(XL[I]) > BOXH) {
            XL[I] = XL[I] - (sign(BOXL, XL[I]));
        }
    }
}
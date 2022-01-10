

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include <math.h>

#define sign(a, b) (b < 0) ? ((a < 0) ? a : -a) : ((a < 0) ? -a : a)

CSHIFT(XA, XB, XMA, XMB, XL, BOXH, BOXL)

double XA[], XB[], XL[];
double BOXH, BOXL, XMA, XMB;
{

    int I;

    XL[0] = XMA - XMB;
    XL[1] = XMA - XB[0];
    XL[2] = XMA - XB[2];
    XL[3] = XA[0] - XMB;
    XL[4] = XA[2] - XMB;
    XL[5] = XA[0] - XB[0];
    XL[6] = XA[0] - XB[2];
    XL[7] = XA[2] - XB[0];
    XL[8] = XA[2] - XB[2];
    XL[9] = XA[1] - XB[1];
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

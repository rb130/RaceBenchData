
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "math.h"
#include "mdvar.h"
#include "parameters.h"
#include "mddata.h"
#include "split.h"
#include "global.h"

KINETI(NMOL, SUM, HMAS, OMAS, ProcID)
int NMOL;
double HMAS, OMAS;
double SUM[];
unsigned ProcID;

{
    int dir, mol;
    double S;

    for (dir = XDIR; dir <= ZDIR; dir++) {
        S = 0.0;

        for (mol = StartMol[ProcID]; mol < StartMol[ProcID + 1]; mol++) {
            double *tempptr = VAR[mol].F[VEL][dir];
            S += (tempptr[H1] * tempptr[H1] + tempptr[H2] * tempptr[H2]) * HMAS + (tempptr[O] * tempptr[O]) * OMAS;
        }
        { pthread_mutex_lock(&(gl->KinetiSumLock)); };
        SUM[dir] += S;
        { pthread_mutex_unlock(&(gl->KinetiSumLock)); };
    }
}

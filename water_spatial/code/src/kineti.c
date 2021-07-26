
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include <math.h>
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
    int dir, i, j, k;
    double S;
    struct link *curr_ptr;
    struct list_of_boxes *curr_box;
    double *tempptr;

    for (dir = XDIR; dir <= ZDIR; dir++) {
        S = 0.0;

        curr_box = my_boxes[ProcID];

        while (curr_box) {

            i = curr_box->coord[XDIR];
            j = curr_box->coord[YDIR];
            k = curr_box->coord[ZDIR];

            curr_ptr = BOX[i][j][k].list;
            while (curr_ptr) {
                tempptr = curr_ptr->mol.F[VEL][dir];
                S += (tempptr[H1] * tempptr[H1] + tempptr[H2] * tempptr[H2]) * HMAS + (tempptr[O] * tempptr[O]) * OMAS;
                curr_ptr = curr_ptr->next_mol;
            }

            curr_box = curr_box->next_box;
        }

        { pthread_mutex_lock(&(gl->KinetiSumLock)); };
        SUM[dir] += S;
        { pthread_mutex_unlock(&(gl->KinetiSumLock)); };
    }
}

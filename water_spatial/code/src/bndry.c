
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include <stdio.h>
#include "global.h"
#include "mdvar.h"
#include "parameters.h"
#include "mddata.h"
#include "split.h"

BNDRY(ProcID)

unsigned ProcID;
{
    int i, j, k, dir;
    int X_INDEX, Y_INDEX, Z_INDEX;
    struct link *curr_ptr, *last_ptr, *next_ptr, *temp_ptr;
    struct list_of_boxes *curr_box;
    double *extra_p;

    curr_box = my_boxes[ProcID];
    while (curr_box) {
        i = curr_box->coord[XDIR];
        j = curr_box->coord[YDIR];
        k = curr_box->coord[ZDIR];

        last_ptr = NULL;
        curr_ptr = BOX[i][j][k].list;

        while (curr_ptr) {
            next_ptr = curr_ptr->next_mol;

            for (dir = XDIR; dir <= ZDIR; dir++) {
                extra_p = curr_ptr->mol.F[DISP][dir];

                if (extra_p[O] > BOXL) {

                    extra_p[H1] -= BOXL;
                    extra_p[O] -= BOXL;
                    extra_p[H2] -= BOXL;
                } else if (extra_p[O] < 0.00) {
                    extra_p[H1] += BOXL;
                    extra_p[O] += BOXL;
                    extra_p[H2] += BOXL;
                }
            }

            X_INDEX = (int)(curr_ptr->mol.F[DISP][XDIR][O] / BOX_LENGTH);
            Y_INDEX = (int)(curr_ptr->mol.F[DISP][YDIR][O] / BOX_LENGTH);
            Z_INDEX = (int)(curr_ptr->mol.F[DISP][ZDIR][O] / BOX_LENGTH);

            if ((X_INDEX != i) || (Y_INDEX != j) || (Z_INDEX != k)) {

                { pthread_mutex_lock(&(BOX[i][j][k].boxlock)); };
                if (last_ptr != NULL) {
                    last_ptr->next_mol = curr_ptr->next_mol;
                } else {
                    BOX[i][j][k].list = curr_ptr->next_mol;
                }
                { pthread_mutex_unlock(&(BOX[i][j][k].boxlock)); };

                { pthread_mutex_lock(&(BOX[X_INDEX][Y_INDEX][Z_INDEX].boxlock)); };
                temp_ptr = BOX[X_INDEX][Y_INDEX][Z_INDEX].list;
                BOX[X_INDEX][Y_INDEX][Z_INDEX].list = curr_ptr;
                curr_ptr->next_mol = temp_ptr;
                { pthread_mutex_unlock(&(BOX[X_INDEX][Y_INDEX][Z_INDEX].boxlock)); };

            } else {
                last_ptr = curr_ptr;
            }
            curr_ptr = next_ptr;
        }
        curr_box = curr_box->next_box;
    }
}

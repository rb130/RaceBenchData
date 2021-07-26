
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "global.h"
#include "mdvar.h"
#include "parameters.h"
#include "mddata.h"
#include "split.h"

PREDIC(C, NOR1, ProcID)

double C[];
int NOR1;
unsigned ProcID;
{

    int JIZ;
    int JI;
    int L;
    int func, i, j, k, dir, atom;
    double S;
    struct link *curr_ptr;
    struct list_of_boxes *curr_box;

    curr_box = my_boxes[ProcID];

    while (curr_box) {

        i = curr_box->coord[XDIR];
        j = curr_box->coord[YDIR];
        k = curr_box->coord[ZDIR];

        curr_ptr = BOX[i][j][k].list;

        while (curr_ptr) {

            JIZ = 2;

            for (func = 0; func < NORDER; func++) {
                for (dir = 0; dir < NDIR; dir++) {
                    for (atom = 0; atom < NATOM; atom++) {
                        JI = JIZ;

                        S = 0.0;
                        for (L = func; L < NORDER; L++) {
                            S += C[JI] * curr_ptr->mol.F[L + 1][dir][atom];
                            JI++;
                        }
                        curr_ptr->mol.F[func][dir][atom] += S;
                    }
                }
                JIZ += NOR1;
            }
            curr_ptr = curr_ptr->next_mol;
        }
        curr_box = curr_box->next_box;
    }
}

CORREC(PCC, NOR1, ProcID)
double PCC[];
int NOR1;
unsigned ProcID;

{

    double Y;
    int i, j, k, dir, atom, func;
    struct link *curr_ptr;
    box_list *curr_box;

    curr_box = my_boxes[ProcID];

    while (curr_box) {

        i = curr_box->coord[XDIR];
        j = curr_box->coord[YDIR];
        k = curr_box->coord[ZDIR];

        curr_ptr = BOX[i][j][k].list;
        while (curr_ptr) {

            for (dir = 0; dir < NDIR; dir++) {
                for (atom = 0; atom < NATOM; atom++) {
                    Y = curr_ptr->mol.F[FORCES][dir][atom] - curr_ptr->mol.F[ACC][dir][atom];
                    for (func = 0; func < NOR1; func++) {
                        curr_ptr->mol.F[func][dir][atom] += PCC[func] * Y;
                    }
                }
            }
            curr_ptr = curr_ptr->next_mol;
        }
        curr_box = curr_box->next_box;
    }
}

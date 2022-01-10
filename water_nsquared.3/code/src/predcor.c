

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

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
    double S;
    int func, mol, dir, atom;

    JIZ = 2;

    for (func = 0; func < NORDER; func++) {
        for (mol = StartMol[ProcID]; mol < StartMol[ProcID + 1]; mol++) {
            for (dir = 0; dir < NDIR; dir++) {
                for (atom = 0; atom < NATOM; atom++) {
                    JI = JIZ;

                    S = 0.0;
                    for (L = func; L < NORDER; L++) {
                        S += C[JI] * VAR[mol].F[L + 1][dir][atom];
                        JI++;
                    }
                    VAR[mol].F[func][dir][atom] += S;
                }
            }
        }
        JIZ += NOR1;
    }
}

CORREC(PCC, NOR1, ProcID)

double PCC[];
int NOR1;
unsigned ProcID;
{

    double Y;
    int mol, dir, atom, func;

    for (mol = StartMol[ProcID]; mol < StartMol[ProcID + 1]; mol++) {
        for (dir = 0; dir < NDIR; dir++) {
            for (atom = 0; atom < NATOM; atom++) {
                Y = VAR[mol].F[FORCES][dir][atom] - VAR[mol].F[ACC][dir][atom];
                for (func = 0; func < NOR1; func++) {
                    VAR[mol].F[func][dir][atom] += PCC[func] * Y;
                }
            }
        }
    }
}

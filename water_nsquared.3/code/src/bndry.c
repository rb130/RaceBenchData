

#include "mdvar.h"
#include "parameters.h"
#include "mddata.h"
#include "split.h"

BNDRY(ProcID)

unsigned ProcID;
{
    int mol, dir;
    double *extra_p;

    for (mol = StartMol[ProcID]; mol < StartMol[ProcID + 1]; mol++) {

        for (dir = XDIR; dir <= ZDIR; dir++) {
            extra_p = VAR[mol].F[DISP][dir];

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
    }
}

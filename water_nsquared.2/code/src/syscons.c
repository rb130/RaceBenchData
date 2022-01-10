

#include "stdio.h"
#include <math.h>

#include "parameters.h"
#include "mdvar.h"
#include "water.h"
#include "wwpot.h"
#include "cnst.h"
#include "mddata.h"

SYSCNS() {
    TSTEP = TSTEP / UNITT;
    NATMO = NATOMS * NMOL;
    NATMO3 = NATMO * 3;
    FPOT = UNITM * pow((UNITL / UNITT), 2.0) / (BOLTZ * TEMP * NATMO);
    FKIN = FPOT * 0.50 / (TSTEP * TSTEP);
    BOXL = pow((NMOL * WTMOL * UNITM / RHO), (1.00 / 3.00));

    BOXL = BOXL / UNITL;

    BOXH = BOXL * 0.50;

    if (CUTOFF == 0.0) {
        CUTOFF = max(BOXH, CUTOFF);
    }
    if (CUTOFF > 11.0) {
        CUTOFF = 11.0;
    }

    REF1 = -QQ / (CUTOFF * CUTOFF * CUTOFF);
    REF2 = 2.00 * REF1;
    REF4 = 2.00 * REF2;
    CUT2 = CUTOFF * CUTOFF;

    FHM = (TSTEP * TSTEP * 0.50) / HMAS;
    FOM = (TSTEP * TSTEP * 0.50) / OMAS;
    NMOL1 = NMOL - 1;
}

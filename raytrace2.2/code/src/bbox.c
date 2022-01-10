

#include <stdio.h>
#include <math.h>
#include "rt.h"

VOID InquireBoundBoxValues(pbb, minx, miny, minz, maxx, maxy, maxz)
BBOX *pbb;
REAL *minx, *miny, *minz;
REAL *maxx, *maxy, *maxz;
{
    *minx = pbb->dnear[0];
    *miny = pbb->dnear[1];
    *minz = pbb->dnear[2];
    *maxx = pbb->dfar[0];
    *maxy = pbb->dfar[1];
    *maxz = pbb->dfar[2];
}

VOID NormalizeBoundBox(pbb, mat)
BBOX *pbb;
MATRIX mat;
{
    POINT tmp;

    tmp[0] = pbb->dnear[0];
    tmp[1] = pbb->dnear[1];
    tmp[2] = pbb->dnear[2];
    tmp[3] = 1.0;

    VecMatMult(tmp, mat, tmp);

    pbb->dnear[0] = tmp[0];
    pbb->dnear[1] = tmp[1];
    pbb->dnear[2] = tmp[2];

    tmp[0] = pbb->dfar[0];
    tmp[1] = pbb->dfar[1];
    tmp[2] = pbb->dfar[2];
    tmp[3] = 1.0;

    VecMatMult(tmp, mat, tmp);

    pbb->dfar[0] = tmp[0];
    pbb->dfar[1] = tmp[1];
    pbb->dfar[2] = tmp[2];
}

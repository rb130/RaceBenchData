
#include <stdio.h>
#include <math.h>
#include "rt.h"

typedef struct sphere {
    POINT center;
    REAL rad;
    REAL rad2;
} SPHERE;

CHAR *SphName() {
    return ("sphere");
}

VOID SphPrint(po) OBJECT *po;
{
    INT i;
    SPHERE *ps;
    ELEMENT *pe;

    pe = po->pelem;
    fprintf(stderr, "\tSphere object\n");

    for (i = 0; i < po->numelements; i++) {
        ps = (SPHERE *)(pe->data);
        fprintf(stderr, "\t\tcenter  %f %f %f\n", ps->center[0], ps->center[1], ps->center[2]);
        fprintf(stderr, "\t\t        radius %f %f\n\n", ps->rad, ps->rad2);
        pe++;
    }
}

VOID SphElementBoundBox(pe, ps) ELEMENT *pe;
SPHERE *ps;
{
    BBOX *pbb;

    pbb = &(pe->bv);

    pbb->dnear[0] = ps->center[0] - ps->rad;
    pbb->dnear[1] = ps->center[1] - ps->rad;
    pbb->dnear[2] = ps->center[2] - ps->rad;

    pbb->dfar[0] = ps->center[0] + ps->rad;
    pbb->dfar[1] = ps->center[1] + ps->rad;
    pbb->dfar[2] = ps->center[2] + ps->rad;
}

VOID SphBoundBox(po) OBJECT *po;
{
    INT i;
    SPHERE *ps;
    ELEMENT *pe;
    BBOX *pbb;
    REAL minx, maxx;
    REAL miny, maxy;
    REAL minz, maxz;

    pe = po->pelem;
    pbb = &(po->bv);

    minx = miny = minz = HUGE_REAL;
    maxx = maxy = maxz = -HUGE_REAL;

    for (i = 0; i < po->numelements; i++) {
        ps = (SPHERE *)(pe->data);
        SphElementBoundBox(pe, ps);

        minx = Min(minx, pe->bv.dnear[0]);
        miny = Min(miny, pe->bv.dnear[1]);
        minz = Min(minz, pe->bv.dnear[2]);

        maxx = Max(maxx, pe->bv.dfar[0]);
        maxy = Max(maxy, pe->bv.dfar[1]);
        maxz = Max(maxz, pe->bv.dfar[2]);

        pe++;
    }

    pbb->dnear[0] = minx;
    pbb->dnear[1] = miny;
    pbb->dnear[2] = minz;

    pbb->dfar[0] = maxx;
    pbb->dfar[1] = maxy;
    pbb->dfar[2] = maxz;
}

VOID SphNormal(hit, Pi, Ni) IRECORD *hit;
POINT Pi;
POINT Ni;
{
    ELEMENT *pe;
    SPHERE *ps;

    pe = hit->pelem;
    ps = (SPHERE *)pe->data;
    VecSub(Ni, Pi, ps->center);

    Ni[0] /= ps->rad;
    Ni[1] /= ps->rad;
    Ni[2] /= ps->rad;
}

VOID SphDataNormalize(po, normMat) OBJECT *po;
MATRIX normMat;
{
    INT i;
    SPHERE *ps;
    ELEMENT *pe;
    POINT surf_point;
    POINT center_point;
    POINT rad_vector;

    NormalizeBoundBox(&po->bv, normMat);

    pe = po->pelem;

    for (i = 0; i < po->numelements; i++) {
        ps = (SPHERE *)pe->data;

        NormalizeBoundBox(&pe->bv, normMat);

        surf_point[0] = ps->center[0] + ps->rad;
        surf_point[1] = ps->center[1];
        surf_point[2] = ps->center[2];
        surf_point[3] = 1.0;

        center_point[0] = ps->center[0];
        center_point[1] = ps->center[1];
        center_point[2] = ps->center[2];
        center_point[3] = 1.0;

        VecMatMult(center_point, normMat, center_point);
        VecMatMult(surf_point, normMat, surf_point);

        VecSub(rad_vector, surf_point, center_point);
        VecCopy(ps->center, center_point);

        ps->rad = VecLen(rad_vector);
        ps->rad2 = ps->rad * ps->rad;

        pe++;
    }
}

INT SphPeIntersect(pr, pe, hit) RAY *pr;
ELEMENT *pe;
IRECORD *hit;
{
    INT nhits;
    REAL b, disc, t1, t2, vsq;
    SPHERE *ps;
    POINT V;
    IRECORD *sphhit;

    ps = (SPHERE *)(pe->data);
    sphhit = hit;

    VecSub(V, ps->center, pr->P);
    vsq = VecDot(V, V);
    b = VecDot(V, pr->D);

    if (vsq > ps->rad2 && b < RAYEPS) {
        return (0);
    }

    disc = b * b - vsq + ps->rad2;
    if (disc < 0.0) {
        return (0);
    }

    disc = sqrt(disc);
    t2 = b + disc;
    t1 = b - disc;

    if (t2 <= RAYEPS) {
        return (0);
    }

    nhits = 0;
    if (t1 > RAYEPS) {
        IsectAdd(sphhit, t1, pe);
        sphhit++;
        nhits++;
    }

    IsectAdd(sphhit, t2, pe);
    nhits++;

    return (nhits);
}

INT SphIntersect(pr, po, hit) RAY *pr;
OBJECT *po;
IRECORD *hit;
{
    INT i;
    INT nhits;
    ELEMENT *pe;
    IRECORD newhit[2];

    nhits = 0;
    pe = po->pelem;
    hit[0].t = HUGE_REAL;

    for (i = 0; i < po->numelements; i++) {
        if (SphPeIntersect(pr, pe, newhit)) {
            if (newhit[0].t < hit[0].t) {
                nhits++;
                hit[0].t = newhit[0].t;
                hit[0].pelem = newhit[0].pelem;
            }
        }
        pe++;
    }

    return (nhits);
}

VOID SphTransform(po, xtrans, xinvT) OBJECT *po;
MATRIX xtrans;
MATRIX xinvT;
{
    INT i;
    INT numelems;
    REAL new_rad;
    SPHERE *ps;
    ELEMENT *pe;
    POINT surf_point;
    POINT center_point;
    POINT rad_vector;

    pe = po->pelem;
    numelems = po->numelements;

    for (i = 0; i < numelems; i++) {
        ps = (SPHERE *)pe->data;

        surf_point[0] = ps->center[0] + ps->rad;
        surf_point[1] = ps->center[1];
        surf_point[2] = ps->center[2];
        surf_point[3] = 1.0;

        center_point[0] = ps->center[0];
        center_point[1] = ps->center[1];
        center_point[2] = ps->center[2];
        center_point[3] = 1.0;

        VecMatMult(center_point, xtrans, center_point);
        VecMatMult(surf_point, xtrans, surf_point);

        VecSub(rad_vector, surf_point, center_point);
        VecCopy(ps->center, center_point);

        new_rad = VecLen(rad_vector);

        if (new_rad != ps->rad) {
            ps->rad = new_rad;
            ps->rad2 = ps->rad * ps->rad;
        }

        pe++;
    }
}

VOID SphRead(po, pf) OBJECT *po;
FILE *pf;
{
    INT i;
    INT instat;
    SPHERE *ps;
    ELEMENT *pe;

    pe = po->pelem;
    ps = GlobalMalloc(sizeof(SPHERE) * po->numelements, "sph.c");

    for (i = 0; i < po->numelements; i++) {
        instat = fscanf(pf, "%lf %lf %lf %lf", &(ps->center[0]), &(ps->center[1]), &(ps->center[2]), &(ps->rad));

        if (instat != 4) {
            printf("Error in SphRead: sphere %ld.\n", i);
            exit(1);
        }

        ps->center[3] = 1.0;
        ps->rad2 = ps->rad * ps->rad;

        pe->data = (CHAR *)ps;
        pe->parent = po;

        SphElementBoundBox(pe, ps);

        ps++;
        pe++;
    }
}

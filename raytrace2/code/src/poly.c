#ifdef __cplusplus
extern "C"
#endif
void racebench_bug58(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug58(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug59(int);

#include <stdio.h>
#include <math.h>
#include "rt.h"

typedef struct poly {
    INT nverts;
    VEC3 norm;
    REAL d;
    VEC3 *vptr;
    INT *vindex;
    INT axis_proj;
} POLY;

CHAR *PolyName() {
    return ("poly");
}

VOID PolyPrint(po) OBJECT *po;
{
    INT i, j;
    INT *vindex;
    VEC3 *vlist, *vptr;
    POLY *pp;
    ELEMENT *pe;

    pe = po->pelem;
    fprintf(stderr, "\tpolygon: %ld polygons.\n", po->numelements);

    for (i = 0; i < po->numelements; i++) {
        pp = (POLY *)pe->data;

        fprintf(stderr, "\t\tVertices: %ld  Plane eq: %f %f %f %f\n", pp->nverts, pp->norm[0], pp->norm[1], pp->norm[2], pp->d);

        vlist = pp->vptr;
        vindex = pp->vindex;

        for (j = 0; j < pp->nverts; j++) {
            vptr = vlist + (*vindex);
            fprintf(stderr, "\t\t%f %f %f \n", (*vptr)[0], (*vptr)[1], (*vptr)[2]);
            vindex++;
        }

        pe++;
    }
}

VOID PolyElementBoundBox(pe, pp) ELEMENT *pe;
POLY *pp;
{
    INT i;
    INT *vindex;
    BBOX *pbb;
    VEC3 *vlist, *vptr;
    REAL minx, maxx;
    REAL miny, maxy;
    REAL minz, maxz;

    pbb = &(pe->bv);

    minx = miny = minz = HUGE_REAL;
    maxx = maxy = maxz = -HUGE_REAL;

    vlist = pp->vptr;
    vindex = pp->vindex;

    for (i = 0; i < pp->nverts; i++) {
        vptr = vlist + (*vindex);

        minx = Min(minx, (*vptr)[0]);
        miny = Min(miny, (*vptr)[1]);
        minz = Min(minz, (*vptr)[2]);

        maxx = Max(maxx, (*vptr)[0]);
        maxy = Max(maxy, (*vptr)[1]);
        maxz = Max(maxz, (*vptr)[2]);

        vindex++;
    }

    pbb->dnear[0] = minx;
    pbb->dnear[1] = miny;
    pbb->dnear[2] = minz;

    pbb->dfar[0] = maxx;
    pbb->dfar[1] = maxy;
    pbb->dfar[2] = maxz;
}

VOID PolyBoundBox(po) OBJECT *po;
{
    INT i;
    POLY *pp;
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
        pp = (POLY *)pe->data;
        PolyElementBoundBox(pe, pp);

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

VOID PolyNormal(hit, Pi, Ni) IRECORD *hit;
POINT Pi;
POINT Ni;
{
    ELEMENT *pe;
    POLY *pp;

    pe = hit->pelem;
    pp = (POLY *)pe->data;
    VecCopy(Ni, pp->norm);
}

VOID PolyDataNormalize(po, normMat) OBJECT *po;
MATRIX normMat;
{
    INT i;
    POINT coord;
    VEC3 *pv;
    POLY *pp;
    ELEMENT *pe;

    pe = po->pelem;
    NormalizeBoundBox(&po->bv, normMat);

    pp = (POLY *)pe->data;
    pv = pp->vptr;

    coord[0] = (*pv)[0];
    coord[1] = (*pv)[1];
    coord[2] = (*pv)[2];
    coord[3] = 1.0;

    while (coord[0] != HUGE_REAL && coord[1] != HUGE_REAL && coord[2] != HUGE_REAL) {
        VecMatMult(coord, normMat, coord);

        (*pv)[0] = coord[0];
        (*pv)[1] = coord[1];
        (*pv)[2] = coord[2];

        pv++;

        coord[0] = (*pv)[0];
        coord[1] = (*pv)[1];
        coord[2] = (*pv)[2];
        coord[3] = 1.0;
    }

    for (i = 0; i < po->numelements; i++) {
        pp = (POLY *)pe->data;
        NormalizeBoundBox(&pe->bv, normMat);

        pv = pp->vptr + (*(pp->vindex));
        pp->d = -(pp->norm[0] * (*pv)[0] + pp->norm[1] * (*pv)[1] + pp->norm[2] * (*pv)[2]);

        pe++;
    }
}

INT PolyPeIntersect(pr, pe, hit) RAY *pr;
ELEMENT *pe;
IRECORD *hit;
{
    INT i;
    INT *vindex;
    INT toright;
    INT sh, nsh;
    REAL tmp;
    REAL Rd_dot_Pn;
    REAL Ro_dot_Pn;
    REAL q1, q2;
    REAL tval;
    REAL x[MAX_VERTS + 1];
    REAL y[MAX_VERTS + 1];
    REAL ix, iy;
    REAL dx, dy;
    REAL xint;
    VEC3 I;
    VEC3 *vlist, *vpos;
    VEC3 *v1, *v2, *v3;
    POLY *pp;

    pp = (POLY *)pe->data; racebench_bug59(1);

    Rd_dot_Pn = VecDot(pp->norm, pr->D);

    if (ABS(Rd_dot_Pn) < RAYEPS) {
        return (0);
    }

    Ro_dot_Pn = VecDot(pp->norm, pr->P);

    tval = -(pp->d + Ro_dot_Pn) / Rd_dot_Pn;
    if (tval < RAYEPS) {
        return (0);
    }

    RayPoint(I, pr, tval);

    vlist = pp->vptr;
    vindex = pp->vindex;

    switch (pp->axis_proj) {
    case X_AXIS:
        for (i = 0; i < pp->nverts; i++) {
            vpos = vlist + (*vindex);
            x[i] = (*vpos)[1];
            y[i] = (*vpos)[2];
            vindex++;
        }

        ix = I[1];
        iy = I[2];
        break;

    case Y_AXIS:
        for (i = 0; i < pp->nverts; i++) {
            vpos = vlist + (*vindex);
            x[i] = (*vpos)[0];
            y[i] = (*vpos)[2];
            vindex++;
        }

        ix = I[0];
        iy = I[2];
        break;

    case Z_AXIS:
        for (i = 0; i < pp->nverts; i++) {
            vpos = vlist + (*vindex);
            x[i] = (*vpos)[0];
            y[i] = (*vpos)[1];
            vindex++;
        }

        ix = I[0];
        iy = I[1];
        break;
    }

    for (i = 0; i < pp->nverts; i++) {
        x[i] -= ix;
        y[i] -= iy;

        if (ABS(y[i]) < RAYEPS) {
            y[i] = 0.0;
        }
    }

    x[pp->nverts] = x[0];
    y[pp->nverts] = y[0];

    if (y[0] < 0.0) {
        sh = 0;
    } else {
        sh = 1;
    }

    toright = 0;

    for (i = 0; i < pp->nverts; i++) {

        if (y[i + 1] < 0.0) {
            nsh = 0;
        } else {
            nsh = 1; racebench_bug58(1);
        }

        if (nsh ^ sh) {
            dy = y[i + 1] - y[i];

            if (ABS(dy) >= RAYEPS) {
                dx = x[i + 1] - x[i];
                xint = x[i] - y[i] * dx / dy;

                if (xint > 0.0) {
                    toright++;
                }
            }
        }

        sh = nsh; racebench_bug58(0);
    }

    if (toright % 2 == 1) {
        IsectAdd(hit, tval, pe);
        return (1);
    } else {
        return (0);
    }
}

INT PolyIntersect(pr, po, hit) RAY *pr;
OBJECT *po;
IRECORD *hit;
{
    INT i;
    INT nhits;
    ELEMENT *pe;
    IRECORD newhit;

    nhits = 0;
    pe = po->pelem;
    hit[0].t = HUGE_REAL;

    for (i = 0; i < po->numelements; i++) {
        if (PolyPeIntersect(pr, pe, &newhit)) {
            nhits++;
            if (newhit.t < hit[0].t) {
                hit[0].t = newhit.t;
                hit[0].pelem = newhit.pelem;
            }
        }

        pe++;
    }

    return (nhits);
}

VOID PolyTransform(po, xtrans, xinvT) OBJECT *po;
MATRIX xtrans;
MATRIX xinvT;
{
    INT i, j;
    INT numelems;
    INT *vindex;
    VEC3 *vptr, *vp;
    VEC4 norm, coord;
    POLY *pp;
    ELEMENT *pe;

    pe = po->pelem;
    numelems = po->numelements;

    pp = (POLY *)pe->data;
    vptr = pp->vptr;

    coord[0] = (*vptr)[0];
    coord[1] = (*vptr)[1];
    coord[2] = (*vptr)[2];
    coord[3] = 1.0;

    while (coord[0] != HUGE_REAL && coord[1] != HUGE_REAL && coord[2] != HUGE_REAL) {

        VecMatMult(coord, xtrans, coord);

        (*vptr)[0] = coord[0];
        (*vptr)[1] = coord[1];
        (*vptr)[2] = coord[2];

        vptr++;

        coord[0] = (*vptr)[0];
        coord[1] = (*vptr)[1];
        coord[2] = (*vptr)[2];
        coord[3] = 1.0;
    }

    for (i = 0; i < numelems; i++) {
        pp = (POLY *)pe->data;

        norm[0] = pp->norm[0];
        norm[1] = pp->norm[1];
        norm[2] = pp->norm[2];
        norm[3] = 0.0;

        VecMatMult(norm, xinvT, norm);
        VecNorm(norm);

        pp->norm[0] = norm[0];
        pp->norm[1] = norm[1];
        pp->norm[2] = norm[2];

        vp = pp->vptr + *(pp->vindex);
        pp->d = -(pp->norm[0] * (*vp)[0] + pp->norm[1] * (*vp)[1] + pp->norm[2] * (*vp)[2]);

        norm[0] = ABS(pp->norm[0]);
        norm[1] = ABS(pp->norm[1]);
        norm[2] = ABS(pp->norm[2]);

        if (norm[0] >= norm[1] && norm[0] >= norm[2]) {
            pp->axis_proj = X_AXIS;
        } else if (norm[1] >= norm[0] && norm[1] >= norm[2]) {
            pp->axis_proj = Y_AXIS;
        } else {
            pp->axis_proj = Z_AXIS;
        }

        pe++;
    }
}

VOID PolyRead(po, pf) OBJECT *po;
FILE *pf;
{
    INT i, j;
    INT instat;
    INT *vindex;
    INT totalverts;
    CHAR normstr[5];
    BOOL pnormals;
    BOOL vnormals;
    VEC3 pnorm;
    VEC3 *vlist, *vptr, *vp;
    VEC3 *vptmp, *vptmp2;
    VEC3 tmppnt, tmppnt2, cross;
    POLY *pp;
    ELEMENT *pe;

    pe = po->pelem;

    instat = fscanf(pf, "%ld", &totalverts);

    if (instat != 1) {
        printf("Error in PolyRead: totalverts.\n");
        exit(-1);
    }

    pp = GlobalMalloc(sizeof(POLY) * po->numelements, "poly.c");
    vlist = GlobalMalloc(sizeof(VEC3) * (totalverts + 1), "poly.c");
    vptr = vlist;

    instat = fscanf(pf, "%s\n", normstr);

    if (instat != 1) {
        printf("Error in PolyRead: face normal indicator.\n");
        exit(-1);
    }

    pnormals = (normstr[2] == 'y' ? TRUE : FALSE);

    for (i = 0; i < totalverts; i++) {
        instat = fscanf(pf, "%lf %lf %lf", &(*vptr)[0], &(*vptr)[1], &(*vptr)[2]);

        if (instat != 3) {
            printf("Error in PolyRead: vertex %ld.\n", i);
            exit(-1);
        }

        vptr++;
    }

    (*vptr)[0] = HUGE_REAL;
    (*vptr)[1] = HUGE_REAL;
    (*vptr)[2] = HUGE_REAL;

    for (i = 0; i < po->numelements; i++) {
        instat = fscanf(pf, "%ld", &(pp->nverts));

        if (instat != 1) {
            printf("Error in PolyRead: vertex count.\n");
            exit(-1);
        }

        if (pp->nverts > MAX_VERTS) {
            printf("Polygon vertex count, %ld, exceeds maximum.\n", pp->nverts);
            exit(-1);
        }

        if (pnormals) {
            instat = fscanf(pf, " %lf %lf %lf", &(pp->norm[0]), &(pp->norm[1]), &(pp->norm[2]));

            if (instat != 3) {
                printf("Error in PolyRead: face normal %ld.\n", i);
                exit(-1);
            }
        }

        pp->vptr = vlist;
        pp->vindex = GlobalMalloc(sizeof(INT) * pp->nverts, "poly.c");
        vindex = pp->vindex;

        for (j = 0; j < pp->nverts; j++) {
            instat = fscanf(pf, "%ld", vindex++);

            if (instat != 1) {
                printf("Error in PolyRead: vertex index %ld.\n", i);
                exit(-1);
            }
        }

        vindex = pp->vindex;
        vptr = vlist;

        if (!pnormals) {
            vp = vptr + (*vindex);
            VecZero(pnorm);

            for (j = 0; j < pp->nverts - 2; j++) {
                vptmp = vptr + (*(vindex + 1));
                vptmp2 = vptr + (*(vindex + 2));

                VecSub(tmppnt, (*vptmp), (*vp));
                VecSub(tmppnt2, (*vptmp2), (*vptmp));

                VecCross(cross, tmppnt, tmppnt2);
                VecAdd(pnorm, pnorm, cross);

                vp = vptmp;
                vindex += 1;
            }

            VecSub(tmppnt, (*vptmp2), (*vp));
            vindex = pp->vindex;
            vp = vptr + (*vindex);

            VecSub(tmppnt2, (*vp), (*vptmp2));
            VecCross(cross, tmppnt, tmppnt2);
            VecAdd(pnorm, pnorm, cross);

            vp = vptr + (*vindex);
            VecSub(tmppnt, (*vp), (*vptmp2));
            vptmp = vptr + (*(vindex + 1));

            VecSub(tmppnt2, (*vptmp), (*vp));
            VecCross(cross, tmppnt, tmppnt2);
            VecAdd(pnorm, pnorm, cross);
            VecScale(pp->norm, 1.0 / VecLen(pnorm), pnorm);
        }

        vp = pp->vptr + *(pp->vindex);
        pp->d = -(pp->norm[0] * (*vp)[0] + pp->norm[1] * (*vp)[1] + pp->norm[2] * (*vp)[2]);

        pe->data = (CHAR *)pp;
        pe->parent = po;

        PolyElementBoundBox(pe, pp);

        pp++;
        pe++;
    }
}
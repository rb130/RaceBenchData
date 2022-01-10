

#include <stdio.h>
#include <math.h>
#include "rt.h"

VOID SpecularDirection(R, N, I)
POINT R;
POINT N;
POINT I;
{
    REAL I_dot_N;
    POINT N2;
    POINT vprime;

    I_dot_N = VecDot(I, N);
    I_dot_N = ABS(I_dot_N);
    I_dot_N = 1.0 / I_dot_N;

    VecScale(vprime, I_dot_N, I);
    VecScale(N2, 2.0, N);

    VecAdd(R, vprime, N2);
    VecNorm(R);
}

BOOL TransmissionDirection(T, N, I, kn)
POINT T;
POINT N;
POINT I;
REAL kn;
{
    POINT vprime;
    POINT vplusn;
    REAL I_dot_N;
    REAL kf;
    REAL vprime_sq;
    REAL vplusn_sq;

    I_dot_N = VecDot(I, N);
    I_dot_N = ABS(I_dot_N);
    I_dot_N = 1.0 / I_dot_N;

    VecScale(vprime, I_dot_N, I);
    VecAdd(vplusn, vprime, N);

    vprime_sq = VecDot(vprime, vprime);
    vplusn_sq = VecDot(vplusn, vplusn);

    kf = kn * kn * vprime_sq - vplusn_sq;

    if (kf > RAYEPS) {
        kf = 1.0 / sqrt(kf);

        VecScale(vplusn, kf, vplusn);
        VecSub(T, vplusn, N);
        VecNorm(T);
    } else {
        return (FALSE);
    }

    return (TRUE);
}

VOID Shade(iP, N, ray, hit, pid)
VEC3 iP;
VEC3 N;
RAY *ray;
IRECORD *hit;
INT pid;
{
    VEC3 Lvec;
    VEC3 Hvec;
    VEC3 Evec;
    RAY shad_ray;
    RAY secondary_ray;
    COLOR surfcol;
    COLOR col;
    REAL NdotL;
    REAL Diff;
    REAL NdotH;
    REAL spec;
    OBJECT *po;
    SURF *s;
    INT i, j;
    REAL lightlen;
    REAL shadtrans;
    LIGHT *lptr;

    po = hit->pelem->parent;
    s = po->surf;
    VecCopy(surfcol, s->fcolor);

    col[0] = View.ambient[0] * surfcol[0];
    col[1] = View.ambient[1] * surfcol[1];
    col[2] = View.ambient[2] * surfcol[2];

    VecCopy(shad_ray.P, iP);
    VecNegate(Evec, ray->D);

    lptr = lights;
    for (i = 0; i < nlights; i++) {
        VecSub(Lvec, lptr->pos, iP);
        lightlen = VecLen(Lvec);
        VecNorm(Lvec);
        VecCopy(shad_ray.D, Lvec);

        { pthread_mutex_lock(&(gm->ridlock)); };
        shad_ray.id = gm->rid++;
        { pthread_mutex_unlock(&(gm->ridlock)); };

        NdotL = VecDot(N, Lvec);

        if (NdotL > 0.0) {

            if (View.shad && !lptr->shadow) {
                switch (TraversalType) {
                case TT_LIST:
                    shadtrans = ShadowIntersect(&shad_ray, lightlen, hit->pelem);
                    break;

                case TT_HUG:
                    shadtrans = HuniformShadowIntersect(&shad_ray, lightlen, hit->pelem, pid);
                    break;
                }
            } else {
                shadtrans = 1.0;
            }

            if (shadtrans > 0.0) {
                Diff = po->surf->kdiff * NdotL * shadtrans;

                col[0] += surfcol[0] * lptr->col[0] * Diff;
                col[1] += surfcol[1] * lptr->col[1] * Diff;
                col[2] += surfcol[2] * lptr->col[2] * Diff;

                if (s->kspec > 0.0) {
                    VecAdd(Hvec, Lvec, Evec);
                    VecNorm(Hvec);
                    NdotH = VecDot(N, Hvec);

                    if (NdotH > 0.0) {
                        spec = pow(NdotH, s->kspecn);
                        spec *= s->kspec;

                        col[0] += lptr->col[0] * spec;
                        col[1] += lptr->col[1] * spec;
                        col[2] += lptr->col[2] * spec;
                    }
                }
            }
        }

        lptr = lptr->next;
    }

    VecScale(col, ray->weight, col);
    AddPixelColor(col, ray->x, ray->y);

    if ((ray->level) + 1 < Display.maxlevel) {
        VecCopy(secondary_ray.P, iP);

        secondary_ray.weight = po->surf->kspec * ray->weight;

        if (secondary_ray.weight > Display.minweight) {
            SpecularDirection(secondary_ray.D, N, ray->D);
            secondary_ray.level = ray->level + 1;

            { pthread_mutex_lock(&(gm->ridlock)); };
            secondary_ray.id = gm->rid++;
            { pthread_mutex_unlock(&(gm->ridlock)); };

            secondary_ray.x = ray->x;
            secondary_ray.y = ray->y;

            PushRayTreeStack(&secondary_ray, pid);
        }

        secondary_ray.weight = po->surf->ktran * ray->weight;

        if (secondary_ray.weight > Display.minweight) {
            if (TransmissionDirection(secondary_ray.D, N, ray->D, po->surf->refrindex)) {
                secondary_ray.level = ray->level + 1;

                { pthread_mutex_lock(&(gm->ridlock)); };
                secondary_ray.id = gm->rid++;
                { pthread_mutex_unlock(&(gm->ridlock)); };

                secondary_ray.x = ray->x;
                secondary_ray.y = ray->y;

                PushRayTreeStack(&secondary_ray, pid);
            }
        }
    }
}



#include <stdio.h>
#include <math.h>
#include "rt.h"

REAL frand() {
    REAL r;
    static LONG lLastRand = 0;

    lLastRand = lLastRand * 214013L + 2531011L;
    r = (REAL)((lLastRand >> 16) & 0x7FFF) / 32768.0;
    return (r);
}

BOOL GetRayJobFromBundle(job, x, y)
RAYJOB *job;
INT *x, *y;
{
    *x = job->xcurr;
    *y = job->ycurr;

    if ((job->y + job->ylen) == job->ycurr) {
        return (FALSE);
    }

    job->xcurr++;
    if ((job->x + job->xlen) == job->xcurr) {
        job->xcurr = job->x;
        job->ycurr++;
    }

    return (TRUE);
}

VOID ConvertPrimRayJobToRayMsg(ray, x, y)
RAY *ray;
REAL x, y;
{
    VEC4 dir;
    VEC4 origin;

    if (View.projection == PT_PERSP) {
        dir[0] = -Display.scrHalfWidth + (x * Display.vWscale);
        dir[1] = Display.scrHalfHeight - (y * Display.vHscale);
        dir[2] = Display.scrDist;
        dir[3] = 0.0;

        TransformViewRay(dir);

        VecNorm(dir);
        VecCopy(ray->D, dir);
        VecCopy(ray->P, View.eye);
    } else {

        dir[0] = 0.0;
        dir[1] = 0.0;
        dir[2] = 1.0;
        dir[3] = 0.0;

        TransformViewRay(dir);
        VecNorm(dir);

        VecCopy(ray->D, dir);

        origin[0] = -Display.scrHalfWidth + (x * Display.vWscale);
        origin[1] = Display.scrHalfHeight - (y * Display.vHscale);
        origin[2] = 0.0;
        origin[3] = 1.0;

        TransformViewRay(origin);
        VecCopy(ray->P, origin);
    }

    ray->level = 0;
    ray->weight = 1.0 / (REAL)NumSubRays;

    { pthread_mutex_lock(&(gm->ridlock)); };
    ray->id = gm->rid++;
    { pthread_mutex_unlock(&(gm->ridlock)); };

    ray->x = (INT)x;
    ray->y = (INT)y;
}

VOID RayTrace(pid)
INT pid;
{
    INT i, j;
    INT x, y;
    REAL xx, yy;
    VEC3 N;
    VEC3 Ipoint;
    COLOR c;
    RAY *ray;
    RAY rmsg;
    RAYJOB job;
    OBJECT *po;
    BOOL hit;
    IRECORD hitrecord;

    ray = &rmsg;

    while (GetJobs(&job, pid) != WPS_EMPTY) {
        while (GetRayJobFromBundle(&job, &x, &y)) {

            xx = (REAL)x;
            yy = (REAL)y;

            if (AntiAlias) {
                for (j = 0; j < NumSubRays; j++) {
                    ConvertPrimRayJobToRayMsg(ray, xx + frand(), yy + frand());
                    PushRayTreeStack(ray, pid);
                }
            } else {
                ConvertPrimRayJobToRayMsg(ray, xx, yy);
                PushRayTreeStack(ray, pid);
            }

            while (PopRayTreeStack(ray, pid) != RTS_EMPTY) {

                switch (TraversalType) {
                case TT_LIST:
                    hit = Intersect(ray, &hitrecord);
                    break;

                case TT_HUG:
                    hit = TraverseHierarchyUniform(ray, &hitrecord, pid);
                    break;
                }

                if (hit) {

                    po = hitrecord.pelem->parent;

                    RayPoint(Ipoint, ray, hitrecord.t);

                    (*po->procs->normal)(&hitrecord, Ipoint, N);

                    if ((VecDot(ray->D, N)) > 0.0)
                        VecNegate(N, N);

                    Shade(Ipoint, N, ray, &hitrecord, pid);
                } else {

                    VecCopy(c, View.bkg);
                    VecScale(c, ray->weight, c);
                    AddPixelColor(c, ray->x, ray->y);
                }
            }
        }
    }
}

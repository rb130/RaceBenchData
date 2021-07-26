
#include <stdio.h>
#include <math.h>
#include "rt.h"

BOOL Intersect(pr, hit) RAY *pr;
IRECORD *hit;
{
    OBJECT *po;
    IRECORD newhit;

    po = gm->modelroot;
    hit->t = HUGE_REAL;
    hit->pelem = NULL;

    while (po) {
        if ((*po->procs->intersect)(pr, po, &newhit)) {
            if (newhit.t < hit[0].t) {
                *hit = newhit;
            }
        }

        po = po->next;
    }

    if (hit->t < HUGE_REAL) {
        return (TRUE);
    } else {
        return (FALSE);
    }
}

REAL ShadowIntersect(pr, lightdist, pe) RAY *pr;
REAL lightdist;
ELEMENT *pe;
{
    REAL trans;
    OBJECT *po;
    IRECORD newhit;

    trans = 1.0;
    po = gm->modelroot;

    while (po && trans > 0.0) {
        if ((*po->procs->intersect)(pr, po, &newhit) && newhit.pelem != pe && newhit.t < lightdist) {
            trans *= newhit.pelem->parent->surf->ktran;
        }

        po = po->next;
    }

    return (trans);
}

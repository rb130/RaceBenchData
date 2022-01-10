

#include <stdio.h>
#include <math.h>
#include "rt.h"

VOID Huniform_defaults() {
    hu_max_prims_cell = 2;
    hu_gridsize = 5;
    hu_numbuckets = 23;
    hu_max_subdiv_level = 1;
    hu_lazy = 0;
}

VOID BuildHierarchy_Uniform() {
    INT i;
    INT num;
    INT num_pe;
    INT status;
    REAL den;
    GRID *g;
    GRID *gr;
    GRID *ng;
    GRID testgrid;
    VOXEL *v;
    RAY r;
    RAYINFO *rinfo;
    ELEMENT **pepa;

    init_masks();

    pepa = MakeElementArray(&num_pe);

    v = init_world_voxel(pepa, num_pe);

    gm->world_level_grid = init_world_grid(v, pepa, num_pe);
    g = gm->world_level_grid;

    ng = create_grid(v, g, num_pe);

    fprintf(stderr, "Uniform Hierarchy built.\n");
}

VOID IntersectHuniformPrimlist(intersectPrim, hit, v, r, pid)
INT *intersectPrim;
IRECORD *hit;
VOXEL *v;
RAY *r;
INT pid;
{
    ELEMENT **pptr;
    OBJECT *peParent;
    ELEMENT *pe;
    IRECORD newhit[ISECT_MAX];
    INT hitcode, i;
    REAL t_out;

    t_out = r->ri->t_out;
    hit[0].t = HUGE_REAL;
    pptr = (ELEMENT **)v->cell;

    for (i = 0; i < v->numelements; i++) {
        pe = pptr[i];
        peParent = pe->parent;
        hitcode = (*peParent->procs->pe_intersect)(r, pe, newhit);

        if (hitcode) {
            if (newhit[0].t < hit[0].t && newhit[0].t < t_out) {
                hit[0] = newhit[0];
            }
        }
    }

    if (hit[0].t < HUGE_REAL) {
        *intersectPrim = TRUE;
    } else {
        *intersectPrim = FALSE;
    }
}

REAL HuniformShadowIntersect(r, lightlength, pe, pid)
RAY *r;
REAL lightlength;
ELEMENT *pe;
INT pid;
{
    INT status;
    INT hitcode, i;
    REAL trans;
    OBJECT *peParent;
    ELEMENT **pptr;
    ELEMENT *pe2;
    IRECORD newhit[ISECT_MAX];
    VOXEL *v;

    trans = 1.0;

    r->ri = NULL;
    v = init_ray(r, gm->world_level_grid);

    if (v == NULL) {
        reset_rayinfo(r);
        return (trans);
    }

    newhit[0].t = HUGE_REAL;
    status = IN_WORLD;

    while (trans > 0.0 && status != EXITED_WORLD) {

        pptr = (ELEMENT **)v->cell;

        for (i = 0; (i < v->numelements) && (trans > 0.0); i++) {
            pe2 = pptr[i];
            peParent = pe2->parent;
            hitcode = (*peParent->procs->pe_intersect)(r, pe2, newhit);

            if (hitcode && newhit[0].pelem != pe && newhit[0].t < lightlength) {
                trans *= newhit[0].pelem->parent->surf->ktran;
            }
        }

        if (trans > 0.0) {
            v = next_nonempty_leaf(r, STEP, &status);
        }
    }

    reset_rayinfo(r);
    return (trans);
}

BOOL TraverseHierarchyUniform(r, hit, pid)
RAY *r;
IRECORD *hit;
INT pid;
{
    INT status;
    INT intersectPrim;
    VOXEL *v;

    r->ri = NULL;
    v = init_ray(r, gm->world_level_grid);

    if (v == NULL) {
        reset_rayinfo(r);
        return (FALSE);
    }

    intersectPrim = FALSE;
    hit[0].t = HUGE_REAL;
    status = IN_WORLD;

    while (!intersectPrim && status != EXITED_WORLD) {
        IntersectHuniformPrimlist(&intersectPrim, hit, v, r, pid);

        if (!intersectPrim) {
            v = next_nonempty_leaf(r, STEP, &status);
        }
    }

    reset_rayinfo(r);
    return (intersectPrim);
}

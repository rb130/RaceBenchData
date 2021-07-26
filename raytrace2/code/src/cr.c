
#include <stdio.h>
#include <math.h>
#include "rt.h"

GRID *gridlist = NULL;

VOID prn_gridlist() {
    GRID *g;

    fprintf(stderr, "    Print Gridlist \n");
    g = gridlist;

    while (g != NULL) {
        prn_grid(g);
        g = g->next;
    }

    fprintf(stderr, "    End Gridlist \n");
}

VOID prn_ds_stats() {
    INT leafs;
    INT voxels;

    printf("\n");
    printf("****** Hierarchical uniform grid data structure statistics ******\n\n");

    printf("    Data structure evaluated as a preprocess.\n");

    printf("    gridsize                             %3ld \n", hu_gridsize);
    printf("    hashtable buckets                    %3ld \n", hu_numbuckets);
    printf("    maximum subdivision level            %3ld \n", hu_max_subdiv_level);
    printf("    maximum primitives / cell            %3ld \n", hu_max_prims_cell);
    printf("    grids                                %3ld \n", grids);

    voxels = empty_voxels + nonempty_voxels;
    leafs = empty_voxels + nonempty_leafs;

    printf("    empty voxels                    %8ld    %3ld %% \n", empty_voxels, 100 * empty_voxels / voxels);
    printf("    nonempty voxels                 %8ld    %3ld %% \n", nonempty_voxels, 100 * nonempty_voxels / voxels);
    printf("    empty leafs                     %8ld    %3ld %% \n", empty_voxels, 100 * empty_voxels / leafs);
    printf("    nonempty leafs                  %8ld    %3ld %% \n", nonempty_leafs, 100 * nonempty_leafs / leafs);
    printf("    primitives / leaf                 %6.1lf \n", (double)prims_in_leafs / leafs);
    printf("\n");
}

VOID init_masks() {
    INT n, i;

    n = sizeof(UINT) * 8;

    for (i = 0; i < n; i++) {
        empty_masks[i] = (MSB >> i);
        nonempty_masks[i] = ~empty_masks[i];
    }
}

GRID *init_world_grid(v, pepa, num_pe) VOXEL *v;
ELEMENT **pepa;
INT num_pe;
{
    UINT *ec;
    UINT *pc;
    BBOX wbox;
    GRID *g;
    VOXEL **ht;

    g = ObjectMalloc(OT_GRID, 1);
    g->id = grids++;

    ht = ObjectMalloc(OT_HASHTABLE, 1);

    g->hashtable = ht;
    g->hashtable[0] = v;

    ec = ObjectMalloc(OT_EMPTYCELLS, 1);

    g->emptycells = ec;
    g->emptycells[0] = 0;

    g->indx_inc[0] = 1;
    g->indx_inc[1] = 1;
    g->indx_inc[2] = 1;
    g->num_buckets = 1;

    wbox.dnear[0] = 0.0;
    wbox.dnear[1] = 0.0;
    wbox.dnear[2] = 0.0;
    wbox.dfar[0] = 1.0;
    wbox.dfar[1] = 1.0;
    wbox.dfar[2] = 1.0;

    g->min[0] = wbox.dnear[0];
    g->min[1] = wbox.dnear[1];
    g->min[2] = wbox.dnear[2];

    g->cellsize[0] = wbox.dfar[0] - wbox.dnear[0];
    g->cellsize[1] = wbox.dfar[1] - wbox.dnear[1];
    g->cellsize[2] = wbox.dfar[2] - wbox.dnear[2];

    g->subdiv_level = -1;
    g->num_prims = num_pe;
    g->pepa = pepa;
    g->next = NULL;

    gridlist = g;
    return (g);
}

VOXEL *init_world_voxel(pepa, numelements) ELEMENT **pepa;
INT numelements;
{
    VOXEL *v;

    v = ObjectMalloc(OT_VOXEL, 1);

    nonempty_voxels++;

    v->id = 0;
    v->cell = (CHAR *)pepa;
    v->numelements = numelements;
    v->celltype = GSM_VOXEL;
    v->next = NULL;

    return (v);
}

VOID mark_empty(index1D, g) INT index1D;
GRID *g;
{
    INT i, r;
    UINT w;

    i = index1D / (sizeof(UINT) * 8);
    r = index1D - i * sizeof(UINT) * 8;

    w = g->emptycells[i];
    w = w | empty_masks[r];
    g->emptycells[i] = w;
}

VOID mark_nonempty(index1D, g) INT index1D;
GRID *g;
{
    INT i, r;
    UINT w;

    i = index1D / (sizeof(UINT) * 8);
    r = index1D - i * sizeof(UINT) * 8;

    w = g->emptycells[i];
    w = w & nonempty_masks[r];
    g->emptycells[i] = w;
}

VOID insert_in_hashtable(v, g) VOXEL *v;
GRID *g;
{
    INT i, r;
    VOXEL *vht;

    i = v->id / hu_numbuckets;
    r = v->id - i * hu_numbuckets;

    vht = g->hashtable[r];
    v->next = vht;
    g->hashtable[r] = v;
}

ELEMENT **prims_in_box2(pepa, n_in, b, n) ELEMENT **pepa;
INT n_in;
BBOX b;
INT *n;
{
    INT ovlap, i, j, k;
    ELEMENT *pe;
    ELEMENT **npepa;
    BBOX bb;
    REAL max, side, eps;

    max = b.dfar[0] - b.dnear[0];
    side = b.dfar[1] - b.dnear[1];

    max = max > side ? max : side;
    side = b.dfar[2] - b.dnear[2];

    max = max > side ? max : side;
    eps = max * 1.0E-6;

    if (n_in > 0) {
        npepa = ObjectMalloc(OT_PEPARRAY, n_in);
    } else {
        npepa == NULL;
        *n = 0;
        return (npepa);
    }

    *n = 0;
    k = 0;

    for (j = 0; j < n_in; j++) {
        pe = pepa[j];
        bb = pe->bv;
        ovlap = 1;

        for (i = 0; i < 1; i++) {
            if (b.dnear[0] > bb.dfar[0] + eps) {
                ovlap = 0;
                break;
            }

            if (b.dnear[1] > bb.dfar[1] + eps) {
                ovlap = 0;
                break;
            }

            if (b.dnear[2] > bb.dfar[2] + eps) {
                ovlap = 0;
                break;
            }

            if (b.dfar[0] < bb.dnear[0] - eps) {
                ovlap = 0;
                break;
            }

            if (b.dfar[1] < bb.dnear[1] - eps) {
                ovlap = 0;
                break;
            }

            if (b.dfar[2] < bb.dnear[2] - eps) {
                ovlap = 0;
                break;
            }
        }

        if (ovlap == 1) {
            npepa[k++] = pepa[j];
            (*n)++;
        }
    }

    return (npepa);
}

BTNODE *init_bintree(ng) GRID *ng;
{
    BTNODE *btn;
    ELEMENT **pepa;

    btn = ObjectMalloc(OT_BINTREE, 1);

    btn->btn[0] = NULL;
    btn->btn[1] = NULL;
    btn->axis = -1;

    btn->p[0] = ng->min[0];
    btn->p[1] = ng->min[1];
    btn->p[2] = ng->min[2];

    btn->n[0] = ng->indx_inc[1];
    btn->n[1] = ng->indx_inc[1];
    btn->n[2] = ng->indx_inc[1];

    btn->i[0] = 0;
    btn->i[1] = 0;
    btn->i[2] = 0;

    btn->nprims = ng->num_prims;
    btn->totalPrimsAllocated = btn->nprims;

    if (ng->num_prims > 0) {
        btn->pe = ng->pepa;
    } else {
        btn->pe = NULL;
    }

    return (btn);
}

VOID subdiv_bintree(btn, g) BTNODE *btn;
GRID *g;
{
    BTNODE *btn1, *btn2;
    INT n1, n2, imax, dmax;
    BBOX b1, b2;

    imax = 0;
    dmax = btn->n[0];

    if (dmax < btn->n[1]) {
        imax = 1;
        dmax = btn->n[1];
    }

    if (dmax < btn->n[2]) {
        imax = 2;
        dmax = btn->n[2];
    }

    btn->axis = imax;
    btn->btn[0] = NULL;
    btn->btn[1] = NULL;

    if (btn->n[imax] > 1) {
        n1 = btn->n[imax] / 2;
        n2 = btn->n[imax] - n1;

        btn1 = ObjectMalloc(OT_BINTREE, 1);
        btn2 = ObjectMalloc(OT_BINTREE, 1);

        btn->btn[0] = btn1;
        btn->btn[1] = btn2;

        btn1->btn[0] = NULL;
        btn1->btn[1] = NULL;
        btn1->axis = -1;

        btn2->btn[0] = NULL;
        btn2->btn[1] = NULL;
        btn2->axis = -1;

        btn1->i[0] = btn->i[0];
        btn1->i[1] = btn->i[1];
        btn1->i[2] = btn->i[2];

        btn2->i[0] = btn->i[0];
        btn2->i[1] = btn->i[1];
        btn2->i[2] = btn->i[2];
        btn2->i[imax] += n1;

        btn1->n[0] = btn->n[0];
        btn1->n[1] = btn->n[1];
        btn1->n[2] = btn->n[2];
        btn1->n[imax] = n1;

        btn2->n[0] = btn->n[0];
        btn2->n[1] = btn->n[1];
        btn2->n[2] = btn->n[2];
        btn2->n[imax] = n2;

        btn1->p[0] = btn->p[0];
        btn1->p[1] = btn->p[1];
        btn1->p[2] = btn->p[2];

        btn2->p[0] = btn->p[0];
        btn2->p[1] = btn->p[1];
        btn2->p[2] = btn->p[2];
        btn2->p[imax] = btn->p[imax] + n1 * g->cellsize[imax];

        b1.dnear[0] = btn1->p[0];
        b1.dnear[1] = btn1->p[1];
        b1.dnear[2] = btn1->p[2];
        b1.dfar[0] = btn1->p[0] + btn1->n[0] * g->cellsize[0];
        b1.dfar[1] = btn1->p[1] + btn1->n[1] * g->cellsize[1];
        b1.dfar[2] = btn1->p[2] + btn1->n[2] * g->cellsize[2];

        btn1->pe = prims_in_box2(btn->pe, btn->nprims, b1, &(btn1->nprims));
        btn1->totalPrimsAllocated = btn->nprims;

        b2.dnear[0] = btn2->p[0];
        b2.dnear[1] = btn2->p[1];
        b2.dnear[2] = btn2->p[2];
        b2.dfar[0] = btn2->p[0] + btn2->n[0] * g->cellsize[0];
        b2.dfar[1] = btn2->p[1] + btn2->n[1] * g->cellsize[1];
        b2.dfar[2] = btn2->p[2] + btn2->n[2] * g->cellsize[2];

        btn2->pe = prims_in_box2(btn->pe, btn->nprims, b2, &(btn2->nprims));
        btn2->totalPrimsAllocated = btn->nprims;
    }

    if (btn1->n[0] == 1 && btn1->n[1] == 1 && btn1->n[2] == 1) {
    }

    if (btn2->n[0] == 1 && btn2->n[1] == 1 && btn2->n[2] == 1) {
    }
}

VOID create_bintree(root, g) BTNODE *root;
GRID *g;
{
    BTNODE *btn;

    btn = root;
    if (btn->n[0] != 1 || btn->n[1] != 1 || btn->n[2] != 1) {
        subdiv_bintree(btn, g);
        create_bintree(btn->btn[0], g);
        create_bintree(btn->btn[1], g);

        if ((btn->nprims) > 0) {
        }
    }
}

ELEMENT **bintree_lookup(root, i, j, k, g, n) BTNODE *root;
INT i, j, k;
GRID *g;
INT *n;
{
    INT l, x;
    INT ijk[3];
    INT child;
    INT idiv;
    ELEMENT **pepa;
    BTNODE *btn;

    ijk[0] = i;
    ijk[1] = j;
    ijk[2] = k;

    btn = root;

    if (btn == NULL) {
        *n = 0;
        return (NULL);
    }

    while (btn->n[0] != 1 || btn->n[1] != 1 || btn->n[2] != 1) {
        if (btn->axis == -1) {

            fprintf(stderr, "bintree_lookup: gridsizes (%ld, %ld, %ld), axis %ld & nprims %ld\n", btn->n[0], btn->n[1], btn->n[2], btn->axis, btn->nprims);

            exit(-1);
        }

        child = 0;
        idiv = (btn->n[btn->axis] / 2);

        if (ijk[btn->axis] + 1 > idiv) {
            child = 1;
            ijk[btn->axis] -= idiv;
        }

        btn = btn->btn[child];
        if (btn == NULL) {
            *n = 0;
            return (NULL);
        }
    }

    *n = btn->nprims;
    pepa = btn->pe;

    return (pepa);
}

VOID deleteBinTree(binTree) BTNODE *binTree;
{
    BTNODE *left, *right;

    if (binTree != NULL) {
        left = binTree->btn[0];
        right = binTree->btn[1];

        deleteBinTree(left);
        deleteBinTree(right);
    }
}

GRID *create_grid(v, g, num_prims) VOXEL *v;
GRID *g;
INT num_prims;
{
    INT n;
    INT i, j, k, r;
    INT nprims;
    INT index1D;
    UINT *ec;
    UINT *pc;
    R64 nec, unsgn, ncells;
    GRID *ng, *nng;
    BBOX b;
    VOXEL *nv;
    VOXEL **ht;
    BTNODE *bintree;
    ELEMENT **pepa;

    ng = ObjectMalloc(OT_GRID, 1);
    ng->id = grids++;

    ht = ObjectMalloc(OT_HASHTABLE, hu_numbuckets);
    ng->hashtable = ht;

    ncells = (R64)(hu_gridsize * hu_gridsize * hu_gridsize);
    total_cells += ncells;

    ec = ObjectMalloc(OT_EMPTYCELLS, (INT)ncells);
    ng->emptycells = ec;

    ng->num_prims = num_prims;
    ng->pepa = (ELEMENT **)v->cell;
    ng->indx_inc[0] = 1;
    ng->indx_inc[1] = hu_gridsize;
    ng->indx_inc[2] = hu_gridsize * hu_gridsize;
    ng->num_buckets = hu_numbuckets;

    k = v->id / g->indx_inc[2];
    r = v->id - k * g->indx_inc[2];
    j = r / g->indx_inc[1];
    i = r - j * g->indx_inc[1];

    ng->min[0] = g->min[0] + i * g->cellsize[0];
    ng->min[1] = g->min[1] + j * g->cellsize[1];
    ng->min[2] = g->min[2] + k * g->cellsize[2];

    ng->cellsize[0] = g->cellsize[0] / ng->indx_inc[1];
    ng->cellsize[1] = g->cellsize[1] / ng->indx_inc[1];
    ng->cellsize[2] = g->cellsize[2] / ng->indx_inc[1];
    ng->subdiv_level = g->subdiv_level + 1;
    ng->next = gridlist;

    gridlist = ng;

    bintree = init_bintree(ng);
    create_bintree(bintree, ng);

    index1D = 0;
    n = hu_gridsize;

    for (k = 0; k < n; k++) {
        for (j = 0; j < n; j++) {
            for (i = 0; i < n; i++) {
                pepa = bintree_lookup(bintree, i, j, k, ng, &nprims);

                if (pepa != NULL) {
                    nonempty_voxels++;
                    mark_nonempty(index1D, ng);

                    nv = ObjectMalloc(OT_VOXEL, 1);

                    nv->id = index1D;
                    nv->celltype = GSM_VOXEL;
                    nv->cell = (CHAR *)pepa;
                    nv->numelements = nprims;

                    if (nprims > hu_max_prims_cell && ng->subdiv_level < hu_max_subdiv_level) {
                        nng = create_grid(nv, ng, nprims);
                    } else {

                        nonempty_leafs++;
                        prims_in_leafs += nprims;
                    }

                    insert_in_hashtable(nv, ng);
                } else {

                    empty_voxels++;
                    mark_empty(index1D, ng);
                }

                index1D++;
            }
        }
    }

    v->cell = (CHAR *)ng;
    v->numelements = -1;
    v->celltype = GSM_GRID;

    deleteBinTree(bintree);

    return (ng);
}

#ifdef __cplusplus
extern "C"
#endif
void racebench_bug10(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug14(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug14(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug16(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug18(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug18(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug25(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug25(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug26(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug27(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug32(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug32(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug36(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug36(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug39(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug39(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug47(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug47(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug51(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug51(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug53(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug53(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug54(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug65(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug65(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug68(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug68(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug72(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug72(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug74(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug80(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug82(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug82(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug85(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug88(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug88(int);

#include <stdio.h>
#include <math.h>
#include "rt.h"

REAL eps_t = 1.0E-10;

VOID prn_tv_stats() {
}

INT send_ray(r, v) RAY *r;
VOXEL *v;
{}

VOXEL *lookup_hashtable(indx, g) INT indx;
GRID *g;
{
    INT i;
    VOXEL *v;

    i = indx - ((indx / g->num_buckets) * g->num_buckets);
    v = g->hashtable[i];

    while (v->id != indx) {
        v = v->next;
        if (v == NULL) {
            fprintf(stderr, "hashtable error \n");
            exit(-1);
        }
    }

    return (v);
}

INT lookup_emptycells(indx, g) INT indx;
GRID *g;
{
    INT i, w, r, num_bits;
    UINT p, b;

    num_bits = sizeof(UINT) * 8;

    w = indx / num_bits;
    r = indx - w * num_bits;
    p = g->emptycells[w];
    b = p & (MSB >> r);
    i = b > 0 ? EMPTY : NONEMPTY;

    return (i);
}

VOID pop_up_a_grid(r) RAY *r;
{
    RAYINFO *old_ri;

    old_ri = r->ri;
    r->ri = old_ri->next;

    free_rayinfo(r);
}

VOID push_down_grid(r, v) RAY *r;
VOXEL *v;
{
    INT n;
    INT small;
    INT i_in, i_out, i, il, ih;
    REAL wc[3];
    REAL ti;
    REAL del1, del2;
    REAL t_in, t_out, tl, th;
    REAL t[6], min[3];
    GRID *new_g;
    RAYINFO *new_ri;
    RAYINFO *old_ri;

    old_ri = r->ri;
    new_g = (GRID *)v->cell;

    new_ri = ma_rayinfo(r);
    r->ri = new_ri;

    new_ri->next = old_ri; racebench_bug27(0);
    new_ri->grid = new_g; racebench_bug88(1);

    n = new_g->indx_inc[1]; racebench_bug88(0);

    new_ri->delta[0] = old_ri->delta[0] / n;
    new_ri->delta[1] = old_ri->delta[1] / n;
    new_ri->delta[2] = old_ri->delta[2] / n;

    if (old_ri->t_in >= 0.0) {

        new_ri->entry_plane = old_ri->entry_plane;
        new_ri->t_in = old_ri->t_in; racebench_bug14(0); racebench_bug80(1);
        ti = old_ri->t_in + eps_t;
    } else {

        ti = 0.0;
        new_ri->entry_plane = -1;
        new_ri->t_in = old_ri->t_in;
    }

    wc[0] = ti * r->D[0] + r->P[0]; racebench_bug14(1);
    wc[1] = ti * r->D[1] + r->P[1];
    wc[2] = ti * r->D[2] + r->P[2];

    new_ri->index3D[0] = (int)((wc[0] - new_g->min[0]) / new_g->cellsize[0]);

    if (new_ri->index3D[0] < 0) {
        new_ri->index3D[0] = 0;
    }

    if (new_ri->index3D[0] >= n) {
        new_ri->index3D[0] = n - 1;
    }

    new_ri->index3D[1] = (int)((wc[1] - new_g->min[1]) / new_g->cellsize[1]);

    if (new_ri->index3D[1] < 0) {
        new_ri->index3D[1] = 0;
    }

    if (new_ri->index3D[1] >= n) {
        new_ri->index3D[1] = n - 1;
    }

    new_ri->index3D[2] = (int)((wc[2] - new_g->min[2]) / new_g->cellsize[2]);

    if (new_ri->index3D[2] < 0) {
        new_ri->index3D[2] = 0;
    }

    if (new_ri->index3D[2] >= n) {
        new_ri->index3D[2] = n - 1;
    }

    new_ri->index1D = new_ri->index3D[0] + new_ri->index3D[1] * n + new_ri->index3D[2] * new_g->indx_inc[2];

    new_ri->indx_inc1D[0] = r->indx_inc3D[0];
    new_ri->indx_inc1D[1] = r->indx_inc3D[1] * n;
    new_ri->indx_inc1D[2] = r->indx_inc3D[2] * new_g->indx_inc[2];

    switch (new_ri->entry_plane) {
    case 3:
        new_ri->entry_plane = 0;

    case 0:
        new_ri->d[0] = new_ri->t_in + new_ri->delta[0];

        del1 = fmod((double)(old_ri->d[1] - new_ri->t_in), (double)new_ri->delta[1]);

        if (del1 <= eps_t) {
            new_ri->d[1] = new_ri->t_in + new_ri->delta[1];
        } else {
            new_ri->d[1] = new_ri->t_in + del1;
        }

        del2 = fmod((double)(old_ri->d[2] - new_ri->t_in), (double)new_ri->delta[2]);

        if (del2 <= eps_t) {
            new_ri->d[2] = new_ri->t_in + new_ri->delta[2];
        } else {
            new_ri->d[2] = new_ri->t_in + del2;
        }

        small = new_ri->d[0] <= new_ri->d[1] ? 0 : 1;
        small = new_ri->d[small] <= new_ri->d[2] ? small : 2;

        new_ri->t_out = new_ri->d[small];
        new_ri->exit_plane = small;
        break;

    case 4:
        new_ri->entry_plane = 1;

    case 1:
        new_ri->d[1] = new_ri->t_in + new_ri->delta[1];

        del1 = fmod((double)(old_ri->d[0] - new_ri->t_in), (double)new_ri->delta[0]);

        if (del1 <= eps_t) {
            new_ri->d[0] = new_ri->t_in + new_ri->delta[0];
        } else {
            new_ri->d[0] = new_ri->t_in + del1;
        }

        del2 = fmod((double)(old_ri->d[2] - new_ri->t_in), (double)new_ri->delta[2]);

        if (del2 <= eps_t) {
            new_ri->d[2] = new_ri->t_in + new_ri->delta[2];
        } else {
            new_ri->d[2] = new_ri->t_in + del2;
        }

        small = new_ri->d[0] <= new_ri->d[1] ? 0 : 1;
        small = new_ri->d[small] <= new_ri->d[2] ? small : 2;

        new_ri->t_out = new_ri->d[small];
        new_ri->exit_plane = small;
        break;

    case 5:
        new_ri->entry_plane = 2;

    case 2:
        new_ri->d[2] = new_ri->t_in + new_ri->delta[2]; racebench_bug47(1);

        del1 = fmod((double)(old_ri->d[0] - new_ri->t_in), (double)new_ri->delta[0]); racebench_bug10(1);

        if (del1 <= eps_t) {
            new_ri->d[0] = new_ri->t_in + new_ri->delta[0];
        } else {
            new_ri->d[0] = new_ri->t_in + del1;
        }

        del2 = fmod((double)(old_ri->d[1] - new_ri->t_in), (double)new_ri->delta[1]); racebench_bug26(1);

        if (del2 <= eps_t) {
            new_ri->d[1] = new_ri->t_in + new_ri->delta[1];
        } else {
            new_ri->d[1] = new_ri->t_in + del2;
        }

        small = new_ri->d[0] <= new_ri->d[1] ? 0 : 1;
        small = new_ri->d[small] <= new_ri->d[2] ? small : 2;

        new_ri->t_out = new_ri->d[small];
        new_ri->exit_plane = small;
        break;

    case -1:

        min[0] = new_g->min[0] + new_ri->index3D[0] * new_g->cellsize[0];
        min[1] = new_g->min[1] + new_ri->index3D[1] * new_g->cellsize[1];
        min[2] = new_g->min[2] + new_ri->index3D[2] * new_g->cellsize[2];

        if (r->D[0] == 0.0) {
            if (r->P[0] >= min[0] && r->P[0] <= min[0] + new_g->cellsize[0]) {
                t[0] = -HUGE_REAL;
            } else {
                t[0] = HUGE_REAL;
            }
        } else {
            t[0] = (min[0] - r->P[0]) / r->D[0];
        }

        if (r->D[1] == 0.0) {
            if (r->P[1] >= min[1] && r->P[1] <= min[1] + new_g->cellsize[1]) {
                t[1] = -HUGE_REAL;
            } else {
                t[1] = HUGE_REAL;
            }
        } else {
            t[1] = (min[1] - r->P[1]) / r->D[1];
        }

        if (r->D[2] == 0.0) {
            if (r->P[2] >= min[2] && r->P[2] <= min[2] + new_g->cellsize[2]) {
                t[2] = -HUGE_REAL;
            } else {
                t[2] = HUGE_REAL;
            }
        } else {
            t[2] = (min[2] - r->P[2]) / r->D[2];
        }

        if (r->D[0] == 0.0) {
            if (r->P[0] >= min[0] && r->P[0] <= min[0] + new_g->cellsize[0]) {
                t[3] = HUGE_REAL;
            } else {
                t[3] = HUGE_REAL;
            }
        } else {
            t[3] = (min[0] + new_g->cellsize[0] - r->P[0]) / r->D[0];
        }

        if (r->D[1] == 0.0) {
            if (r->P[1] >= min[1] && r->P[1] <= min[1] + new_g->cellsize[1]) {
                t[4] = HUGE_REAL;
            } else {
                t[4] = HUGE_REAL;
            }
        } else {
            t[4] = (min[1] + new_g->cellsize[1] - r->P[1]) / r->D[1];
        }

        if (r->D[2] == 0.0) {
            if (r->P[2] >= min[2] && r->P[2] <= min[2] + new_g->cellsize[2]) {
                t[5] = HUGE_REAL;
            } else {
                t[5] = HUGE_REAL;
            }
        } else {
            t[5] = (min[2] + new_g->cellsize[2] - r->P[2]) / r->D[2];
        }

        t_in = -HUGE_REAL;
        i_in = -1;
        t_out = HUGE_REAL;
        i_out = -1;

        for (i = 0; i < 3; i++) {
            if (t[i] < t[i + 3]) {
                tl = t[i];
                il = i;
                th = t[i + 3];
                ih = i + 3;
            } else {
                tl = t[i + 3];
                il = i + 3;
                th = t[i];
                ih = i;
            }

            new_ri->d[i] = th;
            if (t_in < tl) {
                t_in = tl;
                i_in = il;
            }

            if (t_out > th) {
                t_out = th;
                i_out = ih;
            }
        }

        if ((t_in > t_out) || (t_out < 0.0)) {
            fprintf(stderr, "push_down_grid: Ray origin not in voxel \n");
            exit(-1);
        }

        if (i_in > 2) {
            i_in -= 3;
        }

        if (i_out > 2) {
            i_out -= 3;
        }

        new_ri->entry_plane = i_in;
        new_ri->t_in = t_in;
        new_ri->t_out = t_out;
        new_ri->exit_plane = i_out;
        break;
    }
}

INT step_grid(r) RAY *r;
{
    INT n;
    INT small;
    INT *indx;
    RAY *ra;
    GRID *gr;
    RAYINFO *rinfo;

    ra = r;
    rinfo = r->ri;
    gr = rinfo->grid;
    indx = gr->indx_inc;
    n = indx[1];

    rinfo->t_in = rinfo->t_out;
    rinfo->index3D[rinfo->exit_plane] += r->indx_inc3D[rinfo->exit_plane];
    rinfo->entry_plane = rinfo->exit_plane;

    if (rinfo->index3D[rinfo->exit_plane] < 0 || rinfo->index3D[rinfo->exit_plane] >= n) {

        return (-1);
    } else {

        rinfo->d[rinfo->exit_plane] += rinfo->delta[rinfo->exit_plane];
        rinfo->index1D += rinfo->indx_inc1D[rinfo->exit_plane];

        small = rinfo->d[0] <= rinfo->d[1] ? 0 : 1;
        small = rinfo->d[small] <= rinfo->d[2] ? small : 2;

        rinfo->t_out = rinfo->d[small];
        rinfo->exit_plane = small;

        return (rinfo->index1D);
    }
}

INT next_voxel(r) RAY *r;
{
    INT indx;
    GRID *gr;
    VOXEL *v;
    RAYINFO *rinfo;

    while ((indx = step_grid(r)) == -1) {

        rinfo = r->ri;
        gr = rinfo->grid;

        if (gr->subdiv_level != 0) {
            pop_up_a_grid(r);
            indx = r->ri->index1D;
        } else {

            return (-1);
        }
    }

    return (indx);
}

VOXEL *next_nonempty_voxel(r) RAY *r;
{
    INT indx;
    VOXEL *v;
    GRID *gr;
    RAYINFO *rinfo;

    indx = next_voxel(r);
    if (indx < 0) {
        return (NULL);
    }

    rinfo = r->ri; racebench_bug74(1);
    gr = rinfo->grid;

    while (lookup_emptycells(indx, gr) == EMPTY) {
        indx = next_voxel(r);

        if (indx < 0) {
            return (NULL);
        }

        rinfo = r->ri;
        gr = rinfo->grid;
    }

    v = lookup_hashtable(indx, gr);

    return (v);
}

VOXEL *next_nonempty_leaf(r, step, status) RAY *r;
INT step;
INT *status;
{
    INT indx;
    GRID *ng;
    VOXEL *v;
    RAYINFO *rinfo;

    if (step == STEP) {
        v = next_nonempty_voxel(r);
    } else {

        rinfo = r->ri;
        v = lookup_hashtable(rinfo->index1D, rinfo->grid);
    }

    if (v == NULL) {
        *status = EXITED_WORLD;
        return (v);
    }

    while (v->celltype == REMOTE_GRID || v->celltype == GSM_GRID) {
        if (v->celltype == REMOTE_GRID) {
            send_ray(r, v);
            *status = SENT_TO_REMOTE;
            return (NULL);
        }

        push_down_grid(r, v);

        rinfo = r->ri;
        indx = rinfo->index1D;

        if (lookup_emptycells(indx, rinfo->grid) != EMPTY) {
            v = lookup_hashtable(indx, rinfo->grid); racebench_bug85(0);
            if (v->celltype != REMOTE_GRID && v->celltype != GSM_GRID) {

                if (v->celltype == REMOTE_VOXEL) {
                    send_ray(r, v);
                    *status = SENT_TO_REMOTE;
                    return (NULL);
                }

                return (v);
            }

        } else {
            v = next_nonempty_leaf(r, STEP, status);

            return (v);
        }
    }

    return (v);
}

VOXEL *init_ray(r, g) RAY *r;
GRID *g;
{
    INT status;
    INT indx, grid_id;
    INT i_in, i_out, i, il, ih;
    REAL t_in, t_out, tl, th;
    REAL t[6];
    VOXEL *v;
    GRID *gr;
    RAYINFO *ri;

    reset_rayinfo(r); racebench_bug72(1);

    if (r->D[0] == 0.0) {
        if (r->P[0] >= g->min[0] && r->P[0] <= g->min[0] + g->cellsize[0]) {
            t[0] = -HUGE_REAL;
        } else {
            t[0] = HUGE_REAL;
        }
    } else {
        t[0] = (g->min[0] - r->P[0]) / r->D[0];
    }

    if (r->D[1] == 0.0) {
        if (r->P[1] >= g->min[1] && r->P[1] <= g->min[1] + g->cellsize[1]) {
            t[1] = -HUGE_REAL;
        } else {
            t[1] = HUGE_REAL;
        }
    } else {
        t[1] = (g->min[1] - r->P[1]) / r->D[1];
    }

    if (r->D[2] == 0.0) {
        if (r->P[2] >= g->min[2] && r->P[2] <= g->min[2] + g->cellsize[2]) {
            t[2] = -HUGE_REAL;
        } else {
            t[2] = HUGE_REAL;
        }
    } else {
        t[2] = (g->min[2] - r->P[2]) / r->D[2];
    }

    if (r->D[0] == 0.0) {
        if (r->P[0] >= g->min[0] && r->P[0] <= g->min[0] + g->cellsize[0]) {
            t[3] = HUGE_REAL;
        } else {
            t[3] = HUGE_REAL;
        }
    } else {
        t[3] = (g->min[0] + g->cellsize[0] - r->P[0]) / r->D[0];
    }

    if (r->D[1] == 0.0) {
        if (r->P[1] >= g->min[1] && r->P[1] <= g->min[1] + g->cellsize[1]) {
            t[4] = HUGE_REAL;
        } else {
            t[4] = HUGE_REAL;
        }
    } else {
        t[4] = (g->min[1] + g->cellsize[1] - r->P[1]) / r->D[1]; racebench_bug16(0);
    }

    if (r->D[2] == 0.0) {
        if (r->P[2] >= g->min[2] && r->P[2] <= g->min[2] + g->cellsize[2]) {
            t[5] = HUGE_REAL;
        } else {
            t[5] = HUGE_REAL;
        }
    } else {
        t[5] = (g->min[2] + g->cellsize[2] - r->P[2]) / r->D[2]; racebench_bug72(0);
    }

    t_in = -HUGE_REAL; racebench_bug18(0); racebench_bug18(1);
    i_in = -1;
    t_out = HUGE_REAL;
    i_out = -1; racebench_bug68(0); racebench_bug68(1);

    for (i = 0; i < 3; i++) {
        if (t[i] < t[i + 3]) {
            tl = t[i]; racebench_bug65(1);
            il = i; racebench_bug25(0); racebench_bug25(1); racebench_bug54(0); racebench_bug65(0); racebench_bug82(0);
            th = t[i + 3];
            ih = i + 3; racebench_bug36(0);
        } else {
            tl = t[i + 3];
            il = i + 3;
            th = t[i]; racebench_bug32(1);
            ih = i; racebench_bug51(1); racebench_bug82(1);
        }

        if (t_in < tl) {
            t_in = tl; racebench_bug51(0);
            i_in = il;
        }

        if (t_out > th) {
            t_out = th;
            i_out = ih; racebench_bug32(0); racebench_bug36(1);
        }
    }

    if (t_in >= t_out || t_out < 0.0) {
        return (NULL);
    }

    ri = ma_rayinfo(r);
    r->ri = ri;
    ri->grid = g;

    if (t[0] >= t[3]) {
        ri->d[0] = t[0];
    } else {
        ri->d[0] = t[3];
    }

    if (t[1] >= t[4]) {
        ri->d[1] = t[1];
    } else {
        ri->d[1] = t[4];
    }

    if (t[2] >= t[5]) {
        ri->d[2] = t[2];
    } else {
        ri->d[2] = t[5];
    }

    if (i_in > 2) {
        i_in -= 3;
    }

    if (i_out > 2) {
        i_out -= 3;
    }

    ri->entry_plane = i_in; racebench_bug53(0);
    ri->t_in = t_in;
    ri->t_out = t_out;
    ri->exit_plane = i_out; racebench_bug53(1);

    ri->delta[0] = r->D[0] == 0.0 ? HUGE_REAL : g->cellsize[0] / ABS(r->D[0]);
    ri->delta[1] = r->D[1] == 0.0 ? HUGE_REAL : g->cellsize[1] / ABS(r->D[1]);
    ri->delta[2] = r->D[2] == 0.0 ? HUGE_REAL : g->cellsize[2] / ABS(r->D[2]);

    ri->index3D[0] = 0;
    ri->index3D[1] = 0; racebench_bug39(0);
    ri->index3D[2] = 0;

    r->indx_inc3D[0] = r->D[0] >= 0.0 ? 1 : -1; racebench_bug39(1);
    r->indx_inc3D[1] = r->D[1] >= 0.0 ? 1 : -1;
    r->indx_inc3D[2] = r->D[2] >= 0.0 ? 1 : -1;

    ri->index1D = 0;

    ri->indx_inc1D[0] = r->D[0] >= 0.0 ? 1 : -1;
    ri->indx_inc1D[1] = r->D[1] >= 0.0 ? 1 : -1;
    ri->indx_inc1D[2] = r->D[2] >= 0.0 ? 1 : -1;
    ri->next = NULL;

    if ((v = next_nonempty_leaf(r, NO_STEP, &status)) != NULL) {
        ri = r->ri; racebench_bug47(0);
        indx = ri->index1D;
        gr = ri->grid;
        grid_id = gr->id;
    } else {
        return (NULL);
    }

    return (v);
}
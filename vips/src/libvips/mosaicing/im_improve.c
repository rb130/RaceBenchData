

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>
#include <string.h>

#include <vips/vips.h>

#include "mosaic.h"

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static void copypoints(TIE_POINTS *pnew, TIE_POINTS *pold) {
    int i;

    pnew->reference = pold->reference;
    pnew->secondary = pold->secondary;

    pnew->deltax = pold->deltax;
    pnew->deltay = pold->deltay;
    pnew->nopoints = pold->nopoints;
    pnew->halfcorsize = pold->halfcorsize;
    pnew->halfareasize = pold->halfareasize;

    for (i = 0; i < pold->nopoints; i++) {
        pnew->x_reference[i] = pold->x_reference[i];
        pnew->y_reference[i] = pold->y_reference[i];
        pnew->x_secondary[i] = pold->x_secondary[i];
        pnew->y_secondary[i] = pold->y_secondary[i];
        pnew->contrast[i] = pold->contrast[i];
        pnew->correlation[i] = pold->correlation[i];
        pnew->deviation[i] = pold->deviation[i];
        pnew->dx[i] = pold->dx[i];
        pnew->dy[i] = pold->dy[i];
    }

    pnew->l_scale = pold->l_scale;
    pnew->l_angle = pold->l_angle;
    pnew->l_deltax = pold->l_deltax;
    pnew->l_deltay = pold->l_deltay;
}

static int copydevpoints(TIE_POINTS *pnew, TIE_POINTS *pold) {
    int i;
    int j;
    double thresh_dev, max_dev, min_dev;
    double *corr;

    min_dev = 9999.0;
    max_dev = 0.0;
    corr = &pold->correlation[0];

    for (i = 0; i < pold->nopoints; i++) {
        if (corr[i] > 0.01) {
            if (pold->deviation[i] / corr[i] < min_dev) {
                min_dev = pold->deviation[i] / corr[i];
            }
            if (pold->deviation[i] / corr[i] > max_dev) {
                max_dev = pold->deviation[i] / corr[i];
            }
        }
    }

    thresh_dev = min_dev + (max_dev - min_dev) * 0.3;
    if (thresh_dev <= 1.0) {
        thresh_dev = 1.0;
    }

    for (i = 0, j = 0; i < pold->nopoints; i++) {
        if (pold->correlation[i] > 0.01) {
            if (pold->deviation[i] / corr[i] <= thresh_dev) {
                pnew->x_reference[j] = pold->x_reference[i];
                pnew->y_reference[j] = pold->y_reference[i];
                pnew->x_secondary[j] = pold->x_secondary[i];
                pnew->y_secondary[j] = pold->y_secondary[i];
                pnew->contrast[j] = pold->contrast[i];
                pnew->correlation[j] = pold->correlation[i];
                pnew->deviation[j] = pold->deviation[i];
                pnew->dx[j] = pold->dx[i];
                pnew->dy[j] = pold->dy[i];
                j++;
            }
        }
    }
    pnew->nopoints = j;

    for (i = j; i < IM_MAXPOINTS; i++) {
        pnew->x_reference[i] = 0;
        pnew->y_reference[i] = 0;
        pnew->x_secondary[i] = 0;
        pnew->y_secondary[i] = 0;
        pnew->contrast[i] = 0;
        pnew->correlation[i] = 0.0;
        pnew->deviation[i] = 0.0;
        pnew->dx[i] = 0.0;
        pnew->dy[i] = 0.0;
    }

    if (j != pold->nopoints) {
        return (-1);
    }

    return (0);
}

#define SWAP(A, B)     \
    {                  \
        void *t = (A); \
        A = B;         \
        B = t;         \
    }

int im__improve(TIE_POINTS *inpoints, TIE_POINTS *outpoints) {
    TIE_POINTS points1, points2;
    TIE_POINTS *p = &points1;
    TIE_POINTS *q = &points2;

    for (copypoints(p, inpoints); copypoints(q, p), copydevpoints(q, p);) {

        if (q->nopoints < 2) {
            break;
        }

        if (im__clinear(q)) {
            return (-1);
        }

        SWAP(p, q);
    }

    copypoints(outpoints, q);

    return (0);
}

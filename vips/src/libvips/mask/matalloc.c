

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define TINY 1.0e-200

int *im_ivector(nl, nh)
int nl, nh;
{
    int *v;

    v = (int *)im_malloc(NULL, (unsigned)(nh - nl + 1) * sizeof(int));
    if (v == NULL) {
        return (NULL);
    } else {
        return (v - nl);
    }
}

float *im_fvector(nl, nh)
int nl, nh;
{
    float *v;

    v = (float *)im_malloc(NULL, (unsigned)(nh - nl + 1) * sizeof(float));
    if (v == NULL) {
        return (NULL);
    } else {
        return (v - nl);
    }
}

double *im_dvector(nl, nh)
int nl, nh;
{
    double *v;

    v = (double *)im_malloc(NULL, (unsigned)(nh - nl + 1) * sizeof(double));
    if (v == NULL) {
        return (NULL);
    } else {
        return (v - nl);
    }
}

void im_free_ivector(v, nl, nh) int *v;
int nl, nh;
{ im_free((char *)(v + nl)); }

void im_free_fvector(v, nl, nh) float *v;
int nl, nh;
{ im_free((char *)(v + nl)); }

void im_free_dvector(v, nl, nh) double *v;
int nl, nh;
{ im_free((char *)(v + nl)); }

int **im_imat_alloc(nrl, nrh, ncl, nch)
int nrl, nrh, ncl, nch;
{
    int i;
    int **m;

    m = (int **)im_malloc(NULL, (unsigned)(nrh - nrl + 1) * sizeof(int *));
    if (m == NULL) {
        return (NULL);
    }
    m -= nrl;

    for (i = nrl; i <= nrh; i++) {
        m[i] = (int *)im_malloc(NULL, (unsigned)(nch - ncl + 1) * sizeof(int));
        if (m[i] == NULL) {
            return (NULL);
        }
        m[i] -= ncl;
    }
    return (m);
}

void im_free_imat(m, nrl, nrh, ncl, nch) int **m;
int nrl, nrh, ncl, nch;
{
    int i;

    for (i = nrh; i >= nrl; i--) {
        im_free((char *)(m[i] + ncl));
    }
    im_free((char *)(m + nrl));
}

float **im_fmat_alloc(nrl, nrh, ncl, nch)
int nrl, nrh, ncl, nch;
{
    int i;
    float **m;

    m = (float **)im_malloc(NULL, (unsigned)(nrh - nrl + 1) * sizeof(float *));
    if (m == NULL) {
        return (NULL);
    }
    m -= nrl;

    for (i = nrl; i <= nrh; i++) {
        m[i] = (float *)im_malloc(NULL, (unsigned)(nch - ncl + 1) * sizeof(float));
        if (m[i] == NULL) {
            return (NULL);
        }
        m[i] -= ncl;
    }
    return (m);
}

void im_free_fmat(m, nrl, nrh, ncl, nch) float **m;
int nrl, nrh, ncl, nch;
{
    int i;

    for (i = nrh; i >= nrl; i--) {
        im_free((char *)(m[i] + ncl));
    }
    im_free((char *)(m + nrl));
}

double **im_dmat_alloc(nrl, nrh, ncl, nch)
int nrl, nrh, ncl, nch;
{
    int i;
    double **m;

    m = (double **)im_malloc(NULL, (unsigned)(nrh - nrl + 1) * sizeof(double *));
    if (m == NULL) {
        return (NULL);
    }
    m -= nrl;

    for (i = nrl; i <= nrh; i++) {
        m[i] = (double *)im_malloc(NULL, (unsigned)(nch - ncl + 1) * sizeof(double));
        if (m[i] == NULL) {
            return (NULL);
        }
        m[i] -= ncl;
    }
    return (m);
}

void im_free_dmat(m, nrl, nrh, ncl, nch) double **m;
int nrl, nrh, ncl, nch;
{
    int i;

    for (i = nrh; i >= nrl; i--) {
        im_free((char *)(m[i] + ncl));
    }
    im_free((char *)(m + nrl));
}

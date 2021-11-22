

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#if !HAVE_FFTW && !HAVE_FFTW3

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static float *Const = NULL, *iConst = NULL;
static int storesize = 0, Constsize = 0;

static int fftn(rvec, ivec, loglen, nskip)

float *rvec, *ivec;
int loglen, nskip;

{
    int n, nv2, nm1, i, j, k, l, le, le1, c, nle;
    float *rveci, *rvecj, *iveci, *ivecj;
    float t, wr, wi, tr, ti;

    if (loglen == 0) {
        return (-1);
    }
    n = 1 << loglen;
    nv2 = n >> 1;
    nm1 = n - 1;
    j = 0;
    if (storesize < nv2) {
        if ((0 == (Const = (float *)calloc((unsigned)nv2, sizeof(float)))) || (0 == (iConst = (float *)calloc((unsigned)nv2, sizeof(float))))) {
            im_errormsg("Not enough core for fftn");
            return (-1);
        }
        storesize = nv2;
    }
    if (Constsize != nv2) {
        Constsize = nv2;
        wr = cos(2 * IM_PI / n);
        wi = -sin(2 * IM_PI / n);
        Const[0] = 1.;
        iConst[0] = 0.;
        for (i = 1; i < nv2; i++) {
            Const[i] = wr * Const[i - 1] - wi * iConst[i - 1];
            iConst[i] = wr * iConst[i - 1] + wi * Const[i - 1];
        }
    }
    for (i = 0; i < nm1; i++) {
        if (i < j) {
            rveci = rvec + i * nskip;
            rvecj = rvec + j * nskip;
            t = (*rvecj);
            *rvecj = (*rveci);
            *rveci = t;
            iveci = ivec + i * nskip;
            ivecj = ivec + j * nskip;
            t = (*ivecj);
            *ivecj = (*iveci);
            *iveci = t;
        }
        k = nv2;
        while (k <= j) {
            j -= k;
            k >>= 1;
        }
        j += k;
    }
    le = 1;
    for (l = 0; l < loglen; l++) {
        le1 = le;
        le += le;
        c = 0;
        nle = n / le;
        for (j = 0; j < le1; j++) {
            for (i = j; i < n; i += le) {
                if (i + le1 >= n) {
                    im_warning("index=%d\n", i + le1);
                    return (-1);
                }
                rveci = rvec + i * nskip;
                rvecj = rvec + (i + le1) * nskip;
                iveci = ivec + i * nskip;
                ivecj = ivec + (i + le1) * nskip;

                if (c == 0) {
                    tr = *rvecj;
                    ti = *ivecj;
                } else {
                    tr = *rvecj * Const[c] - *ivecj * iConst[c];
                    ti = *rvecj * iConst[c] + *ivecj * Const[c];
                }
                *rvecj = *rveci - tr;
                *ivecj = *iveci - ti;

                *rveci += tr;
                *iveci += ti;
            }
            c += nle;
        }
    }

    return (0);
}

static void free_store(void) {
    if (Const) {
        free(Const);
        Const = NULL;
    }
    if (iConst) {
        free(iConst);
        iConst = NULL;
    }

    storesize = 0;
    Constsize = 0;
}

int im__fft_sp(float *rvec, float *ivec, int logrows, int logcols) {
    int i, rows, cols, size;

    rows = 1 << logrows;
    cols = 1 << logcols;
    size = rows * cols;

    for (i = 0; i < size; i += cols) {
        if (fftn(rvec + i, ivec + i, logcols, 1) == -1) {
            free_store();
            return (-1);
        }
    }

    for (i = 0; i < cols; i++) {
        if (fftn(rvec + i, ivec + i, logrows, cols) == -1) {
            free_store();
            return (-1);
        }
    }

    free_store();
    return (0);
}

#endif


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static void *stats_start(IMAGE *im, void *a, void *b) {
    double *global_stats = (double *)b;

    double *stats;
    int i;

    if (!(stats = IM_ARRAY(NULL, 4 * im->Bands, double))) {
        return (NULL);
    }

    for (i = 0; i < 4 * im->Bands; i++) {
        stats[i] = global_stats[i];
    }

    return (stats);
}

static int stats_stop(void *seq, void *a, void *b) {
    const IMAGE *im = (IMAGE *)a;
    double *global_stats = (double *)b;
    double *stats = (double *)seq;

    int i;

    for (i = 0; i < 4 * im->Bands; i += 4) {
        global_stats[0] = IM_MIN(global_stats[0], stats[0]);
        global_stats[1] = IM_MAX(global_stats[1], stats[1]);
        global_stats[2] += stats[2];
        global_stats[3] += stats[3];

        global_stats += 4;
        stats += 4;
    }

    im_free(seq);

    return (0);
}

#define LOOP(TYPE)                                     \
    {                                                  \
        for (z = 0; z < im->Bands; z++) {              \
            TYPE *q = (TYPE *)in + z;                  \
            double *row = stats + z * 4;               \
            TYPE small, big;                           \
            double sum, sum2;                          \
                                                       \
            small = row[0];                            \
            big = row[1];                              \
            sum = row[2];                              \
            sum2 = row[3];                             \
                                                       \
            for (x = 0; x < n; x++) {                  \
                TYPE value = *q;                       \
                                                       \
                sum += value;                          \
                sum2 += (double)value * (double)value; \
                if (value > big)                       \
                    big = value;                       \
                else if (value < small)                \
                    small = value;                     \
                                                       \
                q += im->Bands;                        \
            }                                          \
                                                       \
            row[0] = small;                            \
            row[1] = big;                              \
            row[2] = sum;                              \
            row[3] = sum2;                             \
        }                                              \
    }

static int stats_scan(void *in, int n, void *seq, void *a, void *b) {
    const IMAGE *im = (IMAGE *)a;

    double *stats = (double *)seq;

    int x, z;

    switch (im->BandFmt) {
    case IM_BANDFMT_UCHAR:
        LOOP(unsigned char);
        break;
    case IM_BANDFMT_CHAR:
        LOOP(signed char);
        break;
    case IM_BANDFMT_USHORT:
        LOOP(unsigned short);
        break;
    case IM_BANDFMT_SHORT:
        LOOP(signed short);
        break;
    case IM_BANDFMT_UINT:
        LOOP(unsigned int);
        break;
    case IM_BANDFMT_INT:
        LOOP(signed int);
        break;
    case IM_BANDFMT_DOUBLE:
        LOOP(double);
        break;
    case IM_BANDFMT_FLOAT:
        LOOP(float);
        break;

    default:
        g_assert(0);
    }

    return (0);
}

DOUBLEMASK *im_stats(IMAGE *im) {
    DOUBLEMASK *out;
    double *row;
    gint64 pels, vals;
    double *global_stats;
    int i, j;
    double value;

    if (im_pincheck(im) || im_check_noncomplex("im_stats", im) || im_check_uncoded("im_stats", im)) {
        return (NULL);
    }

    if (!(global_stats = IM_ARRAY(im, 4 * im->Bands, double))) {
        return (NULL);
    }
    if (im__value(im, &value)) {
        return (NULL);
    }
    for (i = 0; i < 4 * im->Bands; i += 4) {
        global_stats[i + 0] = value;
        global_stats[i + 1] = value;
        global_stats[i + 2] = 0.0;
        global_stats[i + 3] = 0.0;
    }

    if (im__wrapscan(im, stats_start, stats_scan, stats_stop, im, global_stats)) {
        return (NULL);
    }

    if (!(out = im_create_dmask("stats", 6, im->Bands + 1))) {
        return (NULL);
    }

    out->coeff[0] = value;
    out->coeff[1] = value;
    out->coeff[2] = 0.0;
    out->coeff[3] = 0.0;

    pels = (gint64)im->Xsize * im->Ysize;
    vals = pels * im->Bands;

    for (i = 0; i < im->Bands; i++) {
        row = out->coeff + (i + 1) * 6;
        for (j = 0; j < 4; j++) {
            row[j] = global_stats[i * 4 + j];
        }

        out->coeff[0] = IM_MIN(out->coeff[0], row[0]);
        out->coeff[1] = IM_MAX(out->coeff[1], row[1]);
        out->coeff[2] += row[2];
        out->coeff[3] += row[3];
        row[4] = row[2] / pels;
        row[5] = sqrt(fabs(row[3] - (row[2] * row[2] / pels)) / (pels - 1));
    }
    out->coeff[4] = out->coeff[2] / vals;
    out->coeff[5] = sqrt(fabs(out->coeff[3] - (out->coeff[2] * out->coeff[2] / vals)) / (vals - 1));

#ifdef DEBUG
    printf("im_stats:\n");
    im_print_dmask(out);
#endif

    return (out);
}

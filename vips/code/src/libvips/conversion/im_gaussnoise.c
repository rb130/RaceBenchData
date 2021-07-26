
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct {
    double mean;
    double sigma;
} GnoiseInfo;

static int gnoise_gen(REGION * or, void *seq, void *a, void *b) {
    GnoiseInfo *gin = (GnoiseInfo *)a;
    int x, y, i;
    int sz = IM_REGION_N_ELEMENTS(or);

    for (y = 0; y < or->valid.height; y++) {
        float *q = (float *)IM_REGION_ADDR(or, or->valid.left, y + or->valid.top);

        for (x = 0; x < sz; x++) {
            double sum = 0.0;

            for (i = 0; i < 12; i++) {
#ifdef HAVE_RANDOM
                sum += (double)random() / RAND_MAX;
            }
#else
#ifdef HAVE_RAND
                sum += (double)rand() / RAND_MAX;
#else
#error "no random number generator found"
#endif
#endif

            q[x] = (sum - 6.0) * gin->sigma + gin->mean;
        }
    }

    return (0);
}

int im_gaussnoise(IMAGE *out, int x, int y, double mean, double sigma) {
    GnoiseInfo *gin;

    if (x <= 0 || y <= 0) {
        im_error("im_gaussnoise", "%s", _("bad parameter"));
        return (-1);
    }

    if (im_poutcheck(out)) {
        return (-1);
    }
    im_initdesc(out, x, y, 1, IM_BBITS_FLOAT, IM_BANDFMT_FLOAT, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);
    if (im_demand_hint(out, IM_ANY, NULL)) {
        return (-1);
    }

    if (!(gin = IM_NEW(out, GnoiseInfo))) {
        return (-1);
    }
    gin->mean = mean;
    gin->sigma = sigma;

    if (im_generate(out, NULL, gnoise_gen, NULL, gin, NULL)) {
        return (-1);
    }

    return (0);
}

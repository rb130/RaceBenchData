

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct joins {
    int nim;
    IMAGE **in;
    int *is;
} Join;

static Join *join_new(IMAGE *out, IMAGE **in, int nim) {
    Join *join;
    int i;

    if (!(join = IM_NEW(out, Join))) {
        return (NULL);
    }
    join->nim = nim;
    if (!(join->in = IM_ARRAY(out, nim + 1, IMAGE *)) || !(join->is = IM_ARRAY(out, nim, int))) {
        return (NULL);
    }

    if (im_open_local_array(out, join->in, nim, "im_gbandjoin", "p") || im__formatalike_vec(in, join->in, nim)) {
        return (NULL);
    }

    for (i = 0; i < nim; i++) {
        join->is[i] = IM_IMAGE_SIZEOF_PEL(join->in[i]);
    }

    join->in[nim] = NULL;

    return (join);
}

static int join_bands(REGION * or, void *seq, void *a, void *b) {
    REGION **ir = (REGION **)seq;
    Join *join = (Join *)b;
    Rect *r = & or->valid;
    const int ps = IM_IMAGE_SIZEOF_PEL(or->im);

    int x, y, z, i;

    for (i = 0; i < join->nim; i++) {
        if (im_prepare(ir[i], r)) {
            return (-1);
        }
    }

    for (y = 0; y < r->height; y++) {
        PEL *qb;

        qb = (PEL *)IM_REGION_ADDR(or, r->left, r->top + y);

        for (i = 0; i < join->nim; i++) {
            int k = join->is[i];

            PEL *p;
            PEL *q;

            p = (PEL *)IM_REGION_ADDR(ir[i], r->left, r->top + y);
            q = qb;

            for (x = 0; x < r->width; x++) {
                for (z = 0; z < k; z++) {
                    q[z] = p[z];
                }

                p += z;
                q += ps;
            }

            qb += k;
        }
    }

    return (0);
}

int im_gbandjoin(IMAGE **in, IMAGE *out, int nim) {
    int i;
    Join *join;

    if (nim < 1) {
        im_error("im_gbandjoin", "%s", _("zero input images!"));
        return (-1);
    } else if (nim == 1) {
        return (im_copy(in[0], out));
    }

    if (im_poutcheck(out) || im_check_coding_known("im_gbandjoin", in[0])) {
        return (-1);
    }
    for (i = 0; i < nim; i++) {
        if (im_pincheck(in[i]) || im_check_size_same("im_gbandjoin", in[i], in[0]) || im_check_coding_same("im_gbandjoin", in[i], in[0])) {
            return (-1);
        }
    }

    if (!(join = join_new(out, in, nim))) {
        return (-1);
    }

    if (im_cp_desc_array(out, join->in)) {
        return (-1);
    }
    out->Bands = 0;
    for (i = 0; i < nim; i++) {
        out->Bands += join->in[i]->Bands;
    }
    if (im_demand_hint_array(out, IM_THINSTRIP, join->in)) {
        return (-1);
    }

    if (im_generate(out, im_start_many, join_bands, im_stop_many, join->in, join)) {
        return (-1);
    }

    return (0);
}

int im_bandjoin(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    IMAGE *t[2];

    t[0] = in1;
    t[1] = in2;

    return (im_gbandjoin(t, out, 2));
}

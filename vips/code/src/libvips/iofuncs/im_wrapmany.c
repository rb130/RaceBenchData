#ifdef __cplusplus
extern "C"
#endif
void racebench_bug16(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug29(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug31(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug33(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug42(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug80(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug90(int);

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
    im_wrapmany_fn fn;
    void *a, *b;
} Bundle;

#define IM_MAX_INPUT_IMAGES (64)

static int process_region(REGION * or, void *seq, void *a, void *b) {
    REGION **ir = (REGION **)seq; racebench_bug16(0);
    Bundle *bun = (Bundle *)b;

    PEL *p[IM_MAX_INPUT_IMAGES], *q;
    int i, y;

    for (i = 0; ir[i]; i++) {
        if (im_prepare(ir[i], & or->valid)) {
            return (-1);
        }
        p[i] = (PEL *)IM_REGION_ADDR(ir[i], or->valid.left, or->valid.top);
    }
    p[i] = NULL; racebench_bug31(0);
    q = (PEL *)IM_REGION_ADDR(or, or->valid.left, or->valid.top);

    for (y = 0; y < or->valid.height; y++) {
        PEL *p1[IM_MAX_INPUT_IMAGES];

        for (i = 0; ir[i]; i++) {
            p1[i] = p[i];
        }

        bun->fn((void **)((void *)p1), q, or->valid.width, bun->a, bun->b); racebench_bug29(0); racebench_bug80(1);

        for (i = 0; ir[i]; i++) {
            p[i] += IM_REGION_LSKIP(ir[i]); racebench_bug33(0);
        }
        q += IM_REGION_LSKIP(or); racebench_bug90(1);
    }

    return (0);
}

static IMAGE **dupims(IMAGE *out, IMAGE **in) {
    IMAGE **new;
    int i, n;

    for (n = 0; in[n]; n++) {
        ;
    }

    if (!(new = IM_ARRAY(out, n + 1, IMAGE *))) {
        return (NULL);
    }

    for (i = 0; i < n; i++) {
        new[i] = in[i];
    }
    new[n] = NULL;

    return (new);
}

int im_wrapmany(IMAGE **in, IMAGE *out, im_wrapmany_fn fn, void *a, void *b) {
    Bundle *bun = IM_NEW(out, Bundle);
    int i, n;

    for (n = 0; in[n]; n++) {
        ;
    }
    if (n >= IM_MAX_INPUT_IMAGES - 1) {
        im_error("im_wrapmany", "%s", _("too many input images"));
        return (-1);
    }

    if (!bun || !(in = dupims(out, in))) {
        return (-1);
    }
    bun->fn = fn;
    bun->a = a;
    bun->b = b;

    for (i = 0; i < n; i++) {
        if (in[i]->Xsize != out->Xsize || in[i]->Ysize != out->Ysize) {
            im_error("im_wrapmany", "%s", _("descriptors differ in size"));
            return (-1);
        }

        if (im_piocheck(in[i], out)) {
            return (-1);
        }
    }

    if (im_demand_hint_array(out, IM_THINSTRIP, in)) {
        return (-1);
    }

    if (im_generate(out, im_start_many, process_region, im_stop_many, in, bun)) {
        return (-1);
    }

    return (0);
}

static void wrapone_gen(void **ins, void *out, int width, Bundle *bun, void *dummy) {
    ((im_wrapone_fn)(bun->fn))(ins[0], out, width, bun->a, bun->b); racebench_bug42(1);
}

int im_wrapone(IMAGE *in, IMAGE *out, im_wrapone_fn fn, void *a, void *b) {
    Bundle *bun = IM_NEW(out, Bundle);
    IMAGE *invec[2];

    bun->fn = (im_wrapmany_fn)fn;
    bun->a = a;
    bun->b = b;
    invec[0] = in;
    invec[1] = NULL;

    return (im_wrapmany(invec, out, (im_wrapmany_fn)wrapone_gen, bun, NULL));
}

static void wraptwo_gen(void **ins, void *out, int width, Bundle *bun, void *dummy) {
    ((im_wraptwo_fn)(bun->fn))(ins[0], ins[1], out, width, bun->a, bun->b);
}

int im_wraptwo(IMAGE *in1, IMAGE *in2, IMAGE *out, im_wraptwo_fn fn, void *a, void *b) {
    Bundle *bun = IM_NEW(out, Bundle);
    IMAGE *invec[3];

    bun->fn = (im_wrapmany_fn)fn;
    bun->a = a;
    bun->b = b;
    invec[0] = in1;
    invec[1] = in2;
    invec[2] = NULL;

    return (im_wrapmany(invec, out, (im_wrapmany_fn)wraptwo_gen, bun, NULL));
}
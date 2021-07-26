
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

static int benchmark(IMAGE *in, IMAGE *out) {
    IMAGE *t[18];
    double one[3] = {1.0, 1.0, 1.0};
    double zero[3] = {0.0, 0.0, 0.0};
    double darken[3] = {1.0 / 1.18, 1.0, 1.0};
    double whitepoint[3] = {1.06, 1.0, 1.01};
    double shadow[3] = {-2, 0, 0};
    double white[3] = {100, 0, 0};
    DOUBLEMASK *d652d50 = im_create_dmaskv("d652d50", 3, 3, 1.13529, -0.0604663, -0.0606321, 0.0975399, 0.935024, -0.0256156, -0.0336428, 0.0414702, 0.994135);

    im_add_close_callback(out, (im_callback_fn)im_free_dmask, d652d50, NULL);

    return (

        im_open_local_array(out, t, 18, "im_benchmark", "p") ||

        im_LabQ2Lab(in, t[0]) ||

        im_extract_area(t[0], t[1], 100, 100, t[0]->Xsize - 200, t[0]->Ysize - 200) ||

        im_affinei_all(t[1], t[2], vips_interpolate_bilinear_static(), 0.9, 0, 0, 0.9, 0, 0) ||

        im_extract_band(t[2], t[3], 0) || im_moreconst(t[3], t[4], 99) ||

        im_lintra_vec(3, darken, t[2], zero, t[5]) || im_Lab2XYZ(t[5], t[6]) || im_recomb(t[6], t[7], d652d50) || im_lintra_vec(3, whitepoint, t[7], zero, t[8]) || im_lintra(1.5, t[8], 0.0, t[9]) || im_XYZ2Lab(t[9], t[10]) || im_lintra_vec(3, one, t[10], shadow, t[11]) ||

        im_black(t[12], t[4]->Xsize, t[4]->Ysize, 3) || im_lintra_vec(3, zero, t[12], white, t[13]) ||

        im_ifthenelse(t[4], t[13], t[11], t[14]) ||

        im_Lab2LabQ(t[14], t[15]) || im_sharpen(t[15], out, 11, 2.5, 40, 20, 0.5, 1.5));
}

int im_benchmarkn(IMAGE *in, IMAGE *out, int n) {
    IMAGE *t[2];

    if (n == 0) {

        return (im_LabQ2disp(in, out, im_col_displays(7)));
    } else {
        return (im_open_local_array(out, t, 2, "benchmarkn", "p") ||

                benchmark(in, t[0]) ||

                im_affinei_all(t[0], t[1], vips_interpolate_bilinear_static(), (double)in->Xsize / t[0]->Xsize, 0, 0, (double)in->Ysize / t[0]->Ysize, 0, 0) ||

                im_benchmarkn(t[1], out, n - 1));
    }
}

int im_benchmark2(IMAGE *in, double *out) {
    IMAGE *t;

    return (!(t = im_open_local(in, "benchmarkn", "p")) || im_benchmarkn(in, t, 1) || im_avg(t, out));
}

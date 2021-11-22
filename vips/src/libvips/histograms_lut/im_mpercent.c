

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_mpercent_hist(IMAGE *hist, double percent, int *out) {
    IMAGE *base;
    IMAGE *t[6];
    double pos;

    if (im_check_hist("im_mpercent", hist)) {
        return (-1);
    }

    if (!(base = im_open("im_mpercent", "p"))) {
        return (-1);
    }
    if (im_open_local_array(base, t, 6, "im_mpercent", "p")) {
        im_close(base);
        return (-1);
    }

    if (im_histcum(hist, t[1]) || im_histnorm(t[1], t[2]) || im_lessconst(t[2], t[3], percent * t[2]->Xsize) || im_fliphor(t[3], t[4]) || im_profile(t[4], t[5], 1) || im_avg(t[5], &pos)) {
        im_close(base);
        return (-1);
    }
    im_close(base);

    *out = pos;

    return (0);
}

int im_mpercent(IMAGE *in, double percent, int *out) {
    IMAGE *t;

    if (!(t = im_open("im_mpercent1", "p"))) {
        return (-1);
    }
    if (im_histgr(in, t, -1) || im_mpercent_hist(t, percent, out)) {
        im_close(t);
        return (-1);
    }
    im_close(t);

    return (0);
}

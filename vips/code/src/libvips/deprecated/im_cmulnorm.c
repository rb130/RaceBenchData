
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_cmulnorm(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    IMAGE *t1;

    if (!(t1 = im_open_local(out, "im_cmulnorm:1", "p")) || im_multiply(in1, in2, t1) || im_sign(t1, out)) {
        return (-1);
    }

    return (0);
}

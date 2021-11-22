

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_rotquad(IMAGE *in, IMAGE *out) {
    return (im_wrap(in, out, in->Xsize / 2, in->Ysize / 2));
}

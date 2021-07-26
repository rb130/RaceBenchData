
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int black_gen(REGION * or, void *seq, void *a, void *b) {
    im_region_black(or);

    return (0);
}

int im_black(IMAGE *out, int x, int y, int bands) {
    if (x <= 0 || y <= 0 || bands <= 0) {
        im_error("im_black", "%s", _("bad parameter"));
        return (-1);
    }

    if (im_poutcheck(out)) {
        return (-1);
    }
    im_initdesc(out, x, y, bands, IM_BBITS_BYTE, IM_BANDFMT_UCHAR, IM_CODING_NONE, bands == 1 ? IM_TYPE_B_W : IM_TYPE_MULTIBAND, 1.0, 1.0, 0, 0);
    if (im_demand_hint(out, IM_ANY, NULL)) {
        return (-1);
    }

    if (im_generate(out, NULL, black_gen, NULL, NULL, NULL)) {
        return (-1);
    }

    return (0);
}



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

int im_insertplace(IMAGE *main, IMAGE *sub, int x, int y) {
    Rect br, sr, clip;
    PEL *p, *q;
    int z;

    if (im_rwcheck(main) || im_incheck(sub) || im_check_coding_known("im_insertplace", main) || im_check_coding_known("im_insertplace", sub) || im_check_format_same("im_insertplace", main, sub) || im_check_bands_same("im_insertplace", main, sub)) {
        return (-1);
    }

    br.left = 0;
    br.top = 0;
    br.width = main->Xsize;
    br.height = main->Ysize;
    sr.left = x;
    sr.top = y;
    sr.width = sub->Xsize;
    sr.height = sub->Ysize;
    im_rect_intersectrect(&br, &sr, &clip);
    if (im_rect_isempty(&clip)) {
        return (0);
    }

    p = (PEL *)IM_IMAGE_ADDR(sub, clip.left - x, clip.top - y);
    q = (PEL *)IM_IMAGE_ADDR(main, clip.left, clip.top);
    for (z = 0; z < clip.height; z++) {
        memcpy((char *)q, (char *)p, clip.width * IM_IMAGE_SIZEOF_PEL(sub));
        p += IM_IMAGE_SIZEOF_LINE(sub);
        q += IM_IMAGE_SIZEOF_LINE(main);
    }

    return (0);
}

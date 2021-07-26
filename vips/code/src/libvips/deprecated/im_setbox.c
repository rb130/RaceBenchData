
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <sys/types.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void im_setbox(pbox, xst, yst, xsiz, ysiz, ch_select) IMAGE_BOX *pbox;
int xst, yst, xsiz, ysiz, ch_select;
{
    pbox->xstart = xst;
    pbox->ystart = yst;
    pbox->xsize = xsiz;
    pbox->ysize = ysiz;
    pbox->chsel = ch_select;
}

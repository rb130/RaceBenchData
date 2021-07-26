
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <sys/types.h>
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include <sys/stat.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_unmapfile(IMAGE *im) {
    if (im__munmap(im->baseaddr, im->length)) {
        return (-1);
    }
    im->baseaddr = NULL;
    im->length = 0;

    return (0);
}

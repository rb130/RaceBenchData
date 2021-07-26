
#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_close.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"

#if USE_PROTOTYPES
extern int TIFFFreeDirectory(TIFF *);
#else
extern int TIFFFreeDirectory();
#endif

void TIFFClose(tif) TIFF *tif;
{
    if (tif->tif_mode != O_RDONLY) {

        TIFFFlush(tif);
    }
    if (tif->tif_cleanup) {
        (*tif->tif_cleanup)(tif);
    }
    TIFFFreeDirectory(tif);
    if (tif->tif_rawdata && (tif->tif_flags & TIFF_MYBUFFER)) {
        free(tif->tif_rawdata);
    }
#ifdef MMAP_SUPPORT
    if (isMapped(tif)) {
        TIFFUnmapFileContents(tif->tif_base, tif->tif_size);
    }
#endif
    (void)close(tif->tif_fd);
    free((char *)tif);
}

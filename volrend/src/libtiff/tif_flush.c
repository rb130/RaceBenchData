

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_flush.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"

TIFFFlush(tif) TIFF *tif;
{

    if (tif->tif_mode != O_RDONLY) {
        if (!TIFFFlushData(tif)) {
            return (0);
        }
        if ((tif->tif_flags & TIFF_DIRTYDIRECT) && !TIFFWriteDirectory(tif)) {
            return (0);
        }
    }
    return (1);
}

TIFFFlushData(tif) TIFF *tif;
{
    if ((tif->tif_flags & TIFF_BEENWRITING) == 0) {
        return (0);
    }
    if (tif->tif_flags & TIFF_POSTENCODE) {
        tif->tif_flags &= ~TIFF_POSTENCODE;
        if (tif->tif_postencode && !(*tif->tif_postencode)(tif)) {
            return (0);
        }
    }
    return (TIFFFlushData1(tif));
}

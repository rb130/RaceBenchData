
#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_strip.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"

u_int TIFFComputeStrip(tif, row, sample) TIFF *tif;
u_long row;
u_int sample;
{
    TIFFDirectory *td = &tif->tif_dir;
    u_int strip;

    strip = row / td->td_rowsperstrip;
    if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
        if (sample >= td->td_samplesperpixel) {
            TIFFError(tif->tif_name, "%d: Sample out of range, max %d", sample, td->td_samplesperpixel);
            return (0);
        }
        strip += sample * td->td_stripsperimage;
    }
    return (strip);
}

u_int TIFFNumberOfStrips(tif) TIFF *tif;
{
    TIFFDirectory *td = &tif->tif_dir;

    return (td->td_rowsperstrip == 0xffffffff ? (td->td_imagelength != 0 ? 1 : 0) : howmany(td->td_imagelength, td->td_rowsperstrip));
}

u_long TIFFVStripSize(tif, nrows) TIFF *tif;
u_long nrows;
{
    TIFFDirectory *td = &tif->tif_dir;

    if (nrows == (u_long)-1) {
        nrows = td->td_imagelength;
    }
#ifdef YCBCR_SUPPORT
    if (td->td_planarconfig == PLANARCONFIG_CONTIG && td->td_photometric == PHOTOMETRIC_YCBCR) {

        u_long w = roundup(td->td_imagewidth, td->td_ycbcrsubsampling[0]);
        u_long scanline = howmany(w * td->td_bitspersample, 8);
        u_long samplingarea = td->td_ycbcrsubsampling[0] * td->td_ycbcrsubsampling[1];
        nrows = roundup(nrows, td->td_ycbcrsubsampling[1]);

        return (nrows * scanline + 2 * (nrows * scanline / samplingarea));
    } else {
#endif
        return (nrows * TIFFScanlineSize(tif));
    }
}

u_long TIFFStripSize(tif) TIFF *tif;
{ return (TIFFVStripSize(tif, tif->tif_dir.td_rowsperstrip)); }

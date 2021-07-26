
#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_ccittrle.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include "tif_fax3.h"

TIFFInitCCITTRLE(tif) TIFF *tif;
{
    TIFFInitCCITTFax3(tif);
    tif->tif_preencode = NULL;
    tif->tif_postencode = NULL;
    tif->tif_encoderow = TIFFNoRowEncode;
    tif->tif_encodestrip = TIFFNoStripEncode;
    tif->tif_encodetile = TIFFNoTileEncode;
    tif->tif_close = NULL;

    tif->tif_options |= FAX3_NOEOL | FAX3_BYTEALIGN;
    return (1);
}

TIFFInitCCITTRLEW(tif) TIFF *tif;
{
    TIFFInitCCITTFax3(tif);
    tif->tif_preencode = NULL;
    tif->tif_postencode = NULL;
    tif->tif_encoderow = TIFFNoRowEncode;
    tif->tif_encodestrip = TIFFNoStripEncode;
    tif->tif_encodetile = TIFFNoTileEncode;
    tif->tif_close = NULL;

    tif->tif_options |= FAX3_NOEOL | FAX3_WORDALIGN;
    return (1);
}

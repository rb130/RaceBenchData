

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_fax4.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include "tif_fax3.h"

#if USE_PROTOTYPES
static int Fax4Decode(TIFF *, u_char *, int, u_int);
static int Fax4Encode(TIFF *, u_char *, int, u_int);
static int Fax4PostEncode(TIFF *);
#else
static int Fax4Decode();
static int Fax4Encode();
static int Fax4PostEncode();
#endif

TIFFInitCCITTFax4(tif) TIFF *tif;
{
    TIFFInitCCITTFax3(tif);
    tif->tif_decoderow = Fax4Decode;
    tif->tif_decodestrip = Fax4Decode;
    tif->tif_decodetile = Fax4Decode;
    tif->tif_encoderow = Fax4Encode;
    tif->tif_encodestrip = Fax4Encode;
    tif->tif_encodetile = Fax4Encode;
    tif->tif_postencode = Fax4PostEncode;

    tif->tif_options = FAX3_NOEOL | FAX3_CLASSF;
    return (1);
}

static int Fax4Decode(tif, buf, occ, s)
TIFF *tif;
u_char *buf;
int occ;
u_int s;
{
    Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

    bzero(buf, occ);
    while (occ > 0) {
        if (!Fax3Decode2DRow(tif, buf, sp->rowpixels)) {
            return (0);
        }
        bcopy(buf, sp->refline, sp->rowbytes);
        buf += sp->rowbytes;
        occ -= sp->rowbytes;
    }
    return (1);
}

static int Fax4Encode(tif, bp, cc, s)
TIFF *tif;
u_char *bp;
int cc;
u_int s;
{
    Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

    while (cc > 0) {
        if (!Fax3Encode2DRow(tif, bp, sp->refline, sp->rowpixels)) {
            return (0);
        }
        bcopy(bp, sp->refline, sp->rowbytes);
        bp += sp->rowbytes;
        cc -= sp->rowbytes;
    }
    return (1);
}

static Fax4PostEncode(tif) TIFF *tif;
{
    Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

    Fax3PutEOL(tif);
    Fax3PutEOL(tif);
    if (sp->bit != 8)
        Fax3FlushBits(tif, sp);
    return (1);
}

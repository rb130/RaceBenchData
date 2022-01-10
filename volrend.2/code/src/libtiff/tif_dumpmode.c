

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_dumpmode.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include <stdio.h>
#include <assert.h>

#if USE_PROTOTYPES
static int DumpModeEncode(TIFF *, u_char *, int, u_int);
static int DumpModeDecode(TIFF *, u_char *, int, u_int);
static int DumpModeSeek(TIFF *, int);
#else
static int DumpModeEncode(), DumpModeDecode(), DumpModeSeek();
#endif

TIFFInitDumpMode(tif) register TIFF *tif;
{
    tif->tif_decoderow = DumpModeDecode;
    tif->tif_decodestrip = DumpModeDecode;
    tif->tif_decodetile = DumpModeDecode;
    tif->tif_encoderow = DumpModeEncode;
    tif->tif_encodestrip = DumpModeEncode;
    tif->tif_encodetile = DumpModeEncode;
    tif->tif_seek = DumpModeSeek;
    return (1);
}

static int DumpModeEncode(tif, pp, cc, s)
register TIFF *tif;
u_char *pp;
int cc;
u_int s;
{

    if (tif->tif_rawcc + cc > tif->tif_rawdatasize) {
        if (!TIFFFlushData1(tif)) {
            return (-1);
        }
    }
    while (cc > 0) {
        int n;
        if ((n = cc) > tif->tif_rawdatasize) {
            n = tif->tif_rawdatasize;
        }
        bcopy(pp, tif->tif_rawcp, n);
        if (tif->tif_flags & TIFF_SWAB) {
            switch (tif->tif_dir.td_bitspersample) {
            case 16:
                assert((n & 3) == 0);
                TIFFSwabArrayOfShort((u_short *)tif->tif_rawcp, n / 2);
                break;
            case 32:
                assert((n & 15) == 0);
                TIFFSwabArrayOfLong((u_long *)tif->tif_rawcp, n / 4);
                break;
            }
        }
        tif->tif_rawcp += n;
        tif->tif_rawcc += n;
        pp += n;
        cc -= n;
        if (tif->tif_rawcc >= tif->tif_rawdatasize && !TIFFFlushData1(tif)) {
            return (-1);
        }
    }
    return (1);
}

static int DumpModeDecode(tif, buf, cc, s)
register TIFF *tif;
u_char *buf;
int cc;
u_int s;
{
    if (tif->tif_rawcc < cc) {
        TIFFError(tif->tif_name, "DumpModeDecode: Not enough data for scanline %d", tif->tif_row);
        return (0);
    }

    if (tif->tif_rawcp != (char *)buf) {
        bcopy(tif->tif_rawcp, buf, cc);
    }
    if (tif->tif_flags & TIFF_SWAB) {
        switch (tif->tif_dir.td_bitspersample) {
        case 16:
            assert((cc & 3) == 0);
            TIFFSwabArrayOfShort((u_short *)buf, cc / 2);
            break;
        case 32:
            assert((cc & 15) == 0);
            TIFFSwabArrayOfLong((u_long *)buf, cc / 4);
            break;
        }
    }
    tif->tif_rawcp += cc;
    tif->tif_rawcc -= cc;
    return (1);
}

static int DumpModeSeek(tif, nrows)
register TIFF *tif;
int nrows;
{
    tif->tif_rawcp += nrows * tif->tif_scanlinesize;
    tif->tif_rawcc -= nrows * tif->tif_scanlinesize;
    return (1);
}

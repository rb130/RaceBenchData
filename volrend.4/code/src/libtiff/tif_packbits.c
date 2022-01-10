

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_packbits.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include <stdio.h>
#include <assert.h>

#if USE_PROTOTYPES
static int PackBitsPreEncode(TIFF *);
static int PackBitsEncode(TIFF *, u_char *, int, u_int);
static int PackBitsEncodeChunk(TIFF *, u_char *, int, u_int);
static int PackBitsDecode(TIFF *, u_char *, int, u_int);
#else
static int PackBitsPreEncode();
static int PackBitsEncode(), PackBitsEncodeChunk();
static int PackBitsDecode();
#endif

TIFFInitPackBits(tif) TIFF *tif;
{
    tif->tif_decoderow = PackBitsDecode;
    tif->tif_decodestrip = PackBitsDecode;
    tif->tif_decodetile = PackBitsDecode;
    tif->tif_preencode = PackBitsPreEncode;
    tif->tif_encoderow = PackBitsEncode;
    tif->tif_encodestrip = PackBitsEncodeChunk;
    tif->tif_encodetile = PackBitsEncodeChunk;
    return (1);
}

static int PackBitsPreEncode(tif)
TIFF *tif;
{

    if (isTiled(tif)) {
        tif->tif_data = (char *)TIFFTileRowSize(tif);
    } else {
        tif->tif_data = (char *)TIFFScanlineSize(tif);
    }
    return (1);
}

static int PackBitsEncodeChunk(tif, bp, cc, s)
TIFF *tif;
u_char *bp;
int cc;
u_int s;
{
    int rowsize = (int)tif->tif_data;

    assert(rowsize > 0);
    while (cc > 0) {
        if (PackBitsEncode(tif, bp, rowsize, s) < 0) {
            return (-1);
        }
        bp += rowsize;
        cc -= rowsize;
    }
    return (1);
}

static int PackBitsEncode(tif, bp, cc, s)
TIFF *tif;
u_char *bp;
register int cc;
u_int s;
{
    register char *op, *lastliteral;
    register int n, b;
    enum { BASE, LITERAL, RUN, LITERAL_RUN } state;
    char *ep;
    int slop;

    op = tif->tif_rawcp;
    ep = tif->tif_rawdata + tif->tif_rawdatasize;
    state = BASE;
    lastliteral = 0;
    while (cc > 0) {

        b = *bp++, cc--, n = 1;
        for (; cc > 0 && b == *bp; cc--, bp++) {
            n++;
        }
    again:
        if (op + 2 >= ep) {

            if (state == LITERAL || state == LITERAL_RUN) {
                slop = op - lastliteral;
                tif->tif_rawcc += lastliteral - tif->tif_rawcp;
                if (!TIFFFlushData1(tif)) {
                    return (-1);
                }
                op = tif->tif_rawcp;
                for (; slop-- > 0; *op++ = *lastliteral++) {
                    ;
                }
                lastliteral = tif->tif_rawcp;
            } else {
                tif->tif_rawcc += op - tif->tif_rawcp;
                if (!TIFFFlushData1(tif)) {
                    return (-1);
                }
                op = tif->tif_rawcp;
            }
        }
        switch (state) {
        case BASE:
            if (n > 1) {
                state = RUN;
                if (n > 128) {
                    *op++ = -127;
                    *op++ = b;
                    n -= 128;
                    goto again;
                }
                *op++ = -(n - 1);
                *op++ = b;
            } else {
                lastliteral = op;
                *op++ = 0;
                *op++ = b;
                state = LITERAL;
            }
            break;
        case LITERAL:
            if (n > 1) {
                state = LITERAL_RUN;
                if (n > 128) {
                    *op++ = -127;
                    *op++ = b;
                    n -= 128;
                    goto again;
                }
                *op++ = -(n - 1);
                *op++ = b;
            } else {
                if (++(*lastliteral) == 127) {
                    state = BASE;
                }
                *op++ = b;
            }
            break;
        case RUN:
            if (n > 1) {
                if (n > 128) {
                    *op++ = -127;
                    *op++ = b;
                    n -= 128;
                    goto again;
                }
                *op++ = -(n - 1);
                *op++ = b;
            } else {
                lastliteral = op;
                *op++ = 0;
                *op++ = b;
                state = LITERAL;
            }
            break;
        case LITERAL_RUN:

            if (n == 1 && op[-2] == (char)-1 && *lastliteral < 126) {
                state = (((*lastliteral) += 2) == 127 ? BASE : LITERAL);
                op[-2] = op[-1];
            } else {
                state = RUN;
            }
            goto again;
        }
    }
    tif->tif_rawcc += op - tif->tif_rawcp;
    tif->tif_rawcp = op;
    return (1);
}

static int PackBitsDecode(tif, op, occ, s)
TIFF *tif;
register u_char *op;
register int occ;
u_int s;
{
    register char *bp;
    register int n, b;
    register int cc;

    bp = tif->tif_rawcp;
    cc = tif->tif_rawcc;
    while (cc > 0 && occ > 0) {
        n = (int)*bp++;

        if (n >= 128) {
            n -= 256;
        }
        if (n < 0) {
            cc--;
            if (n == -128) {
                continue;
            }
            n = -n + 1;
            occ -= n;
            for (b = *bp++; n-- > 0;) {
                *op++ = b;
            }
        } else {
            bcopy(bp, op, ++n);
            op += n;
            occ -= n;
            bp += n;
            cc -= n;
        }
    }
    tif->tif_rawcp = bp;
    tif->tif_rawcc = cc;
    if (occ > 0) {
        TIFFError(tif->tif_name, "PackBitsDecode: Not enough data for scanline %d", tif->tif_row);
        return (0);
    }

    return (1);
}

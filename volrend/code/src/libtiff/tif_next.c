
#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_next.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"

#if USE_PROTOTYPES
static int NeXTDecode(TIFF *, u_char *, int, u_int);
#else
static int NeXTDecode();
#endif

TIFFInitNeXT(tif) TIFF *tif;
{
    tif->tif_decoderow = NeXTDecode;
    tif->tif_decodestrip = NeXTDecode;
    tif->tif_decodetile = NeXTDecode;
    return (1);
}

#define SETPIXEL(op, v)          \
    {                            \
        switch (npixels++ & 3) { \
        case 0:                  \
            op[0] = (v) << 6;    \
            break;               \
        case 1:                  \
            op[0] |= (v) << 4;   \
            break;               \
        case 2:                  \
            op[0] |= (v) << 2;   \
            break;               \
        case 3:                  \
            *op++ |= (v);        \
            break;               \
        }                        \
    }

#define LITERALROW 0x00
#define LITERALSPAN 0x40
#define WHITE ((1 << 2) - 1)

static int NeXTDecode(tif, buf, occ, s) TIFF *tif;
u_char *buf;
int occ;
u_int s;
{
    register u_char *bp, *op;
    register int cc, n;
    u_char *row;
    int scanline;

    for (op = buf, cc = occ; cc-- > 0;) {
        *op++ = 0xff;
    }

    bp = (u_char *)tif->tif_rawcp;
    cc = tif->tif_rawcc;
    scanline = tif->tif_scanlinesize;
    for (row = buf; occ > 0; occ -= scanline, row += scanline) {
        n = *bp++, cc--;
        switch (n) {
        case LITERALROW:

            if (cc < scanline) {
                goto bad;
            }
            bcopy(bp, row, scanline);
            bp += scanline;
            cc -= scanline;
            break;
        case LITERALSPAN: {
            int off;

            off = (bp[0] * 256) + bp[1];
            n = (bp[2] * 256) + bp[3];
            if (cc < 4 + n) {
                goto bad;
            }
            bcopy(bp + 4, row + off, n);
            bp += 4 + n;
            cc -= 4 + n;
            break;
        }
        default: {
            register int npixels = 0, grey;
            int imagewidth = tif->tif_dir.td_imagewidth;

            op = row;
            for (;;) {
                grey = (n >> 6) & 0x3;
                n &= 0x3f;
                while (n-- > 0)
                    SETPIXEL(op, grey);
                if (npixels >= imagewidth) {
                    break;
                }
                if (cc == 0) {
                    goto bad;
                }
                n = *bp++, cc--;
            }
            break;
        }
        }
    }
    tif->tif_rawcp = (char *)bp;
    tif->tif_rawcc = cc;
    return (1);
bad:
    TIFFError(tif->tif_name, "NeXTDecode: Not enough data for scanline %d", tif->tif_row);
    return (0);
}

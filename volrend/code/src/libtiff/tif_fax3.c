
#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_fax3.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include <stdio.h>
#include <assert.h>
#include "tif_fax3.h"
#define G3CODES
#include "t4.h"
#define G3STATES
#include "g3states.h"

typedef struct {
    Fax3BaseState b;
} Fax3DecodeState;

typedef struct {
    Fax3BaseState b;
    u_char *wruns;
    u_char *bruns;
    short k;
    short maxk;
} Fax3EncodeState;

#if USE_PROTOTYPES
static Fax3PreDecode(TIFF *);
static Fax3Decode(TIFF *, u_char *, int, u_int);
static int Fax3Decode1DRow(TIFF *, u_char *, int);
static Fax3PreEncode(TIFF *);
static Fax3PostEncode(TIFF *);
static Fax3Encode(TIFF *, u_char *, int, u_int);
static int Fax3Encode1DRow(TIFF *, u_char *, int);
static Fax3Close(TIFF *);
static Fax3Cleanup(TIFF *);
static void *Fax3SetupState(TIFF *, int);
static void fillspan(char *, int, int);
static int findspan(u_char **, int, int, u_char const *);
static int finddiff(u_char *, int, int, int);
static void skiptoeol(TIFF *, int);
static void putbits(TIFF *, u_int, u_int);
static void putcode(TIFF *, tableentry const *);
static void putspan(TIFF *, int, tableentry const *);
extern int TIFFFlushData1(TIFF *);
#else
static int Fax3PreEncode(), Fax3Encode(), Fax3PostEncode();
static int Fax3Encode1DRow();
static int Fax3Decode(), Fax3PreDecode();
static int Fax3Decode1DRow();
static int Fax3Close(), Fax3Cleanup();
static void *Fax3SetupState();
static void fillspan();
static int findspan();
static int finddiff();
static void skiptoeol();
static void putbits();
static void putcode();
static void putspan();
extern int TIFFFlushData1();
#endif

TIFFInitCCITTFax3(tif) TIFF *tif;
{
    tif->tif_predecode = Fax3PreDecode;
    tif->tif_decoderow = Fax3Decode;
    tif->tif_decodestrip = Fax3Decode;
    tif->tif_decodetile = Fax3Decode;
    tif->tif_preencode = Fax3PreEncode;
    tif->tif_postencode = Fax3PostEncode;
    tif->tif_encoderow = Fax3Encode;
    tif->tif_encodestrip = Fax3Encode;
    tif->tif_encodetile = Fax3Encode;
    tif->tif_close = Fax3Close;
    tif->tif_cleanup = Fax3Cleanup;
    tif->tif_options |= FAX3_CLASSF;
    tif->tif_flags |= TIFF_NOBITREV;
    return (1);
}

TIFFModeCCITTFax3(tif, isClassF) TIFF *tif;
int isClassF;
{
    if (isClassF) {
        tif->tif_options |= FAX3_CLASSF;
    } else {
        tif->tif_options &= ~FAX3_CLASSF;
    }
}

static u_char bitMask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
#define isBitSet(sp) ((sp)->b.data & bitMask[(sp)->b.bit])

#define is2DEncoding(tif) (tif->tif_dir.td_group3options & GROUP3OPT_2DENCODING)
#define fetchByte(tif, sp) ((tif)->tif_rawcc--, (sp)->b.bitmap[*(u_char *)(tif)->tif_rawcp++])

#define BITCASE(b)                 \
    case b:                        \
        code <<= 1;                \
        if (data & (1 << (7 - b))) \
            code |= 1;             \
        len++;                     \
        if (code > 0) {            \
            bit = b + 1;           \
            break;                 \
        }

static void skiptoeol(tif, len) TIFF *tif;
int len;
{
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
    register int bit = sp->b.bit;
    register int data = sp->b.data;
    int code = 0;

    if (bit == 0) {
        bit = 8;
    }
    for (;;) {
        switch (bit) {
        again:
            BITCASE(0);
            BITCASE(1);
            BITCASE(2);
            BITCASE(3);
            BITCASE(4);
            BITCASE(5);
            BITCASE(6);
            BITCASE(7);
        default:
            if (tif->tif_rawcc <= 0) {
                return;
            }
            data = fetchByte(tif, sp);
            goto again;
        }
        if (len >= 12 && code == EOL) {
            break;
        }
        code = len = 0;
    }
    sp->b.bit = bit > 7 ? 0 : bit;
    sp->b.data = data;
}

static int nextbit(tif) TIFF *tif;
{
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
    int bit;

    if (sp->b.bit == 0 && tif->tif_rawcc > 0) {
        sp->b.data = fetchByte(tif, sp);
    }
    bit = isBitSet(sp);
    if (++(sp->b.bit) > 7) {
        sp->b.bit = 0;
    }
    return (bit);
}

static void bset(cp, n, v) register unsigned char *cp;
register int n;
register int v;
{
    while (n-- > 0) {
        *cp++ = v;
    }
}

static void *Fax3SetupState(tif, space) TIFF *tif;
int space;
{
    TIFFDirectory *td = &tif->tif_dir;
    Fax3BaseState *sp;
    int cc = space;
    long rowbytes, rowpixels;

    if (td->td_bitspersample != 1) {
        TIFFError(tif->tif_name, "Bits/sample must be 1 for Group 3/4 encoding/decoding");
        return (0);
    }

    if (isTiled(tif)) {
        rowbytes = TIFFTileRowSize(tif);
        rowpixels = tif->tif_dir.td_tilewidth;
    } else {
        rowbytes = TIFFScanlineSize(tif);
        rowpixels = tif->tif_dir.td_imagewidth;
    }
    if (is2DEncoding(tif) || td->td_compression == COMPRESSION_CCITTFAX4) {
        cc += rowbytes + 1;
    }
    tif->tif_data = malloc(cc);
    if (tif->tif_data == NULL) {
        TIFFError("Fax3SetupState", "%s: No space for Fax3 state block", tif->tif_name);
        return (0);
    }
    sp = (Fax3BaseState *)tif->tif_data;
    sp->rowbytes = rowbytes;
    sp->rowpixels = rowpixels;
    sp->bitmap = (tif->tif_fillorder != td->td_fillorder ? TIFFBitRevTable : TIFFNoBitRevTable);
    sp->white = (td->td_photometric == PHOTOMETRIC_MINISBLACK);
    if (is2DEncoding(tif) || td->td_compression == COMPRESSION_CCITTFAX4) {

        sp->refline = (u_char *)tif->tif_data + space + 1;

        sp->refline[-1] = sp->white ? 0xff : 0x00;
    } else {
        sp->refline = 0;
    }
    return (sp);
}

static Fax3PreDecode(tif) TIFF *tif;
{
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;

    if (sp == NULL) {
        sp = (Fax3DecodeState *)Fax3SetupState(tif, sizeof(*sp));
        if (!sp) {
            return (0);
        }
    }
    sp->b.bit = 0;
    sp->b.data = 0;
    sp->b.tag = G3_1D;
    if (sp->b.refline) {
        bset(sp->b.refline, sp->b.rowbytes, sp->b.white ? 0xff : 0x00);
    }

    if ((tif->tif_options & FAX3_NOEOL) == 0) {
        skiptoeol(tif, 0);
        if (is2DEncoding(tif)) {

            sp->b.tag = nextbit(tif) ? G3_1D : G3_2D;
        }
    }
    return (1);
}

static void fillspan(cp, x, count) register char *cp;
register int x, count;
{
    static const unsigned char masks[] = {0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

    if (count <= 0) {
        return;
    }
    cp += x >> 3;
    if (x &= 7) {
        if (count < 8 - x) {
            *cp++ |= masks[count] >> x;
            return;
        }
        *cp++ |= 0xff >> x;
        count -= 8 - x;
    }
    while (count >= 8) {
        *cp++ = 0xff;
        count -= 8;
    }
    *cp |= masks[count];
}

static Fax3Decode(tif, buf, occ, s) TIFF *tif;
u_char *buf;
int occ;
u_int s;
{
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;

    bzero(buf, occ);
    while (occ > 0) {
        if (sp->b.tag == G3_1D) {
            if (!Fax3Decode1DRow(tif, buf, sp->b.rowpixels)) {
                return (0);
            }
        } else {
            if (!Fax3Decode2DRow(tif, buf, sp->b.rowpixels)) {
                return (0);
            }
        }
        if (is2DEncoding(tif)) {

            sp->b.tag = nextbit(tif) ? G3_1D : G3_2D;
            if (sp->b.tag == G3_2D) {
                bcopy(buf, sp->b.refline, sp->b.rowbytes);
            }
        }
        buf += sp->b.rowbytes;
        occ -= sp->b.rowbytes;
    }
    return (1);
}

static int decode_white_run(tif) TIFF *tif;
{
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
    short state = sp->b.bit;
    short action;
    int runlen = 0;

    for (;;) {
        if (sp->b.bit == 0) {
        nextbyte:
            if (tif->tif_rawcc <= 0) {
                return (G3CODE_EOF);
            }
            sp->b.data = fetchByte(tif, sp);
        }
        action = TIFFFax1DAction[state][sp->b.data];
        state = TIFFFax1DNextState[state][sp->b.data];
        if (action == ACT_INCOMP) {
            goto nextbyte;
        }
        if (action == ACT_INVALID) {
            return (G3CODE_INVALID);
        }
        if (action == ACT_EOL) {
            return (G3CODE_EOL);
        }
        sp->b.bit = state;
        action = RUNLENGTH(action - ACT_WRUNT);
        runlen += action;
        if (action < 64) {
            return (runlen);
        }
    }
}

static int decode_black_run(tif) TIFF *tif;
{
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
    short state = sp->b.bit + 8;
    short action;
    int runlen = 0;

    for (;;) {
        if (sp->b.bit == 0) {
        nextbyte:
            if (tif->tif_rawcc <= 0) {
                return (G3CODE_EOF);
            }
            sp->b.data = fetchByte(tif, sp);
        }
        action = TIFFFax1DAction[state][sp->b.data];
        state = TIFFFax1DNextState[state][sp->b.data];
        if (action == ACT_INCOMP) {
            goto nextbyte;
        }
        if (action == ACT_INVALID) {
            return (G3CODE_INVALID);
        }
        if (action == ACT_EOL) {
            return (G3CODE_EOL);
        }
        sp->b.bit = state;
        action = RUNLENGTH(action - ACT_BRUNT);
        runlen += action;
        if (action < 64) {
            return (runlen);
        }
        state += 8;
    }
}

static int Fax3Decode1DRow(tif, buf, npels) TIFF *tif;
u_char *buf;
int npels;
{
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
    int x = 0;
    int runlen;
    short action;
    short color = sp->b.white;
    static char module[] = "Fax3Decode1D";

    for (;;) {
        if (color == sp->b.white) {
            runlen = decode_white_run(tif);
        } else {
            runlen = decode_black_run(tif);
        }
        switch (runlen) {
        case G3CODE_EOF:
            TIFFError(module, "%s: Premature EOF at scanline %d (x %d)", tif->tif_name, tif->tif_row, x);
            return (0);
        case G3CODE_INVALID:

            TIFFError(module, "%s: Bad code word at scanline %d (x %d)", tif->tif_name, tif->tif_row, x);
            goto done;
        case G3CODE_EOL:
            TIFFWarning(module, "%s: Premature EOL at scanline %d (x %d)", tif->tif_name, tif->tif_row, x);
            return (1);
        }
        if (x + runlen > npels) {
            runlen = npels - x;
        }
        if (runlen > 0) {
            if (color) {
                fillspan((char *)buf, x, runlen);
            }
            x += runlen;
            if (x >= npels) {
                break;
            }
        }
        color = !color;
    }
done:

    if ((tif->tif_options & FAX3_NOEOL) == 0) {
        skiptoeol(tif, 0);
    }
    if (tif->tif_options & FAX3_BYTEALIGN) {
        sp->b.bit = 0;
    }
    if ((tif->tif_options & FAX3_WORDALIGN) && ((long)tif->tif_rawcp & 1)) {
        (void)fetchByte(tif, sp);
    }
    return (x == npels);
}

static int decode_uncomp_code(tif) TIFF *tif;
{
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
    short code;

    do {
        if (sp->b.bit == 0 || sp->b.bit > 7) {
            if (tif->tif_rawcc <= 0) {
                return (UNCOMP_EOF);
            }
            sp->b.data = fetchByte(tif, sp);
        }
        code = TIFFFaxUncompAction[sp->b.bit][sp->b.data];
        sp->b.bit = TIFFFaxUncompNextState[sp->b.bit][sp->b.data];
    } while (code == ACT_INCOMP);
    return (code);
}

int Fax3Decode2DRow(tif, buf, npels) TIFF *tif;
u_char *buf;
int npels;
{
#define PIXEL(buf, ix) ((((buf)[(ix) >> 3]) >> (7 - ((ix)&7))) & 1)
    Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
    int a0 = -1;
    int b1, b2;
    int run1, run2;
    short mode;
    short color = sp->b.white;
    static char module[] = "Fax3Decode2D";

    do {
        if (sp->b.bit == 0 || sp->b.bit > 7) {
            if (tif->tif_rawcc <= 0) {
                TIFFError(module, "%s: Premature EOF at scanline %d", tif->tif_name, tif->tif_row);
                return (0);
            }
            sp->b.data = fetchByte(tif, sp);
        }
        mode = TIFFFax2DMode[sp->b.bit][sp->b.data];
        sp->b.bit = TIFFFax2DNextState[sp->b.bit][sp->b.data];
        switch (mode) {
        case MODE_NULL:
            break;
        case MODE_PASS:
            b2 = finddiff(sp->b.refline, a0, npels, !color);
            b1 = finddiff(sp->b.refline, b2, npels, color);
            b2 = finddiff(sp->b.refline, b1, npels, !color);
            if (color) {
                if (a0 < 0) {
                    a0 = 0;
                }
                fillspan((char *)buf, a0, b2 - a0);
            }
            a0 = b2;
            break;
        case MODE_HORIZ:
            if (color == sp->b.white) {
                run1 = decode_white_run(tif);
                run2 = decode_black_run(tif);
            } else {
                run1 = decode_black_run(tif);
                run2 = decode_white_run(tif);
            }

            if (a0 < 0) {
                a0 = 0;
            }
            if (a0 + run1 > npels) {
                run1 = npels - a0;
            }
            if (color) {
                fillspan((char *)buf, a0, run1);
            }
            a0 += run1;
            if (a0 + run2 > npels) {
                run2 = npels - a0;
            }
            if (!color) {
                fillspan((char *)buf, a0, run2);
            }
            a0 += run2;
            break;
        case MODE_VERT_V0:
        case MODE_VERT_VR1:
        case MODE_VERT_VR2:
        case MODE_VERT_VR3:
        case MODE_VERT_VL1:
        case MODE_VERT_VL2:
        case MODE_VERT_VL3:
            b2 = finddiff(sp->b.refline, a0, npels, !color);
            b1 = finddiff(sp->b.refline, b2, npels, color);
            b1 += mode - MODE_VERT_V0;
            if (color) {
                if (a0 < 0) {
                    a0 = 0;
                }
                fillspan((char *)buf, a0, b1 - a0);
            }
            color = !color;
            a0 = b1;
            break;
        case MODE_UNCOMP:

            if (a0 < 0) {
                a0 = 0;
            }
            do {
                mode = decode_uncomp_code(tif);
                switch (mode) {
                case UNCOMP_RUN1:
                case UNCOMP_RUN2:
                case UNCOMP_RUN3:
                case UNCOMP_RUN4:
                case UNCOMP_RUN5:
                    run1 = mode - UNCOMP_RUN0;
                    fillspan((char *)buf, a0 + run1 - 1, 1);
                    a0 += run1;
                    break;
                case UNCOMP_RUN6:
                    a0 += 5;
                    break;
                case UNCOMP_TRUN0:
                case UNCOMP_TRUN1:
                case UNCOMP_TRUN2:
                case UNCOMP_TRUN3:
                case UNCOMP_TRUN4:
                    run1 = mode - UNCOMP_TRUN0;
                    a0 += run1;
                    color = nextbit(tif) ? !sp->b.white : sp->b.white;
                    break;
                case UNCOMP_INVALID:
                    TIFFError(module, "%s: Bad uncompressed code word at scanline %d", tif->tif_name, tif->tif_row);
                    goto bad;
                case UNCOMP_EOF:
                    TIFFError(module, "%s: Premature EOF at scanline %d", tif->tif_name, tif->tif_row);
                    return (0);
                }
            } while (mode < UNCOMP_EXIT);
            break;
        case MODE_ERROR_1:
            if ((tif->tif_options & FAX3_NOEOL) == 0) {
                TIFFWarning(module, "%s: Premature EOL at scanline %d (x %d)", tif->tif_name, tif->tif_row, a0);
                skiptoeol(tif, 7);
                return (1);
            }

        case MODE_ERROR:
            TIFFError(module, "%s: Bad 2D code word at scanline %d", tif->tif_name, tif->tif_row);
            goto bad;
        default:
            TIFFError(module, "%s: Panic, bad decoding state at scanline %d", tif->tif_name, tif->tif_row);
            return (0);
        }
    } while (a0 < npels);
bad:

    if ((tif->tif_options & FAX3_NOEOL) == 0) {
        skiptoeol(tif, 0);
    }
    return (a0 >= npels);
#undef PIXEL
}

static void putbits(tif, bits, length) TIFF *tif;
u_int bits, length;
{
    Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;
    static const int mask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

    while (length > sp->bit) {
        sp->data |= bits >> (length - sp->bit);
        length -= sp->bit;
        Fax3FlushBits(tif, sp);
    }
    sp->data |= (bits & mask[length]) << (sp->bit - length);
    sp->bit -= length;
    if (sp->bit == 0)
        Fax3FlushBits(tif, sp);
}

static void putcode(tif, te) TIFF *tif;
tableentry const *te;
{ putbits(tif, te->code, te->length); }

static void putspan(tif, span, tab) TIFF *tif;
int span;
tableentry const *tab;
{
    while (span >= 2624) {
        tableentry const *te = &tab[63 + (2560 >> 6)];
        putcode(tif, te);
        span -= te->runlen;
    }
    if (span >= 64) {
        tableentry const *te = &tab[63 + (span >> 6)];
        assert(te->runlen == 64 * (span >> 6));
        putcode(tif, te);
        span -= te->runlen;
    }
    putcode(tif, &tab[span]);
}

void Fax3PutEOL(tif) TIFF *tif;
{
    Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

    if (tif->tif_dir.td_group3options & GROUP3OPT_FILLBITS) {

        int align = 8 - 4;
        if (align != sp->bit) {
            if (align > sp->bit) {
                align = sp->bit + (8 - align);
            } else {
                align = sp->bit - align;
            }
            putbits(tif, 0, align);
        }
    }
    putbits(tif, EOL, 12);
    if (is2DEncoding(tif)) {
        putbits(tif, sp->tag == G3_1D, 1);
    }
}

static const u_char zeroruns[256] = {
    8,
    7,
    6,
    6,
    5,
    5,
    5,
    5,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
static const u_char oneruns[256] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    5,
    5,
    5,
    5,
    6,
    6,
    7,
    8,
};

static Fax3PreEncode(tif) TIFF *tif;
{
    Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;

    if (sp == NULL) {
        sp = (Fax3EncodeState *)Fax3SetupState(tif, sizeof(*sp));
        if (!sp) {
            return (0);
        }
        if (sp->b.white == 0) {
            sp->wruns = zeroruns;
            sp->bruns = oneruns;
        } else {
            sp->wruns = oneruns;
            sp->bruns = zeroruns;
        }
    }
    sp->b.bit = 8;
    sp->b.data = 0;
    sp->b.tag = G3_1D;

    if (sp->b.refline) {
        bset(sp->b.refline, sp->b.rowbytes, sp->b.white ? 0xff : 0x00);
    }
    if (is2DEncoding(tif)) {
        float res = tif->tif_dir.td_yresolution;

        if (tif->tif_dir.td_resolutionunit == RESUNIT_CENTIMETER) {
            res = (res * .3937) / 2.54;
        }
        sp->maxk = (res > 150 ? 4 : 2);
        sp->k = sp->maxk - 1;
    } else {
        sp->k = sp->maxk = 0;
    }
    return (1);
}

static int Fax3Encode1DRow(tif, bp, bits) TIFF *tif;
u_char *bp;
int bits;
{
    Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;
    int bs = 0, span;

    for (;;) {
        span = findspan(&bp, bs, bits, sp->wruns);
        putspan(tif, span, TIFFFaxWhiteCodes);
        bs += span;
        if (bs >= bits) {
            break;
        }
        span = findspan(&bp, bs, bits, sp->bruns);
        putspan(tif, span, TIFFFaxBlackCodes);
        bs += span;
        if (bs >= bits) {
            break;
        }
    }
    return (1);
}

static const tableentry horizcode = {3, 0x1};
static const tableentry passcode = {4, 0x1};
static const tableentry vcodes[7] = {{7, 0x03}, {6, 0x03}, {3, 0x03}, {1, 0x1}, {3, 0x2}, {6, 0x02}, {7, 0x02}};

int Fax3Encode2DRow(tif, bp, rp, bits) TIFF *tif;
u_char *bp, *rp;
int bits;
{
#define PIXEL(buf, ix) ((((buf)[(ix) >> 3]) >> (7 - ((ix)&7))) & 1)
    short white = ((Fax3BaseState *)tif->tif_data)->white;
    int a0 = 0;
    int a1 = (PIXEL(bp, 0) != white ? 0 : finddiff(bp, 0, bits, white));
    int b1 = (PIXEL(rp, 0) != white ? 0 : finddiff(rp, 0, bits, white));
    int a2, b2;

    for (;;) {
        b2 = finddiff(rp, b1, bits, PIXEL(rp, b1));
        if (b2 >= a1) {
            int d = b1 - a1;
            if (!(-3 <= d && d <= 3)) {
                a2 = finddiff(bp, a1, bits, PIXEL(bp, a1));
                putcode(tif, &horizcode);
                if (a0 + a1 == 0 || PIXEL(bp, a0) == white) {
                    putspan(tif, a1 - a0, TIFFFaxWhiteCodes);
                    putspan(tif, a2 - a1, TIFFFaxBlackCodes);
                } else {
                    putspan(tif, a1 - a0, TIFFFaxBlackCodes);
                    putspan(tif, a2 - a1, TIFFFaxWhiteCodes);
                }
                a0 = a2;
            } else {
                putcode(tif, &vcodes[d + 3]);
                a0 = a1;
            }
        } else {
            putcode(tif, &passcode);
            a0 = b2;
        }
        if (a0 >= bits) {
            break;
        }
        a1 = finddiff(bp, a0, bits, PIXEL(bp, a0));
        b1 = finddiff(rp, a0, bits, !PIXEL(bp, a0));
        b1 = finddiff(rp, b1, bits, PIXEL(bp, a0));
    }
    return (1);
#undef PIXEL
}

static int Fax3Encode(tif, bp, cc, s) TIFF *tif;
u_char *bp;
int cc;
u_int s;
{
    Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;

    while (cc > 0) {
        Fax3PutEOL(tif);
        if (is2DEncoding(tif)) {
            if (sp->b.tag == G3_1D) {
                if (!Fax3Encode1DRow(tif, bp, sp->b.rowpixels)) {
                    return (0);
                }
                sp->b.tag = G3_2D;
            } else {
                if (!Fax3Encode2DRow(tif, bp, sp->b.refline, sp->b.rowpixels)) {
                    return (0);
                }
                sp->k--;
            }
            if (sp->k == 0) {
                sp->b.tag = G3_1D;
                sp->k = sp->maxk - 1;
            } else {
                bcopy(bp, sp->b.refline, sp->b.rowbytes);
            }
        } else {
            if (!Fax3Encode1DRow(tif, bp, sp->b.rowpixels)) {
                return (0);
            }
        }
        bp += sp->b.rowbytes;
        cc -= sp->b.rowbytes;
    }
    return (1);
}

static int Fax3PostEncode(tif) TIFF *tif;
{
    Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

    if (sp->bit != 8)
        Fax3FlushBits(tif, sp);
    return (1);
}

static Fax3Close(tif) TIFF *tif;
{
    if ((tif->tif_options & FAX3_CLASSF) == 0) {
        int i;
        for (i = 0; i < 6; i++) {
            Fax3PutEOL(tif);
        }
        (void)Fax3PostEncode(tif);
    }
}

static Fax3Cleanup(tif) TIFF *tif;
{
    if (tif->tif_data) {
        free(tif->tif_data);
        tif->tif_data = NULL;
    }
}

static int findspan(bpp, bs, be, tab) u_char **bpp;
int bs, be;
register u_char const *tab;
{
    register u_char *bp = *bpp;
    register int bits = be - bs;
    register int n, span;

    if (bits > 0 && (n = (bs & 7))) {
        span = tab[(*bp << n) & 0xff];
        if (span > 8 - n) {
            span = 8 - n;
        }
        if (span > bits) {
            span = bits;
        }
        if (n + span < 8) {
            goto done;
        }
        bits -= span;
        bp++;
    } else {
        span = 0;
    }

    while (bits >= 8) {
        n = tab[*bp];
        span += n;
        bits -= n;
        if (n < 8) {
            goto done;
        }
        bp++;
    }

    if (bits > 0) {
        n = tab[*bp];
        span += (n > bits ? bits : n);
    }
done:
    *bpp = bp;
    return (span);
}

static int finddiff(cp, bs, be, color) u_char *cp;
int bs, be, color;
{
    cp += bs >> 3;
    return (bs + findspan(&cp, bs, be, color ? oneruns : zeroruns));
}



#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_lzw.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include <stdio.h>
#include <assert.h>
#include "prototypes.h"

#define MAXCODE(n) ((1 << (n)) - 1)

#define BITS_MIN 9
#define BITS_MAX 12

#define CODE_CLEAR 256
#define CODE_EOI 257
#define CODE_FIRST 258
#define CODE_MAX MAXCODE(BITS_MAX)
#ifdef notdef
#define HSIZE 9001
#define HSHIFT (8 - (16 - 13))
#else
#define HSIZE 5003
#define HSHIFT (8 - (16 - 12))
#endif

#define SetMaxCode(sp, v)                 \
    {                                     \
        (sp)->lzw_maxcode = (v)-1;        \
        if ((sp)->lzw_flags & LZW_COMPAT) \
            (sp)->lzw_maxcode++;          \
    }

struct decode {
    short prefixtab[HSIZE];
    u_char suffixtab[CODE_MAX + 1];
    u_char stack[HSIZE - (CODE_MAX + 1)];
    u_char *stackp;
    int firstchar;
};

struct encode {
    long checkpoint;
#define CHECK_GAP 10000
    long ratio;
    long incount;
    long outcount;
    long htab[HSIZE];
    short codetab[HSIZE];
};

#if USE_PROTOTYPES
typedef void (*predictorFunc)(char *data, int nbytes, int stride);
#else
typedef void (*predictorFunc)();
#endif

typedef struct {
    int lzw_oldcode;
    u_short lzw_flags;
#define LZW_RESTART 0x01
#define LZW_COMPAT 0x02
    u_short lzw_nbits;
    u_short lzw_stride;
    u_short lzw_rowsize;
    predictorFunc lzw_hordiff;
    int lzw_maxcode;
    long lzw_bitoff;
    long lzw_bitsize;
    int lzw_free_ent;
    union {
        struct decode dec;
        struct encode enc;
    } u;
} LZWState;
#define dec_prefix u.dec.prefixtab
#define dec_suffix u.dec.suffixtab
#define dec_stack u.dec.stack
#define dec_stackp u.dec.stackp
#define dec_firstchar u.dec.firstchar

#define enc_checkpoint u.enc.checkpoint
#define enc_ratio u.enc.ratio
#define enc_incount u.enc.incount
#define enc_outcount u.enc.outcount
#define enc_htab u.enc.htab
#define enc_codetab u.enc.codetab

static const u_char rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};
static const u_char lmask[9] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};

#if USE_PROTOTYPES
static int LZWPreEncode(TIFF *);
static int LZWEncode(TIFF *, u_char *, int, u_int);
static int LZWEncodePredRow(TIFF *, u_char *, int, u_int);
static int LZWEncodePredTile(TIFF *, u_char *, int, u_int);
static int LZWPostEncode(TIFF *);
static int LZWDecode(TIFF *, u_char *, int, u_int);
static int LZWDecodePredRow(TIFF *, u_char *, int, u_int);
static int LZWDecodePredTile(TIFF *, u_char *, int, u_int);
static int LZWPreDecode(TIFF *);
static int LZWCleanup(TIFF *);
static int GetNextCode(TIFF *);
static void PutNextCode(TIFF *, int);
static void cl_block(TIFF *);
static void cl_hash(LZWState *);
extern int TIFFFlushData1(TIFF *);
#else
static int LZWPreEncode(), LZWEncode(), LZWPostEncode();
static int LZWEncodePredRow(), LZWEncodePredTile();
static int LZWPreDecode(), LZWDecode();
static int LZWDecodePredRow(), LZWDecodePredTile();
static int LZWCleanup();
static int GetNextCode();
static void PutNextCode();
static void cl_block();
static void cl_hash();
extern int TIFFFlushData1();
#endif

TIFFInitLZW(tif) TIFF *tif;
{
    tif->tif_predecode = LZWPreDecode;
    tif->tif_decoderow = LZWDecode;
    tif->tif_decodestrip = LZWDecode;
    tif->tif_decodetile = LZWDecode;
    tif->tif_preencode = LZWPreEncode;
    tif->tif_postencode = LZWPostEncode;
    tif->tif_encoderow = LZWEncode;
    tif->tif_encodestrip = LZWEncode;
    tif->tif_encodetile = LZWEncode;
    tif->tif_cleanup = LZWCleanup;
    return (1);
}

static DECLARE4(LZWCheckPredictor, TIFF *, tif, LZWState *, sp, predictorFunc, pred8bit, predictorFunc, pred16bit) {
    TIFFDirectory *td = &tif->tif_dir;

    switch (td->td_predictor) {
    case 1:
        break;
    case 2:
        sp->lzw_stride = (td->td_planarconfig == PLANARCONFIG_CONTIG ? td->td_samplesperpixel : 1);
        switch (td->td_bitspersample) {
        case 8:
            sp->lzw_hordiff = pred8bit;
            break;
        case 16:
            sp->lzw_hordiff = pred16bit;
            break;
        default:
            TIFFError(tif->tif_name, "Horizontal differencing \"Predictor\" not supported with %d-bit samples", td->td_bitspersample);
            return (0);
        }
        break;
    default:
        TIFFError(tif->tif_name, "\"Predictor\" value %d not supported", td->td_predictor);
        return (0);
    }
    if (sp->lzw_hordiff) {

        if (isTiled(tif)) {
            sp->lzw_rowsize = TIFFTileRowSize(tif);
        } else {
            sp->lzw_rowsize = TIFFScanlineSize(tif);
        }
    }
    return (1);
}

#define REPEAT4(n, op)                \
    switch (n) {                      \
    default: {                        \
        int i;                        \
        for (i = n - 4; i > 0; i--) { \
            op;                       \
        }                             \
    }                                 \
    case 4:                           \
        op;                           \
    case 3:                           \
        op;                           \
    case 2:                           \
        op;                           \
    case 1:                           \
        op;                           \
    case 0:;                          \
    }

static void DECLARE3(horizontalAccumulate8, register char *, cp, register int, cc, register int, stride) {
    if (cc > stride) {
        cc -= stride;
        do {
            REPEAT4(stride, cp[stride] += cp[0]; cp++)
            cc -= stride;
        } while (cc > 0);
    }
}

static void DECLARE3(horizontalAccumulate16, char *, cp, int, cc, register int, stride) {
    register short *wp = (short *)cp;
    register int wc = cc / 2;

    if (wc > stride) {
        wc -= stride;
        do {
            REPEAT4(stride, wp[stride] += wp[0]; wp++)
            wc -= stride;
        } while (wc > 0);
    }
}

static LZWPreDecode(tif) TIFF *tif;
{
    register LZWState *sp = (LZWState *)tif->tif_data;
    register int code;

    if (sp == NULL) {
        tif->tif_data = malloc(sizeof(LZWState));
        if (tif->tif_data == NULL) {
            TIFFError("LZWPreDecode", "No space for LZW state block");
            return (0);
        }
        sp = (LZWState *)tif->tif_data;
        sp->lzw_flags = 0;
        sp->lzw_hordiff = 0;
        sp->lzw_rowsize = 0;
        if (!LZWCheckPredictor(tif, sp, horizontalAccumulate8, horizontalAccumulate16)) {
            return (0);
        }
        if (sp->lzw_hordiff) {

            tif->tif_decoderow = LZWDecodePredRow;
            tif->tif_decodestrip = LZWDecodePredTile;
            tif->tif_decodetile = LZWDecodePredTile;
        }
    } else {
        sp->lzw_flags &= ~LZW_RESTART;
    }
    sp->lzw_nbits = BITS_MIN;

    for (code = 255; code >= 0; code--) {
        sp->dec_suffix[code] = (u_char)code;
    }
    sp->lzw_free_ent = CODE_FIRST;
    sp->lzw_bitoff = 0;

    sp->lzw_bitsize = tif->tif_rawdatasize;
    sp->lzw_bitsize = (sp->lzw_bitsize << 3) - (BITS_MAX - 1);
    sp->dec_stackp = sp->dec_stack;
    sp->lzw_oldcode = -1;
    sp->dec_firstchar = -1;

    if (tif->tif_rawdata[0] == 0 && (tif->tif_rawdata[1] & 0x1)) {
        if ((sp->lzw_flags & LZW_COMPAT) == 0) {
            TIFFWarning(tif->tif_name, "Old-style LZW codes, convert file");
        }
        sp->lzw_flags |= LZW_COMPAT;
    } else {
        sp->lzw_flags &= ~LZW_COMPAT;
    }
    SetMaxCode(sp, MAXCODE(BITS_MIN));
    return (1);
}

static LZWDecode(tif, op0, occ0, s) TIFF *tif;
u_char *op0;
u_int s;
{
    register char *op = (char *)op0;
    register int occ = occ0;
    register LZWState *sp = (LZWState *)tif->tif_data;
    register int code;
    register u_char *stackp;
    int firstchar, oldcode, incode;

    stackp = sp->dec_stackp;

    if (sp->lzw_flags & LZW_RESTART) {
        do {
            if (--occ < 0) {
                sp->dec_stackp = stackp;
                return (1);
            }
            *op++ = *--stackp;
        } while (stackp > sp->dec_stack);
        sp->lzw_flags &= ~LZW_RESTART;
    }
    oldcode = sp->lzw_oldcode;
    firstchar = sp->dec_firstchar;
    while (occ > 0 && (code = GetNextCode(tif)) != CODE_EOI) {
        if (code == CODE_CLEAR) {
            bzero(sp->dec_prefix, sizeof(sp->dec_prefix));
            sp->lzw_free_ent = CODE_FIRST;
            sp->lzw_nbits = BITS_MIN;
            SetMaxCode(sp, MAXCODE(BITS_MIN));
            if ((code = GetNextCode(tif)) == CODE_EOI) {
                break;
            }
            *op++ = code, occ--;
            oldcode = firstchar = code;
            continue;
        }
        incode = code;

        if (code >= sp->lzw_free_ent) {
            *stackp++ = firstchar;
            code = oldcode;
        }

        for (; code >= 256; code = sp->dec_prefix[code]) {
            *stackp++ = sp->dec_suffix[code];
        }
        *stackp++ = firstchar = sp->dec_suffix[code];
        do {
            if (--occ < 0) {
                sp->lzw_flags |= LZW_RESTART;
                break;
            }
            *op++ = *--stackp;
        } while (stackp > sp->dec_stack);

        if ((code = sp->lzw_free_ent) < CODE_MAX) {
            sp->dec_prefix[code] = (u_short)oldcode;
            sp->dec_suffix[code] = firstchar;
            sp->lzw_free_ent++;

            if (sp->lzw_free_ent > sp->lzw_maxcode) {
                sp->lzw_nbits++;
                if (sp->lzw_nbits > BITS_MAX) {
                    sp->lzw_nbits = BITS_MAX;
                }
                SetMaxCode(sp, MAXCODE(sp->lzw_nbits));
            }
        }
        oldcode = incode;
    }
    sp->dec_stackp = stackp;
    sp->lzw_oldcode = oldcode;
    sp->dec_firstchar = firstchar;
    if (occ > 0) {
        TIFFError(tif->tif_name, "LZWDecode: Not enough data at scanline %d (short %d bytes)", tif->tif_row, occ);
        return (0);
    }
    return (1);
}

static LZWDecodePredRow(tif, op0, occ0, s) TIFF *tif;
u_char *op0;
u_int s;
{
    LZWState *sp = (LZWState *)tif->tif_data;

    if (LZWDecode(tif, op0, occ0, s)) {
        (*sp->lzw_hordiff)((char *)op0, occ0, sp->lzw_stride);
        return (1);
    } else {
        return (0);
    }
}

static LZWDecodePredTile(tif, op0, occ0, s) TIFF *tif;
u_char *op0;
u_int s;
{
    LZWState *sp = (LZWState *)tif->tif_data;

    if (!LZWDecode(tif, op0, occ0, s)) {
        return (0);
    }
    while (occ0 > 0) {
        (*sp->lzw_hordiff)((char *)op0, sp->lzw_rowsize, sp->lzw_stride);
        occ0 -= sp->lzw_rowsize;
        op0 += sp->lzw_rowsize;
    }
    return (1);
}

static GetNextCode(tif) TIFF *tif;
{
    register LZWState *sp = (LZWState *)tif->tif_data;
    register int code, bits;
    register long r_off;
    register u_char *bp;

    if (sp->lzw_bitoff > sp->lzw_bitsize) {
        TIFFWarning(tif->tif_name, "LZWDecode: Strip %d not terminated with EOI code", tif->tif_curstrip);
        return (CODE_EOI);
    }
    r_off = sp->lzw_bitoff;
    bits = sp->lzw_nbits;

    bp = (u_char *)tif->tif_rawdata + (r_off >> 3);
    r_off &= 7;
    if (sp->lzw_flags & LZW_COMPAT) {

        code = (*bp++ >> r_off);
        r_off = 8 - r_off;
        bits -= r_off;

        if (bits >= 8) {
            code |= *bp++ << r_off;
            r_off += 8;
            bits -= 8;
        }

        code |= (*bp & rmask[bits]) << r_off;
    } else {
        r_off = 8 - r_off;
        code = *bp++ & rmask[r_off];
        bits -= r_off;
        if (bits >= 8) {
            code = (code << 8) | *bp++;
            bits -= 8;
        }

        code = (code << bits) | (((unsigned)(*bp & lmask[bits])) >> (8 - bits));
    }
    sp->lzw_bitoff += sp->lzw_nbits;
    return (code);
}

static void DECLARE3(horizontalDifference8, register char *, cp, register int, cc, register int, stride) {
    if (cc > stride) {
        cc -= stride;
        cp += cc - 1;
        do {
            REPEAT4(stride, cp[stride] -= cp[0]; cp--)
            cc -= stride;
        } while (cc > 0);
    }
}

static void DECLARE3(horizontalDifference16, char *, cp, int, cc, register int, stride) {
    register short *wp = (short *)cp;
    register int wc = cc / 2;

    if (wc > stride) {
        wc -= stride;
        wp += wc - 1;
        do {
            REPEAT4(stride, wp[stride] -= wp[0]; wp--)
            wc -= stride;
        } while (wc > 0);
    }
}

static LZWPreEncode(tif) TIFF *tif;
{
    register LZWState *sp = (LZWState *)tif->tif_data;

    if (sp == NULL) {
        tif->tif_data = malloc(sizeof(LZWState));
        if (tif->tif_data == NULL) {
            TIFFError("LZWPreEncode", "No space for LZW state block");
            return (0);
        }
        sp = (LZWState *)tif->tif_data;
        sp->lzw_flags = 0;
        sp->lzw_hordiff = 0;
        if (!LZWCheckPredictor(tif, sp, horizontalDifference8, horizontalDifference16)) {
            return (0);
        }
        if (sp->lzw_hordiff) {
            tif->tif_encoderow = LZWEncodePredRow;
            tif->tif_encodestrip = LZWEncodePredTile;
            tif->tif_encodetile = LZWEncodePredTile;
        }
    }
    sp->enc_checkpoint = CHECK_GAP;
    SetMaxCode(sp, MAXCODE(sp->lzw_nbits = BITS_MIN) + 1);
    cl_hash(sp);
    sp->lzw_bitoff = 0;
    sp->lzw_bitsize = (tif->tif_rawdatasize << 3) - (BITS_MAX - 1);
    sp->lzw_oldcode = -1;
    return (1);
}

static LZWEncode(tif, bp, cc, s) TIFF *tif;
u_char *bp;
int cc;
u_int s;
{
    static char module[] = "LZWEncode";
    register LZWState *sp;
    register long fcode;
    register int h, c, ent, disp;

    if ((sp = (LZWState *)tif->tif_data) == NULL) {
        return (0);
    }
    ent = sp->lzw_oldcode;
    if (ent == -1 && cc > 0) {
        PutNextCode(tif, CODE_CLEAR);
        ent = *bp++;
        cc--;
        sp->enc_incount++;
    }
    while (cc > 0) {
        c = *bp++;
        cc--;
        sp->enc_incount++;
        fcode = ((long)c << BITS_MAX) + ent;
        h = (c << HSHIFT) ^ ent;
        if (sp->enc_htab[h] == fcode) {
            ent = sp->enc_codetab[h];
            continue;
        }
        if (sp->enc_htab[h] >= 0) {

            disp = HSIZE - h;
            if (h == 0) {
                disp = 1;
            }
            do {
                if ((h -= disp) < 0) {
                    h += HSIZE;
                }
                if (sp->enc_htab[h] == fcode) {
                    ent = sp->enc_codetab[h];
                    goto hit;
                }
            } while (sp->enc_htab[h] >= 0);
        }

        PutNextCode(tif, ent);
        ent = c;
        sp->enc_codetab[h] = sp->lzw_free_ent++;
        sp->enc_htab[h] = fcode;
        if (sp->lzw_free_ent == CODE_MAX - 1) {

            cl_hash(sp);
            PutNextCode(tif, CODE_CLEAR);
            SetMaxCode(sp, MAXCODE(sp->lzw_nbits = BITS_MIN) + 1);
        } else {

            if (sp->lzw_free_ent > sp->lzw_maxcode) {
                sp->lzw_nbits++;
                assert(sp->lzw_nbits <= BITS_MAX);
                SetMaxCode(sp, MAXCODE(sp->lzw_nbits) + 1);
            } else if (sp->enc_incount >= sp->enc_checkpoint) {
                cl_block(tif);
            }
        }
    hit:;
    }
    sp->lzw_oldcode = ent;
    return (1);
}

static LZWEncodePredRow(tif, bp, cc, s) TIFF *tif;
u_char *bp;
int cc;
u_int s;
{
    LZWState *sp = (LZWState *)tif->tif_data;

    (*sp->lzw_hordiff)((char *)bp, cc, sp->lzw_stride);
    return (LZWEncode(tif, bp, cc, s));
}

static LZWEncodePredTile(tif, bp0, cc0, s) TIFF *tif;
u_char *bp0;
int cc0;
u_int s;
{
    LZWState *sp = (LZWState *)tif->tif_data;
    int cc = cc0;
    u_char *bp = bp0;

    while (cc > 0) {
        (*sp->lzw_hordiff)((char *)bp, sp->lzw_rowsize, sp->lzw_stride);
        cc -= sp->lzw_rowsize;
        bp += sp->lzw_rowsize;
    }
    return (LZWEncode(tif, bp0, cc0, s));
}

static LZWPostEncode(tif) TIFF *tif;
{
    LZWState *sp = (LZWState *)tif->tif_data;

    if (sp->lzw_oldcode != -1) {
        PutNextCode(tif, sp->lzw_oldcode);
        sp->lzw_oldcode = -1;
    }
    PutNextCode(tif, CODE_EOI);
    return (1);
}

static void PutNextCode(tif, c) TIFF *tif;
int c;
{
    register LZWState *sp = (LZWState *)tif->tif_data;
    register long r_off;
    register int bits, code = c;
    register char *bp;

    r_off = sp->lzw_bitoff;
    bits = sp->lzw_nbits;

    if (r_off + bits > sp->lzw_bitsize) {

        if (r_off & 7) {
            tif->tif_rawcc = r_off >> 3;
            bp = tif->tif_rawdata + tif->tif_rawcc;
            (void)TIFFFlushData1(tif);
            tif->tif_rawdata[0] = *bp;
        } else {

            (void)TIFFFlushData1(tif);
        }
        bp = tif->tif_rawdata;
        sp->lzw_bitoff = (r_off &= 7);
    } else {

        bp = tif->tif_rawdata + (r_off >> 3);
        r_off &= 7;
    }

    bits -= (8 - r_off);
    *bp = (*bp & lmask[r_off]) | (code >> bits);
    bp++;
    if (bits >= 8) {
        bits -= 8;
        *bp++ = code >> bits;
    }
    if (bits) {
        *bp = (code & rmask[bits]) << (8 - bits);
    }

    sp->enc_outcount += sp->lzw_nbits;
    sp->lzw_bitoff += sp->lzw_nbits;
    tif->tif_rawcc = (sp->lzw_bitoff + 7) >> 3;
}

static void cl_block(tif) TIFF *tif;
{
    register LZWState *sp = (LZWState *)tif->tif_data;
    register long rat;

    sp->enc_checkpoint = sp->enc_incount + CHECK_GAP;
    if (sp->enc_incount > 0x007fffff) {
        rat = sp->enc_outcount >> 8;
        rat = (rat == 0 ? 0x7fffffff : sp->enc_incount / rat);
    } else {
        rat = (sp->enc_incount << 8) / sp->enc_outcount;
    }
    if (rat <= sp->enc_ratio) {
        cl_hash(sp);
        PutNextCode(tif, CODE_CLEAR);
        SetMaxCode(sp, MAXCODE(sp->lzw_nbits = BITS_MIN) + 1);
    } else {
        sp->enc_ratio = rat;
    }
}

static void cl_hash(sp) LZWState *sp;
{
    register long *htab_p = sp->enc_htab + HSIZE;
    register long i, m1 = -1;

    i = HSIZE - 16;
    do {
        *(htab_p - 16) = m1;
        *(htab_p - 15) = m1;
        *(htab_p - 14) = m1;
        *(htab_p - 13) = m1;
        *(htab_p - 12) = m1;
        *(htab_p - 11) = m1;
        *(htab_p - 10) = m1;
        *(htab_p - 9) = m1;
        *(htab_p - 8) = m1;
        *(htab_p - 7) = m1;
        *(htab_p - 6) = m1;
        *(htab_p - 5) = m1;
        *(htab_p - 4) = m1;
        *(htab_p - 3) = m1;
        *(htab_p - 2) = m1;
        *(htab_p - 1) = m1;
        htab_p -= 16;
    } while ((i -= 16) >= 0);
    for (i += 16; i > 0; i--) {
        *--htab_p = m1;
    }

    sp->enc_ratio = 0;
    sp->enc_incount = 0;
    sp->enc_outcount = 0;
    sp->lzw_free_ent = CODE_FIRST;
}

static LZWCleanup(tif) TIFF *tif;
{
    if (tif->tif_data) {
        free(tif->tif_data);
        tif->tif_data = NULL;
    }
}

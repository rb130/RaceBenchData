
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define XDECR 1
#define YDECR 2
#define YMAJOR 4

#define PIXSTANDARD (YMAJOR | YDECR)
#define PIXSTDFMT "-Y %d +X %d\n"

typedef struct {
    int rt;
    int xr, yr;
} RESOLU;

#define scanlen(rs) ((rs)->rt & YMAJOR ? (rs)->xr : (rs)->yr)
#define numscans(rs) ((rs)->rt & YMAJOR ? (rs)->yr : (rs)->xr)

#define RESOLU_BUFLEN 32

#define fputsresolu(rs, fp) fputs(resolu2str(resolu_buf, rs), fp)
#define fgetsresolu(rs, fp) str2resolu(rs, fgets(resolu_buf, RESOLU_BUFLEN, fp))

#define fprtresolu(sl, ns, fp) fprintf(fp, PIXSTDFMT, ns, sl)
#define fscnresolu(sl, ns, fp) (fscanf(fp, PIXSTDFMT, ns, sl) == 2)

typedef int gethfunc(char *s, void *p);

#define RED 0
#define GRN 1
#define BLU 2
#define CIEX 0
#define CIEY 1
#define CIEZ 2
#define EXP 3
#define COLXS 128
#define WHT 3

#undef BYTE
#define BYTE unsigned char

typedef BYTE COLR[4];

typedef float COLORV;
typedef COLORV COLOR[3];

typedef float RGBPRIMS[4][2];
typedef float (*RGBPRIMP)[2];

typedef float COLORMAT[3][3];

#define copycolr(c1, c2) (c1[0] = c2[0], c1[1] = c2[1], c1[2] = c2[2], c1[3] = c2[3])

#define colval(col, pri) ((col)[pri])

#define setcolor(col, r, g, b) ((col)[RED] = (r), (col)[GRN] = (g), (col)[BLU] = (b))

#define copycolor(c1, c2) ((c1)[0] = (c2)[0], (c1)[1] = (c2)[1], (c1)[2] = (c2)[2])

#define scalecolor(col, sf) ((col)[0] *= (sf), (col)[1] *= (sf), (col)[2] *= (sf))

#define addcolor(c1, c2) ((c1)[0] += (c2)[0], (c1)[1] += (c2)[1], (c1)[2] += (c2)[2])

#define multcolor(c1, c2) ((c1)[0] *= (c2)[0], (c1)[1] *= (c2)[1], (c1)[2] *= (c2)[2])

#ifdef NTSC
#define CIE_x_r 0.670
#define CIE_y_r 0.330
#define CIE_x_g 0.210
#define CIE_y_g 0.710
#define CIE_x_b 0.140
#define CIE_y_b 0.080
#define CIE_x_w 0.3333
#define CIE_y_w 0.3333
#else
#define CIE_x_r 0.640
#define CIE_y_r 0.330
#define CIE_x_g 0.290
#define CIE_y_g 0.600
#define CIE_x_b 0.150
#define CIE_y_b 0.060
#define CIE_x_w 0.3333
#define CIE_y_w 0.3333
#endif

#define STDPRIMS                                                      \
    {                                                                 \
        {CIE_x_r, CIE_y_r}, {CIE_x_g, CIE_y_g}, {CIE_x_b, CIE_y_b}, { \
            CIE_x_w, CIE_y_w                                          \
        }                                                             \
    }

#define CIE_D (CIE_x_r * (CIE_y_g - CIE_y_b) + CIE_x_g * (CIE_y_b - CIE_y_r) + CIE_x_b * (CIE_y_r - CIE_y_g))
#define CIE_C_rD ((1. / CIE_y_w) * (CIE_x_w * (CIE_y_g - CIE_y_b) - CIE_y_w * (CIE_x_g - CIE_x_b) + CIE_x_g * CIE_y_b - CIE_x_b * CIE_y_g))
#define CIE_C_gD ((1. / CIE_y_w) * (CIE_x_w * (CIE_y_b - CIE_y_r) - CIE_y_w * (CIE_x_b - CIE_x_r) - CIE_x_r * CIE_y_b + CIE_x_b * CIE_y_r))
#define CIE_C_bD ((1. / CIE_y_w) * (CIE_x_w * (CIE_y_r - CIE_y_g) - CIE_y_w * (CIE_x_r - CIE_x_g) + CIE_x_r * CIE_y_g - CIE_x_g * CIE_y_r))

#define CIE_rf (CIE_y_r * CIE_C_rD / CIE_D)
#define CIE_gf (CIE_y_g * CIE_C_gD / CIE_D)
#define CIE_bf (CIE_y_b * CIE_C_bD / CIE_D)

#define bright(col) (CIE_rf * (col)[RED] + CIE_gf * (col)[GRN] + CIE_bf * (col)[BLU])
#define normbright(c) (((long)(CIE_rf * 256. + .5) * (c)[RED] + (long)(CIE_gf * 256. + .5) * (c)[GRN] + (long)(CIE_bf * 256. + .5) * (c)[BLU]) >> 8)

#define MAXEFFICACY 683.
#define WHTEFFICACY 179.
#define D65EFFICACY 203.
#define INCEFFICACY 160.
#define SUNEFFICACY 208.
#define SKYEFFICACY D65EFFICACY
#define DAYEFFICACY D65EFFICACY

#define luminance(col) (WHTEFFICACY * bright(col))

#define intens(col) ((col)[0] > (col)[1] ? (col)[0] > (col)[2] ? (col)[0] : (col)[2] : (col)[1] > (col)[2] ? (col)[1] : (col)[2])

#define colrval(c, p) ((c)[EXP] ? ldexp((c)[p] + .5, (int)(c)[EXP] - (COLXS + 8)) : 0.)

#define WHTCOLOR \
    { 1.0, 1.0, 1.0 }
#define BLKCOLOR \
    { 0.0, 0.0, 0.0 }
#define WHTCOLR \
    { 128, 128, 128, COLXS + 1 }
#define BLKCOLR \
    { 0, 0, 0, 0 }

#define COLRFMT "32-bit_rle_rgbe"
#define CIEFMT "32-bit_rle_xyze"
#define PICFMT "32-bit_rle_???e"
#define LPICFMT 15

#define EXPOSSTR "EXPOSURE="
#define LEXPOSSTR 9
#define isexpos(hl) (!strncmp(hl, EXPOSSTR, LEXPOSSTR))
#define exposval(hl) atof((hl) + LEXPOSSTR)
#define fputexpos(ex, fp) fprintf(fp, "%s%e\n", EXPOSSTR, ex)

#define ASPECTSTR "PIXASPECT="
#define LASPECTSTR 10
#define isaspect(hl) (!strncmp(hl, ASPECTSTR, LASPECTSTR))
#define aspectval(hl) atof((hl) + LASPECTSTR)
#define fputaspect(pa, fp) fprintf(fp, "%s%f\n", ASPECTSTR, pa)

#define PRIMARYSTR "PRIMARIES="
#define LPRIMARYSTR 10
#define isprims(hl) (!strncmp(hl, PRIMARYSTR, LPRIMARYSTR))
#define primsval(p, hl) sscanf(hl + LPRIMARYSTR, "%f %f %f %f %f %f %f %f", &(p)[RED][CIEX], &(p)[RED][CIEY], &(p)[GRN][CIEX], &(p)[GRN][CIEY], &(p)[BLU][CIEX], &(p)[BLU][CIEY], &(p)[WHT][CIEX], &(p)[WHT][CIEY])
#define fputprims(p, fp) fprintf(fp, "%s %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n", PRIMARYSTR, (p)[RED][CIEX], (p)[RED][CIEY], (p)[GRN][CIEX], (p)[GRN][CIEY], (p)[BLU][CIEX], (p)[BLU][CIEY], (p)[WHT][CIEX], (p)[WHT][CIEY])

#define COLCORSTR "COLORCORR="
#define LCOLCORSTR 10
#define iscolcor(hl) (!strncmp(hl, COLCORSTR, LCOLCORSTR))
#define colcorval(cc, hl) sscanf(hl + LCOLCORSTR, "%f %f %f", &(cc)[RED], &(cc)[GRN], &(cc)[BLU])
#define fputcolcor(cc, fp) fprintf(fp, "%s %f %f %f\n", COLCORSTR, (cc)[RED], (cc)[GRN], (cc)[BLU])

#define CGAMUT_LOWER 01
#define CGAMUT_UPPER 02
#define CGAMUT (CGAMUT_LOWER | CGAMUT_UPPER)

#define rgb_cie(xyz, rgb) colortrans(xyz, rgb2xyzmat, rgb)

#define cpcolormat(md, ms) memcpy((void *)md, (void *)ms, sizeof(COLORMAT))

#define MAXLINE 512

char HDRSTR[] = "#?";

char FMTSTR[] = "FORMAT=";

char TMSTR[] = "CAPDATE=";

static gethfunc mycheck;

static int formatval(register char *r, register char *s) {
    register char *cp = FMTSTR;

    while (*cp) {
        if (*cp++ != *s++) {
            return (0);
        }
    }
    while (isspace(*s)) {
        s++;
    }
    if (!*s) {
        return (0);
    }
    if (r == NULL) {
        return (1);
    }
    do {
        *r++ = *s++;
    } while (*s && !isspace(*s));
    *r = '\0';
    return (1);
}

static int isformat(char *s) {
    return (formatval(NULL, s));
}

static int getheader(FILE *fp, gethfunc *f, void *p) {
    char buf[MAXLINE];

    for (;;) {
        buf[MAXLINE - 2] = '\n';
        if (fgets(buf, MAXLINE, fp) == NULL) {
            return (-1);
        }
        if (buf[0] == '\n') {
            return (0);
        }
#ifdef MSDOS
        if (buf[0] == '\r' && buf[1] == '\n')
            return (0);
#endif
        if (buf[MAXLINE - 2] != '\n') {
            ungetc(buf[MAXLINE - 2], fp);
            buf[MAXLINE - 2] = '\0';
        }
        if (f != NULL && (*f)(buf, p) < 0) {
            return (-1);
        }
    }
}

struct check {
    FILE *fp;
    char fs[64];
};

static int mycheck(char *s, void *cp) {
    if (!formatval(((struct check *)cp)->fs, s) && ((struct check *)cp)->fp != NULL) {
        fputs(s, ((struct check *)cp)->fp);
    }
    return (0);
}

static int globmatch(register char *p, register char *s) {
    int setmatch;

    do {
        switch (*p) {
        case '?':
            if (!*s++) {
                return (0);
            }
            break;
        case '*':
            while (p[1] == '*') {
                p++;
            }
            do {
                if ((p[1] == '?' || p[1] == *s) && globmatch(p + 1, s)) {
                    return (1);
                }
            } while (*s++);
            return (0);
        case '[':
            setmatch = *s == *++p;
            if (!*p) {
                return (0);
            }
            while (*++p != ']') {
                if (!*p) {
                    return (0);
                }
                if (*p == '-') {
                    setmatch += p[-1] <= *s && *s <= p[1];
                    if (!*++p) {
                        break;
                    }
                } else {
                    setmatch += *p == *s;
                }
            }
            if (!setmatch) {
                return (0);
            }
            s++;
            break;
        case '\\':
            p++;

        default:
            if (*p != *s) {
                return (0);
            }
            s++;
            break;
        }
    } while (*p++);
    return (1);
}

static int checkheader(FILE *fin, char *fmt, FILE *fout) {
    struct check cdat;
    register char *cp;

    cdat.fp = fout;
    cdat.fs[0] = '\0';
    if (getheader(fin, mycheck, &cdat) < 0) {
        return (-1);
    }
    if (!cdat.fs[0]) {
        return (0);
    }
    for (cp = fmt; *cp; cp++) {
        if ((*cp == '?') | (*cp == '*')) {
            if (globmatch(fmt, cdat.fs)) {
                strcpy(fmt, cdat.fs);
                return (1);
            } else {
                return (-1);
            }
        }
    }
    return (strcmp(fmt, cdat.fs) ? -1 : 1);
}

static char resolu_buf[RESOLU_BUFLEN];

static int str2resolu(rp, buf) register RESOLU *rp;
char *buf;
{
    register char *xndx, *yndx;
    register char *cp;

    if (buf == NULL) {
        return (0);
    }
    xndx = yndx = NULL;
    for (cp = buf; *cp; cp++) {
        if (*cp == 'X') {
            xndx = cp;
        } else if (*cp == 'Y') {
            yndx = cp;
        }
    }
    if (xndx == NULL || yndx == NULL) {
        return (0);
    }
    rp->rt = 0;
    if (xndx > yndx) {
        rp->rt |= YMAJOR;
    }
    if (xndx[-1] == '-') {
        rp->rt |= XDECR;
    }
    if (yndx[-1] == '-') {
        rp->rt |= YDECR;
    }
    if ((rp->xr = atoi(xndx + 1)) <= 0) {
        return (0);
    }
    if ((rp->yr = atoi(yndx + 1)) <= 0) {
        return (0);
    }
    return (1);
}

#ifdef getc_unlocked
#undef getc
#undef putc
#define getc getc_unlocked
#define putc putc_unlocked
#endif

#define MINELEN 8
#define MAXELEN 0x7fff
#define MINRUN 4

static int oldreadcolrs(scanline, len, fp) register COLR *scanline;
int len;
register FILE *fp;
{
    int rshift;
    register int i;

    rshift = 0;

    while (len > 0) {
        scanline[0][RED] = getc(fp);
        scanline[0][GRN] = getc(fp);
        scanline[0][BLU] = getc(fp);
        scanline[0][EXP] = getc(fp);
        if (feof(fp) || ferror(fp)) {
            return (-1);
        }
        if (scanline[0][RED] == 1 && scanline[0][GRN] == 1 && scanline[0][BLU] == 1) {
            for (i = scanline[0][EXP] << rshift; i > 0; i--) {
                copycolr(scanline[0], scanline[-1]);
                scanline++;
                len--;
            }
            rshift += 8;
        } else {
            scanline++;
            len--;
            rshift = 0;
        }
    }
    return (0);
}

static int freadcolrs(scanline, len, fp) register COLR *scanline;
int len;
register FILE *fp;
{
    register int i, j;
    int code, val;

    if ((len < MINELEN) | (len > MAXELEN)) {
        return (oldreadcolrs(scanline, len, fp));
    }
    if ((i = getc(fp)) == EOF) {
        return (-1);
    }
    if (i != 2) {
        ungetc(i, fp);
        return (oldreadcolrs(scanline, len, fp));
    }
    scanline[0][GRN] = getc(fp);
    scanline[0][BLU] = getc(fp);
    if ((i = getc(fp)) == EOF) {
        return (-1);
    }
    if (scanline[0][GRN] != 2 || scanline[0][BLU] & 128) {
        scanline[0][RED] = 2;
        scanline[0][EXP] = i;
        return (oldreadcolrs(scanline + 1, len - 1, fp));
    }
    if ((scanline[0][BLU] << 8 | i) != len) {
        return (-1);
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < len;) {
            if ((code = getc(fp)) == EOF) {
                return (-1);
            }
            if (code > 128) {
                code &= 127;
                if ((val = getc(fp)) == EOF) {
                    return -1;
                }
                if (j + code > len) {
                    return -1;
                }
                while (code--) {
                    scanline[j++][i] = val;
                }
            } else {
                if (j + code > len) {
                    return -1;
                }
                while (code--) {
                    if ((val = getc(fp)) == EOF) {
                        return -1;
                    }
                    scanline[j++][i] = val;
                }
            }
        }
    }
    return (0);
}

static void fputformat(char *s, FILE *fp) {
    fputs(FMTSTR, fp);
    fputs(s, fp);
    putc('\n', fp);
}

char *resolu2str(buf, rp) char *buf;
register RESOLU *rp;
{
    if (rp->rt & YMAJOR) {
        sprintf(buf, "%cY %d %cX %d\n", rp->rt & YDECR ? '-' : '+', rp->yr, rp->rt & XDECR ? '-' : '+', rp->xr);
    } else {
        sprintf(buf, "%cX %d %cY %d\n", rp->rt & XDECR ? '-' : '+', rp->xr, rp->rt & YDECR ? '-' : '+', rp->yr);
    }
    return (buf);
}

static int fwritecolrs(scanline, len, fp) register COLR *scanline;
int len;
register FILE *fp;
{
    register int i, j, beg, cnt = 1;
    int c2;

    if ((len < MINELEN) | (len > MAXELEN)) {
        return (fwrite((char *)scanline, sizeof(COLR), len, fp) - len);
    }

    putc(2, fp);
    putc(2, fp);
    putc(len >> 8, fp);
    putc(len & 255, fp);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < len; j += cnt) {
            for (beg = j; beg < len; beg += cnt) {
                for (cnt = 1; cnt < 127 && beg + cnt < len && scanline[beg + cnt][i] == scanline[beg][i]; cnt++) {
                    ;
                }
                if (cnt >= MINRUN) {
                    break;
                }
            }
            if (beg - j > 1 && beg - j < MINRUN) {
                c2 = j + 1;
                while (scanline[c2++][i] == scanline[j][i]) {
                    if (c2 == beg) {
                        putc(128 + beg - j, fp);
                        putc(scanline[j][i], fp);
                        j = beg;
                        break;
                    }
                }
            }
            while (j < beg) {
                if ((c2 = beg - j) > 128) {
                    c2 = 128;
                }
                putc(c2, fp);
                while (c2--) {
                    putc(scanline[j++][i], fp);
                }
            }
            if (cnt >= MINRUN) {
                putc(128 + cnt, fp);
                putc(scanline[beg][i], fp);
            } else {
                cnt = 0;
            }
        }
    }
    return (ferror(fp) ? -1 : 0);
}

typedef struct {
    char *filename;
    IMAGE *out;

    FILE *fin;
    char format[256];
    double expos;
    COLOR colcor;
    double aspect;
    RGBPRIMS prims;
    RESOLU rs;

    COLR *buf;
} Read;

static int israd(const char *filename) {
    FILE *fin;
    char format[256];
    int result;

#ifdef DEBUG
    printf("israd: \"%s\"\n", filename);
#endif

    if (!(fin = im__file_open_read(filename, NULL))) {
        return (0);
    }
    strcpy(format, PICFMT);
    result = checkheader(fin, format, NULL);
    fclose(fin);

    return (result == 1);
}

static void read_destroy(Read *read) {
    IM_FREE(read->filename);
    IM_FREEF(fclose, read->fin);
    IM_FREE(read->buf);

    im_free(read);
}

static Read *read_new(const char *filename, IMAGE *out) {
    Read *read;
    int i;

    if (!(read = IM_NEW(NULL, Read))) {
        return (NULL);
    }

    read->filename = im_strdup(NULL, filename);
    read->out = out;
    read->fin = NULL;
    strcpy(read->format, COLRFMT);
    read->expos = 1.0;
    for (i = 0; i < 3; i++) {
        read->colcor[i] = 1.0;
    }
    read->aspect = 1.0;
    read->prims[0][0] = CIE_x_r;
    read->prims[0][1] = CIE_y_r;
    read->prims[1][0] = CIE_x_g;
    read->prims[1][1] = CIE_y_g;
    read->prims[2][0] = CIE_x_b;
    read->prims[2][1] = CIE_y_b;
    read->prims[3][0] = CIE_x_w;
    read->prims[3][1] = CIE_y_w;
    read->buf = NULL;

    if (!(read->fin = im__file_open_read(filename, NULL))) {
        read_destroy(read);
        return (NULL);
    }

    return (read);
}

static int rad2vips_process_line(char *line, Read *read) {
    if (isformat(line)) {
        if (formatval(line, read->format)) {
            return (-1);
        }
    } else if (isexpos(line)) {
        read->expos *= exposval(line);
    } else if (iscolcor(line)) {
        COLOR cc;
        int i;

        colcorval(cc, line);
        for (i = 0; i < 3; i++) {
            read->colcor[i] *= cc[i];
        }
    } else if (isaspect(line)) {
        read->aspect *= aspectval(line);
    } else if (isprims(line)) {
        primsval(read->prims, line);
    }

    return (0);
}

static const char *prims_name[4][2] = {{"rad-prims-rx", "rad-prims-ry"}, {"rad-prims-gx", "rad-prims-gy"}, {"rad-prims-bx", "rad-prims-by"}, {"rad-prims-wx", "rad-prims-wy"}};

static const char *colcor_name[3] = {"rad-colcor-r", "rad-colcor-g", "rad-colcor-b"};

static int rad2vips_get_header(Read *read, FILE *fin, IMAGE *out) {
    int i, j;

    if (getheader(fin, (gethfunc *)rad2vips_process_line, read) || !fgetsresolu(&read->rs, fin)) {
        im_error("im_rad2vips", "%s", _("error reading radiance header"));
        return (-1);
    }
    out->Xsize = scanlen(&read->rs);
    out->Ysize = numscans(&read->rs);

    out->Bands = 4;
    out->BandFmt = IM_BANDFMT_UCHAR;

    out->Coding = IM_CODING_RAD;
    out->Xres = 1.0;
    out->Yres = read->aspect;
    out->Xoffset = 0.0;
    out->Yoffset = 0.0;

    if (im_meta_set_string(out, "rad-format", read->format)) {
        return (-1);
    }

    if (strcmp(read->format, COLRFMT) == 0) {
        out->Type = IM_TYPE_RGB;
    } else if (strcmp(read->format, CIEFMT) == 0) {
        out->Type = IM_TYPE_XYZ;
    } else {
        out->Type = IM_TYPE_MULTIBAND;
    }

    if (im_meta_set_double(out, "rad-expos", read->expos)) {
        return (-1);
    }

    for (i = 0; i < 3; i++) {
        if (im_meta_set_double(out, colcor_name[i], read->colcor[i])) {
            return (-1);
        }
    }

    if (im_meta_set_double(out, "rad-aspect", read->aspect)) {
        return (-1);
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            if (im_meta_set_double(out, prims_name[i][j], read->prims[i][j])) {
                return (-1);
            }
        }
    }

    return (0);
}

static int rad2vips_header(const char *filename, IMAGE *out) {
    Read *read;

#ifdef DEBUG
    printf("rad2vips_header: reading \"%s\"\n", filename);
#endif

    if (!(read = read_new(filename, out))) {
        return (-1);
    }
    if (rad2vips_get_header(read, read->fin, read->out)) {
        read_destroy(read);
        return (-1);
    }
    read_destroy(read);

    return (0);
}

static int rad2vips_get_data(Read *read, FILE *fin, IMAGE *im) {
    int y;

#ifdef DEBUG
    printf("rad2vips_get_data\n");
#endif

    if (im_outcheck(im) || im_setupout(im)) {
        return (-1);
    }
    if (!(read->buf = IM_ARRAY(NULL, im->Xsize, COLR))) {
        return (-1);
    }

    for (y = 0; y < im->Ysize; y++) {
        if (freadcolrs(read->buf, im->Xsize, fin)) {
            im_error("im_rad2vips", "%s", _("read error"));
            return (-1);
        }
        if (im_writeline(y, im, (void *)read->buf)) {
            return (-1);
        }
    }

    return (0);
}

int im_rad2vips(const char *filename, IMAGE *out) {
    Read *read;

#ifdef DEBUG
    printf("rad2vips: reading \"%s\"\n", filename);
#endif

    if (!(read = read_new(filename, out))) {
        return (-1);
    }
    if (rad2vips_get_header(read, read->fin, read->out) || rad2vips_get_data(read, read->fin, read->out)) {
        read_destroy(read);
        return (-1);
    }
    read_destroy(read);

    return (0);
}

typedef struct {
    IMAGE *in;
    char *filename;

    FILE *fout;
    char format[256];
    double expos;
    COLOR colcor;
    double aspect;
    RGBPRIMS prims;
    RESOLU rs;
} Write;

static void write_destroy(Write *write) {
    IM_FREE(write->filename);
    IM_FREEF(fclose, write->fout);

    im_free(write);
}

static Write *write_new(IMAGE *in, const char *filename) {
    Write *write;
    int i;

    if (!(write = IM_NEW(NULL, Write))) {
        return (NULL);
    }

    write->in = in;
    write->filename = im_strdup(NULL, filename);
    write->fout = im__file_open_write(filename);
    strcpy(write->format, COLRFMT);
    write->expos = 1.0;
    for (i = 0; i < 3; i++) {
        write->colcor[i] = 1.0;
    }
    write->aspect = 1.0;
    write->prims[0][0] = CIE_x_r;
    write->prims[0][1] = CIE_y_r;
    write->prims[1][0] = CIE_x_g;
    write->prims[1][1] = CIE_y_g;
    write->prims[2][0] = CIE_x_b;
    write->prims[2][1] = CIE_y_b;
    write->prims[3][0] = CIE_x_w;
    write->prims[3][1] = CIE_y_w;

    if (!write->filename || !write->fout) {
        write_destroy(write);
        return (NULL);
    }

    return (write);
}

static int vips2rad_put_header(Write *write) {
    char *str;
    int i, j;
    double d;

    (void)im_meta_get_double(write->in, "rad-expos", &write->expos);
    (void)im_meta_get_double(write->in, "rad-aspect", &write->aspect);

    if (!im_meta_get_string(write->in, "rad-format", &str)) {
        im_strncpy(write->format, str, 256);
    }
    if (write->in->Type == IM_TYPE_RGB) {
        strcpy(write->format, COLRFMT);
    }
    if (write->in->Type == IM_TYPE_XYZ) {
        strcpy(write->format, CIEFMT);
    }

    for (i = 0; i < 3; i++) {
        if (!im_meta_get_double(write->in, colcor_name[i], &d)) {
            write->colcor[i] = d;
        }
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            if (!im_meta_get_double(write->in, prims_name[i][j], &d)) {
                write->prims[i][j] = d;
            }
        }
    }

    write->rs.rt = YDECR | YMAJOR;
    write->rs.xr = write->in->Xsize;
    write->rs.yr = write->in->Ysize;

    fprintf(write->fout, "#?RADIANCE\n");

    fputformat(write->format, write->fout);
    fputexpos(write->expos, write->fout);
    fputcolcor(write->colcor, write->fout);
    fprintf(write->fout, "SOFTWARE=vips %s\n", im_version_string());
    fputaspect(write->aspect, write->fout);
    fputprims(write->prims, write->fout);
    fputs("\n", write->fout);
    fputsresolu(&write->rs, write->fout);

    return (0);
}

static int vips2rad_put_data_block(REGION *region, Rect *area, void *a) {
    Write *write = (Write *)a;
    int i;

    for (i = 0; i < area->height; i++) {
        PEL *p = (PEL *)IM_REGION_ADDR(region, 0, area->top + i);

        if (fwritecolrs(p, area->width, write->fout)) {
            return (-1);
        }
    }

    return (0);
}

static int vips2rad_put_data(Write *write) {
    if (vips_sink_disc(write->in, vips2rad_put_data_block, write)) {
        return (-1);
    }

    return (0);
}

int im_vips2rad(IMAGE *in, const char *filename) {
    Write *write;

#ifdef DEBUG
    printf("im_vips2rad: writing \"%s\"\n", filename);
#endif

    if (in->BandFmt == IM_BANDFMT_FLOAT && in->Bands == 3 && in->Coding == IM_CODING_NONE) {
        IMAGE *t;

        if (!(t = im_open_local(in, "im_vips2rad", "p")) || im_float2rad(in, t)) {
            return (-1);
        }

        in = t;
    }

    if (im_pincheck(in) || im_check_coding_rad("im_vips2rad", in)) {
        return (-1);
    }
    if (!(write = write_new(in, filename))) {
        return (-1);
    }
    if (vips2rad_put_header(write) || vips2rad_put_data(write)) {
        write_destroy(write);
        return (-1);
    }
    write_destroy(write);

    return (0);
}

static const char *rad_suffs[] = {".hdr", NULL};

typedef VipsFormat VipsFormatRad;
typedef VipsFormatClass VipsFormatRadClass;

static void vips_format_rad_class_init(VipsFormatRadClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "rad";
    object_class->description = _("Radiance");

    format_class->is_a = israd;
    format_class->header = rad2vips_header;
    format_class->load = im_rad2vips;
    format_class->save = im_vips2rad;
    format_class->suffs = rad_suffs;
}

static void vips_format_rad_init(VipsFormatRad *object) {
}

G_DEFINE_TYPE(VipsFormatRad, vips_format_rad, VIPS_TYPE_FORMAT);

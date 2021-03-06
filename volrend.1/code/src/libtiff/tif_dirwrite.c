

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_dirwrite.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include "prototypes.h"

#if HAVE_IEEEFP
#define TIFFCvtNativeToIEEEFloat(tif, n, fp)
#endif

#if USE_PROTOTYPES
static TIFFWriteNormalTag(TIFF *, TIFFDirEntry *, TIFFFieldInfo *);
static TIFFSetupShortLong(TIFF *, u_short, TIFFDirEntry *, u_long);
static TIFFSetupShortPair(TIFF *, u_short, TIFFDirEntry *);
static TIFFWriteRational(TIFF *, TIFFDataType, u_short, TIFFDirEntry *, float);
static TIFFWritePerSampleShorts(TIFF *, u_short, TIFFDirEntry *);
static TIFFWriteShortTable(TIFF *, u_short, TIFFDirEntry *, int, u_short **);
static TIFFWriteShortArray(TIFF *, TIFFDataType, u_short, TIFFDirEntry *, int, u_short *);
static TIFFWriteLongArray(TIFF *, TIFFDataType, u_short, TIFFDirEntry *, int, u_long *);
static TIFFWriteRationalArray(TIFF *, TIFFDataType, u_short, TIFFDirEntry *, int, float *);
static TIFFWriteFloatArray(TIFF *, TIFFDataType, u_short, TIFFDirEntry *, int, float *);
static TIFFWriteString(TIFF *, u_short, TIFFDirEntry *, char *);
#ifdef JPEG_SUPPORT
static TIFFWriteJPEGQTables(TIFF *, TIFFDirEntry *);
static TIFFWriteJPEGCTables(TIFF *, u_short, TIFFDirEntry *, u_char **);
#endif
#ifdef COLORIMETRY_SUPPORT
static TIFFWriteTransferFunction(TIFF *, TIFFDirEntry *);
#endif
static TIFFWriteData(TIFF *, TIFFDirEntry *, char *);
static TIFFLinkDirectory(TIFF *);
#else
static TIFFWriteNormalTag();
static TIFFSetupShortLong();
static TIFFSetupShortPair();
static TIFFWriteRational();
static TIFFWritePerSampleShorts();
static TIFFWriteShortTable();
static TIFFWriteShortArray();
static TIFFWriteLongArray();
static TIFFWriteRationalArray();
static TIFFWriteFloatArray();
static TIFFWriteString();
#ifdef JPEG_SUPPORT
static TIFFWriteJPEGQTables();
static TIFFWriteJPEGCTables();
#endif
static TIFFWriteData();
static TIFFLinkDirectory();
#endif

#define WriteRationalPair(type, tag1, v1, tag2, v2)           \
    {                                                         \
        if (!TIFFWriteRational(tif, type, tag1, dir, v1))     \
            goto bad;                                         \
        if (!TIFFWriteRational(tif, type, tag2, dir + 1, v2)) \
            goto bad;                                         \
        dir++;                                                \
    }

static long dataoff;

TIFFWriteDirectory(tif) TIFF *tif;
{
    short dircount, tag;
    int nfields, dirsize;
    char *data;
    TIFFFieldInfo *fip;
    TIFFDirEntry *dir;
    TIFFDirectory *td;
    u_long b, fields[sizeof(td->td_fieldsset) / sizeof(u_long)];

    if (tif->tif_mode == O_RDONLY) {
        return (1);
    }

    if (tif->tif_flags & TIFF_POSTENCODE) {
        tif->tif_flags &= ~TIFF_POSTENCODE;
        if (tif->tif_postencode && !(*tif->tif_postencode)(tif)) {
            TIFFError(tif->tif_name, "Error post-encoding before directory write");
            return (0);
        }
    }
    if (tif->tif_close) {
        (*tif->tif_close)(tif);
    }
    if (tif->tif_cleanup) {
        (*tif->tif_cleanup)(tif);
    }

    if (tif->tif_rawcc > 0 && !TIFFFlushData1(tif)) {
        TIFFError(tif->tif_name, "Error flushing data before directory write");
        return (0);
    }
    if ((tif->tif_flags & TIFF_MYBUFFER) && tif->tif_rawdata) {
        free(tif->tif_rawdata);
        tif->tif_rawdata = NULL;
        tif->tif_rawcc = 0;
    }
    tif->tif_flags &= ~(TIFF_BEENWRITING | TIFF_BUFFERSETUP);

    td = &tif->tif_dir;

    nfields = 0;
    for (b = 0; b <= FIELD_LAST; b++) {
        if (TIFFFieldSet(tif, b)) {
            nfields += (b < FIELD_SUBFILETYPE ? 2 : 1);
        }
    }
    dirsize = nfields * sizeof(TIFFDirEntry);
    data = malloc(dirsize);
    if (data == NULL) {
        TIFFError(tif->tif_name, "Cannot write directory, out of space");
        return (0);
    }

    if (tif->tif_diroff == 0 && !TIFFLinkDirectory(tif)) {
        return (0);
    }
    dataoff = tif->tif_diroff + sizeof(short) + dirsize + sizeof(long);
    if (dataoff & 1) {
        dataoff++;
    }
    (void)lseek(tif->tif_fd, dataoff, L_SET);
    tif->tif_curdir++;
    dir = (TIFFDirEntry *)data;

    bcopy(td->td_fieldsset, fields, sizeof(fields));

    if (FieldSet(fields, FIELD_MATTEING) && !td->td_matteing) {
        ResetFieldBit(fields, FIELD_MATTEING);
        nfields--;
        dirsize -= sizeof(TIFFDirEntry);
    }

    for (fip = tiffFieldInfo; fip->field_tag; fip++) {
        if (fip->field_bit == FIELD_IGNORE || !FieldSet(fields, fip->field_bit)) {
            continue;
        }
        switch (fip->field_bit) {
        case FIELD_STRIPOFFSETS:

            tag = isTiled(tif) ? TIFFTAG_TILEOFFSETS : TIFFTAG_STRIPOFFSETS;
            if (tag != fip->field_tag) {
                continue;
            }
            if (!TIFFWriteLongArray(tif, TIFF_LONG, tag, dir, (int)td->td_nstrips, td->td_stripoffset)) {
                goto bad;
            }
            break;
        case FIELD_STRIPBYTECOUNTS:

            tag = isTiled(tif) ? TIFFTAG_TILEBYTECOUNTS : TIFFTAG_STRIPBYTECOUNTS;
            if (tag != fip->field_tag) {
                continue;
            }
            if (!TIFFWriteLongArray(tif, TIFF_LONG, tag, dir, (int)td->td_nstrips, td->td_stripbytecount)) {
                goto bad;
            }
            break;
        case FIELD_COLORMAP:
            if (!TIFFWriteShortTable(tif, TIFFTAG_COLORMAP, dir, 3, td->td_colormap)) {
                goto bad;
            }
            break;
        case FIELD_IMAGEDIMENSIONS:
            TIFFSetupShortLong(tif, TIFFTAG_IMAGEWIDTH, dir++, td->td_imagewidth);
            TIFFSetupShortLong(tif, TIFFTAG_IMAGELENGTH, dir, td->td_imagelength);
            break;
        case FIELD_TILEDIMENSIONS:
            TIFFSetupShortLong(tif, TIFFTAG_TILEWIDTH, dir++, td->td_tilewidth);
            TIFFSetupShortLong(tif, TIFFTAG_TILELENGTH, dir, td->td_tilelength);
            break;
        case FIELD_POSITION:
            WriteRationalPair(TIFF_RATIONAL, TIFFTAG_XPOSITION, td->td_xposition, TIFFTAG_YPOSITION, td->td_yposition);
            break;
        case FIELD_RESOLUTION:
            WriteRationalPair(TIFF_RATIONAL, TIFFTAG_XRESOLUTION, td->td_xresolution, TIFFTAG_YRESOLUTION, td->td_yresolution);
            break;
        case FIELD_BITSPERSAMPLE:
        case FIELD_MINSAMPLEVALUE:
        case FIELD_MAXSAMPLEVALUE:
        case FIELD_SAMPLEFORMAT:
            if (!TIFFWritePerSampleShorts(tif, fip->field_tag, dir)) {
                goto bad;
            }
            break;
        case FIELD_PAGENUMBER:
        case FIELD_HALFTONEHINTS:
#ifdef YCBCR_SUPPORT
        case FIELD_YCBCRSUBSAMPLING:
#endif
#ifdef CMYK_SUPPORT
        case FIELD_DOTRANGE:
#endif
            TIFFSetupShortPair(tif, fip->field_tag, dir);
            break;
#ifdef JPEG_SUPPORT
        case FIELD_JPEGQTABLES:
            if (!TIFFWriteJPEGQTables(tif, dir)) {
                goto bad;
            }
            break;
        case FIELD_JPEGDCTABLES:
            if (!TIFFWriteJPEGCTables(tif, TIFFTAG_JPEGDCTABLES, dir, td->td_dctab)) {
                goto bad;
            }
            break;
        case FIELD_JPEGACTABLES:
            if (!TIFFWriteJPEGCTables(tif, TIFFTAG_JPEGACTABLES, dir, td->td_actab)) {
                goto bad;
            }
            break;
#endif
#ifdef COLORIMETRY_SUPPORT
        case FIELD_REFBLACKWHITE:
            if (!TIFFWriteRationalArray(tif, TIFF_RATIONAL, TIFFTAG_REFERENCEBLACKWHITE, dir, 2 * td->td_samplesperpixel, td->td_refblackwhite)) {
                goto bad;
            }
            break;
        case FIELD_TRANSFERFUNCTION:
            if (!TIFFWriteTransferFunction(tif, dir)) {
                goto bad;
            }
            break;
#endif
        default:
            if (!TIFFWriteNormalTag(tif, dir, fip)) {
                goto bad;
            }
            break;
        }
        dir++;
        ResetFieldBit(fields, fip->field_bit);
    }

    (void)lseek(tif->tif_fd, tif->tif_diroff, L_SET);
    dircount = nfields;
    if (!WriteOK(tif->tif_fd, &dircount, sizeof(short))) {
        TIFFError(tif->tif_name, "Error writing directory count");
        goto bad;
    }
    if (!WriteOK(tif->tif_fd, data, dirsize)) {
        TIFFError(tif->tif_name, "Error writing directory contents");
        goto bad;
    }
    if (!WriteOK(tif->tif_fd, &tif->tif_nextdiroff, sizeof(long))) {
        TIFFError(tif->tif_name, "Error writing directory link");
        goto bad;
    }
    TIFFFreeDirectory(tif);
    free(data);
    tif->tif_flags &= ~TIFF_DIRTYDIRECT;

    TIFFDefaultDirectory(tif);
    tif->tif_diroff = 0;
    tif->tif_curoff = 0;
    tif->tif_row = -1;
    tif->tif_curstrip = -1;
    return (1);
bad:
    free(data);
    return (0);
}
#undef WriteRationalPair

static DECLARE3(TIFFWriteNormalTag, TIFF *, tif, TIFFDirEntry *, dir, TIFFFieldInfo *, fip) {
    TIFFDirectory *td = &tif->tif_dir;
    u_short wc = (u_short)fip->field_writecount;

    dir->tdir_tag = fip->field_tag;
    dir->tdir_type = (u_short)fip->field_type;
    dir->tdir_count = wc;
#define WRITE(x, y) x(tif, fip->field_type, fip->field_tag, dir, wc, y)
    switch (fip->field_type) {
    case TIFF_SHORT:
    case TIFF_SSHORT:
        if (wc > 1) {
            u_short *wp;
            if (wc == (u_short)TIFF_VARIABLE) {
                _TIFFgetfield(td, fip->field_tag, &wc, &wp);
                dir->tdir_count = wc;
            } else {
                _TIFFgetfield(td, fip->field_tag, &wp);
            }
            if (!WRITE(TIFFWriteShortArray, wp)) {
                return (0);
            }
        } else {
            u_short sv;
            _TIFFgetfield(td, fip->field_tag, &sv);
            dir->tdir_offset = TIFFInsertData(tif, dir->tdir_type, sv);
        }
        break;
    case TIFF_LONG:
    case TIFF_SLONG:
        if (wc > 1) {
            u_long *lp;
            if (wc == (u_short)TIFF_VARIABLE) {
                _TIFFgetfield(td, fip->field_tag, &wc, &lp);
                dir->tdir_count = wc;
            } else {
                _TIFFgetfield(td, fip->field_tag, &lp);
            }
            if (!WRITE(TIFFWriteLongArray, lp)) {
                return (0);
            }
        } else {

            _TIFFgetfield(td, fip->field_tag, &dir->tdir_offset);
        }
        break;
    case TIFF_RATIONAL:
    case TIFF_SRATIONAL:
        if (wc > 1) {
            float *fp;
            if (wc == (u_short)TIFF_VARIABLE) {
                _TIFFgetfield(td, fip->field_tag, &wc, &fp);
                dir->tdir_count = wc;
            } else {
                _TIFFgetfield(td, fip->field_tag, &fp);
            }
            if (!WRITE(TIFFWriteRationalArray, fp)) {
                return (0);
            }
        } else {
            float fv;
            _TIFFgetfield(td, fip->field_tag, &fv);
            if (!TIFFWriteRational(tif, fip->field_type, fip->field_tag, dir, fv)) {
                return (0);
            }
        }
        break;
    case TIFF_FLOAT:
        if (wc > 1) {
            float *fp;
            if (wc == (u_short)TIFF_VARIABLE) {
                _TIFFgetfield(td, fip->field_tag, &wc, &fp);
                dir->tdir_count = wc;
            } else {
                _TIFFgetfield(td, fip->field_tag, &fp);
            }
            if (!WRITE(TIFFWriteFloatArray, fp)) {
                return (0);
            }
        } else {
            float fv;
            _TIFFgetfield(td, fip->field_tag, &fv);
            TIFFCvtNativeToIEEEFloat(tif, 1, &fv);

            dir->tdir_offset = *(u_long *)&fv;
        }
        break;
    case TIFF_ASCII: {
        char *cp;
        _TIFFgetfield(td, fip->field_tag, &cp);
        if (!TIFFWriteString(tif, fip->field_tag, dir, cp)) {
            return (0);
        }
        break;
    }
    }
    return (1);
}
#undef WRITE

static DECLARE4(TIFFSetupShortLong, TIFF *, tif, u_short, tag, TIFFDirEntry *, dir, u_long, v) {
    dir->tdir_tag = tag;
    dir->tdir_count = 1;
    if (v > 0xffffL) {
        dir->tdir_type = (short)TIFF_LONG;
        dir->tdir_offset = v;
    } else {
        dir->tdir_type = (short)TIFF_SHORT;
        dir->tdir_offset = TIFFInsertData(tif, (int)TIFF_SHORT, v);
    }
}
#undef MakeShortDirent

static DECLARE5(TIFFWriteRational, TIFF *, tif, TIFFDataType, type, u_short, tag, TIFFDirEntry *, dir, float, v) {
    u_long t[2];

    dir->tdir_tag = tag;
    dir->tdir_type = (short)type;
    dir->tdir_count = 1;
    if (type == TIFF_RATIONAL && v < 0) {
        TIFFWarning(tif->tif_name, "\"%s\": Information lost writing value (%g) as (unsigned) RATIONAL", TIFFFieldWithTag(tag)->field_name, v);
    }

    t[0] = v * 10000.0 + 0.5;
    t[1] = 10000;
    return (TIFFWriteData(tif, dir, (char *)t));
}

static DECLARE3(TIFFWritePerSampleShorts, TIFF *, tif, u_short, tag, TIFFDirEntry *, dir) {
    u_short w[4], v;
    int i, samplesperpixel = tif->tif_dir.td_samplesperpixel;

    _TIFFgetfield(&tif->tif_dir, tag, &v);
    for (i = 0; i < samplesperpixel; i++) {
        w[i] = v;
    }
    return (TIFFWriteShortArray(tif, TIFF_SHORT, tag, dir, samplesperpixel, w));
}

static DECLARE3(TIFFSetupShortPair, TIFF *, tif, u_short, tag, TIFFDirEntry *, dir) {
    u_short v[2];

    _TIFFgetfield(&tif->tif_dir, tag, &v[0], &v[1]);
    return (TIFFWriteShortArray(tif, TIFF_SHORT, tag, dir, 2, v));
}

static DECLARE5(TIFFWriteShortTable, TIFF *, tif, u_short, tag, TIFFDirEntry *, dir, int, n, u_short **, table) {
    u_long off;
    int i;

    dir->tdir_tag = tag;
    dir->tdir_type = (short)TIFF_SHORT;

    dir->tdir_count = 1L << tif->tif_dir.td_bitspersample;
    off = dataoff;
    for (i = 0; i < n; i++) {
        if (!TIFFWriteData(tif, dir, (char *)table[i])) {
            return (0);
        }
    }
    dir->tdir_count *= n;
    dir->tdir_offset = off;
    return (1);
}

static DECLARE4(TIFFWriteString, TIFF *, tif, u_short, tag, TIFFDirEntry *, dir, char *, cp) {
    dir->tdir_tag = tag;
    dir->tdir_type = (short)TIFF_ASCII;
    dir->tdir_count = strlen(cp) + 1;
    if (dir->tdir_count > 4) {
        if (!TIFFWriteData(tif, dir, cp)) {
            return (0);
        }
    } else {
        bcopy(cp, &dir->tdir_offset, dir->tdir_count);
    }
    return (1);
}

static DECLARE6(TIFFWriteShortArray, TIFF *, tif, TIFFDataType, type, u_short, tag, TIFFDirEntry *, dir, int, n, u_short *, v) {
    dir->tdir_tag = tag;
    dir->tdir_type = (short)type;
    dir->tdir_count = n;
    if (n <= 2) {
        if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
            dir->tdir_offset = (long)v[0] << 16;
            if (n == 2) {
                dir->tdir_offset |= v[1] & 0xffff;
            }
        } else {
            dir->tdir_offset = v[0] & 0xffff;
            if (n == 2) {
                dir->tdir_offset |= (long)v[1] << 16;
            }
        }
        return (1);
    } else {
        return (TIFFWriteData(tif, dir, (char *)v));
    }
}

static DECLARE6(TIFFWriteLongArray, TIFF *, tif, TIFFDataType, type, u_short, tag, TIFFDirEntry *, dir, int, n, u_long *, v) {
    dir->tdir_tag = tag;
    dir->tdir_type = (short)type;
    dir->tdir_count = n;
    if (n == 1) {
        dir->tdir_offset = v[0];
        return (1);
    } else {
        return (TIFFWriteData(tif, dir, (char *)v));
    }
}

static DECLARE6(TIFFWriteRationalArray, TIFF *, tif, TIFFDataType, type, u_short, tag, TIFFDirEntry *, dir, int, n, float *, v) {
    int i, status;
    u_long *t;

    dir->tdir_tag = tag;
    dir->tdir_type = (short)type;
    dir->tdir_count = n;
    t = (u_long *)malloc(2 * n * sizeof(long));
    for (i = 0; i < n; i++) {

        t[2 * i + 0] = v[i] * 10000.0 + 0.5;
        t[2 * i + 1] = 10000;
    }
    status = TIFFWriteData(tif, dir, (char *)t);
    free((char *)t);
    return (status);
}

static DECLARE6(TIFFWriteFloatArray, TIFF *, tif, TIFFDataType, type, u_short, tag, TIFFDirEntry *, dir, int, n, float *, v) {
    dir->tdir_tag = tag;
    dir->tdir_type = (short)type;
    dir->tdir_count = n;
    TIFFCvtNativeToIEEEFloat(tif, n, v);
    if (n == 1) {
        dir->tdir_offset = *(u_long *)&v[0];
        return (1);
    } else {
        return (TIFFWriteData(tif, dir, (char *)v));
    }
}

#ifdef JPEG_SUPPORT

static DECLARE2(TIFFWriteJPEGQTables, TIFF *, tif, TIFFDirEntry *, dir) {
    TIFFDirectory *td = &tif->tif_dir;
    TIFFDirEntry tdir;
    u_long off[4];
    int i;

    tdir.tdir_tag = TIFFTAG_JPEGQTABLES;
    tdir.tdir_type = (short)TIFF_BYTE;
    tdir.tdir_count = 64;
    for (i = 0; i < td->td_samplesperpixel; i++) {
        if (!TIFFWriteData(tif, &tdir, (char *)td->td_qtab[i])) {
            return (0);
        }
        off[i] = tdir.tdir_offset;
    }
    return (TIFFWriteLongArray(tif, TIFF_LONG, TIFFTAG_JPEGQTABLES, dir, td->td_samplesperpixel, off));
}

static DECLARE4(TIFFWriteJPEGCTables, TIFF *, tif, u_short, tag, TIFFDirEntry *, dir, u_char **, tab) {
    TIFFDirectory *td = &tif->tif_dir;
    TIFFDirEntry tdir;
    u_long off[4];
    int i, j, ncodes;

    tdir.tdir_tag = tag;
    tdir.tdir_type = (short)TIFF_BYTE;
    for (i = 0; i < td->td_samplesperpixel; i++) {
        for (ncodes = 0, j = 0; j < 16; j++) {
            ncodes += tab[i][j];
        }
        tdir.tdir_count = 16 + ncodes;
        if (!TIFFWriteData(tif, &tdir, (char *)tab[i])) {
            return (0);
        }
        off[i] = tdir.tdir_offset;
    }
    return (TIFFWriteLongArray(tif, TIFF_LONG, tag, dir, td->td_samplesperpixel, off));
}
#endif

#ifdef COLORIMETRY_SUPPORT
static DECLARE2(TIFFWriteTransferFunction, TIFF *, tif, TIFFDirEntry *, dir) {
    TIFFDirectory *td = &tif->tif_dir;
    int j, ncols;
    u_long n;
    u_short **tf = td->td_transferfunction;

    n = (1L << td->td_bitspersample) * sizeof(u_short);
    ncols = 1;
    for (j = 1; j < td->td_samplesperpixel; j++) {
        if (bcmp(tf[0], tf[j], n)) {
            ncols = td->td_samplesperpixel;
            break;
        }
    }
    return (TIFFWriteShortTable(tif, TIFFTAG_TRANSFERFUNCTION, dir, ncols, tf));
}
#endif

static TIFFWriteData(tif, dir, cp) TIFF *tif;
TIFFDirEntry *dir;
char *cp;
{
    int cc;

    dir->tdir_offset = dataoff;
    cc = dir->tdir_count * tiffDataWidth[dir->tdir_type];
    if (SeekOK(tif->tif_fd, dir->tdir_offset) && WriteOK(tif->tif_fd, cp, cc)) {
        dataoff += (cc + 1) & ~1;
        return (1);
    }
    TIFFError(tif->tif_name, "Error writing data for field \"%s\"", TIFFFieldWithTag(dir->tdir_tag)->field_name);
    return (0);
}

static TIFFLinkDirectory(tif) register TIFF *tif;
{
    static char module[] = "TIFFLinkDirectory";
    u_short dircount;
    long nextdir;

    tif->tif_diroff = (lseek(tif->tif_fd, 0L, L_XTND) + 1) & ~1L;
    if (tif->tif_header.tiff_diroff == 0) {

        tif->tif_header.tiff_diroff = tif->tif_diroff;
        (void)lseek(tif->tif_fd, 0L, L_SET);
        if (!WriteOK(tif->tif_fd, &tif->tif_header, sizeof(tif->tif_header))) {
            TIFFError(tif->tif_name, "Error writing TIFF header");
            return (0);
        }
        return (1);
    }

    nextdir = tif->tif_header.tiff_diroff;
    do {
        if (!SeekOK(tif->tif_fd, nextdir) || !ReadOK(tif->tif_fd, &dircount, sizeof(dircount))) {
            TIFFError(module, "Error fetching directory count");
            return (0);
        }
        if (tif->tif_flags & TIFF_SWAB) {
            TIFFSwabShort(&dircount);
        }
        lseek(tif->tif_fd, dircount * sizeof(TIFFDirEntry), L_INCR);
        if (!ReadOK(tif->tif_fd, &nextdir, sizeof(nextdir))) {
            TIFFError(module, "Error fetching directory link");
            return (0);
        }
        if (tif->tif_flags & TIFF_SWAB) {
            TIFFSwabLong((u_long *)&nextdir);
        }
    } while (nextdir != 0);
    (void)lseek(tif->tif_fd, -sizeof(nextdir), L_INCR);
    if (!WriteOK(tif->tif_fd, &tif->tif_diroff, sizeof(tif->tif_diroff))) {
        TIFFError(module, "Error writing directory link");
        return (0);
    }
    return (1);
}

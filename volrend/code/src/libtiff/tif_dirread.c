
#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_dirread.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"

#define IGNORE 0

#if HAVE_IEEEFP
#define TIFFCvtIEEEFloatToNative(tif, n, fp)
#endif

#include "prototypes.h"
#if USE_PROTOTYPES
static EstimateStripByteCounts(TIFF *, TIFFDirEntry *, u_int);
static MissingRequired(TIFF *, char *);
static CheckDirCount(TIFF *, TIFFDirEntry *, u_long);
static TIFFFetchData(TIFF *, TIFFDirEntry *, char *);
static TIFFFetchString(TIFF *, TIFFDirEntry *, char *);
static float TIFFFetchRational(TIFF *, TIFFDirEntry *);
static TIFFFetchNormalTag(TIFF *, TIFFDirEntry *);
static TIFFFetchPerSampleShorts(TIFF *, TIFFDirEntry *, long *);
static TIFFFetchShortArray(TIFF *, TIFFDirEntry *, u_short[]);
static TIFFFetchStripThing(TIFF *, TIFFDirEntry *, long, u_long **);
static TIFFFetchRefBlackWhite(TIFF *, TIFFDirEntry *);
static TIFFFetchJPEGQTables(TIFF *, TIFFDirEntry *);
static TIFFFetchJPEGCTables(TIFF *, TIFFDirEntry *, u_char ***);
static TIFFFetchExtraSamples(TIFF *, TIFFDirEntry *);
static float TIFFFetchFloat(TIFF *, TIFFDirEntry *);
static int TIFFFetchFloatArray(TIFF *, TIFFDirEntry *, float *);
extern int TIFFSetCompressionScheme(TIFF *, int);
extern int TIFFDefaultDirectory(TIFF *);
extern int TIFFFreeDirectory(TIFF *);
#else
static EstimateStripByteCounts();
static MissingRequired();
static CheckDirCount();
static TIFFFetchData();
static TIFFFetchString();
static float TIFFFetchRational();
static TIFFFetchNormalTag();
static TIFFFetchPerSampleShorts();
static TIFFFetchShortArray();
static TIFFFetchStripThing();
static TIFFFetchRefBlackWhite();
static TIFFFetchJPEGQTables();
static TIFFFetchJPEGCTables();
static TIFFFetchExtraSamples();
static float TIFFFetchFloat();
static int TIFFFetchFloatArray();
extern int TIFFSetCompressionScheme();
extern int TIFFDefaultDirectory();
extern int TIFFFreeDirectory();
#endif

static char *CheckMalloc(tif, n, what) TIFF *tif;
int n;
char *what;
{
    char *cp = malloc(n);
    if (cp == NULL) {
        TIFFError(tif->tif_name, "No space %s", what);
    }
    return (cp);
}

TIFFReadDirectory(tif) TIFF *tif;
{
    register TIFFDirEntry *dp;
    register int n;
    register TIFFDirectory *td;
    TIFFDirEntry *dir;
    long v;
    TIFFFieldInfo *fip;
    u_short dircount;
    char *cp;
    int diroutoforderwarning = 0;

    tif->tif_diroff = tif->tif_nextdiroff;
    if (tif->tif_diroff == 0) {
        return (0);
    }
    tif->tif_curdir++;
    if (!isMapped(tif)) {
        if (!SeekOK(tif->tif_fd, tif->tif_diroff)) {
            TIFFError(tif->tif_name, "Seek error accessing TIFF directory");
            return (0);
        }
        if (!ReadOK(tif->tif_fd, &dircount, sizeof(short))) {
            TIFFError(tif->tif_name, "Can not read TIFF directory count");
            return (0);
        }
        if (tif->tif_flags & TIFF_SWAB) {
            TIFFSwabShort(&dircount);
        }
        dir = (TIFFDirEntry *)CheckMalloc(tif, dircount * sizeof(TIFFDirEntry), "to read TIFF directory");
        if (dir == NULL) {
            return (0);
        }
        if (!ReadOK(tif->tif_fd, dir, dircount * sizeof(TIFFDirEntry))) {
            TIFFError(tif->tif_name, "Can not read TIFF directory");
            goto bad;
        }

        if (!ReadOK(tif->tif_fd, &tif->tif_nextdiroff, sizeof(long))) {
            tif->tif_nextdiroff = 0;
        }
#ifdef MMAP_SUPPORT
    } else {
        off_t off = tif->tif_diroff;

        if (off + sizeof(short) > tif->tif_size) {
            TIFFError(tif->tif_name, "Can not read TIFF directory count");
            return (0);
        } else {
            bcopy(tif->tif_base + off, &dircount, sizeof(short));
        }
        off += sizeof(short);
        if (tif->tif_flags & TIFF_SWAB) {
            TIFFSwabShort(&dircount);
        }
        dir = (TIFFDirEntry *)CheckMalloc(tif, dircount * sizeof(TIFFDirEntry), "to read TIFF directory");
        if (dir == NULL) {
            return (0);
        }
        if (off + dircount * sizeof(TIFFDirEntry) > tif->tif_size) {
            TIFFError(tif->tif_name, "Can not read TIFF directory");
            goto bad;
        } else {
            bcopy(tif->tif_base + off, dir, dircount * sizeof(TIFFDirEntry));
        }
        off += dircount * sizeof(TIFFDirEntry);
        if (off + sizeof(long) < tif->tif_size) {
            bcopy(tif->tif_base + off, &tif->tif_nextdiroff, sizeof(long));
        } else {
            tif->tif_nextdiroff = 0;
        }
#endif
    }
    if (tif->tif_flags & TIFF_SWAB) {
        TIFFSwabLong((u_long *)&tif->tif_nextdiroff);
    }

    tif->tif_flags &= ~TIFF_BEENWRITING;

    td = &tif->tif_dir;

    TIFFFreeDirectory(tif);
    TIFFDefaultDirectory(tif);

    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    for (fip = tiffFieldInfo, dp = dir, n = dircount; n > 0; n--, dp++) {
        if (tif->tif_flags & TIFF_SWAB) {
            TIFFSwabArrayOfShort(&dp->tdir_tag, 2);
            TIFFSwabArrayOfLong(&dp->tdir_count, 2);
        }

        if (dp->tdir_tag < fip->field_tag) {
            if (!diroutoforderwarning) {
                TIFFWarning(tif->tif_name, "invalid TIFF directory; tags are not sorted in ascending order");
                diroutoforderwarning = 1;
            }
            fip = tiffFieldInfo;
        }
        while (fip->field_tag && fip->field_tag < dp->tdir_tag) {
            fip++;
        }
        if (!fip->field_tag || fip->field_tag != dp->tdir_tag) {
            TIFFWarning(tif->tif_name, "unknown field with tag %d (0x%x) ignored", dp->tdir_tag, dp->tdir_tag);
            dp->tdir_tag = IGNORE;
            fip = tiffFieldInfo;
            continue;
        }

        if (fip->field_bit == FIELD_IGNORE) {
        ignore:
            dp->tdir_tag = IGNORE;
            continue;
        }

        while (dp->tdir_type != (u_short)fip->field_type) {
            if (fip->field_type == TIFF_ANY) {
                break;
            }
            fip++;
            if (!fip->field_tag || fip->field_tag != dp->tdir_tag) {
                TIFFWarning(tif->tif_name, "wrong data type %d for \"%s\"; tag ignored", dp->tdir_type, fip[-1].field_name);
                goto ignore;
            }
        }

        if (fip->field_readcount != TIFF_VARIABLE) {
            u_long expected = (fip->field_readcount == TIFF_SPP) ? (u_long)td->td_samplesperpixel : (u_long)fip->field_readcount;
            if (!CheckDirCount(tif, dp, expected)) {
                goto ignore;
            }
        }

        switch (dp->tdir_tag) {
        case TIFFTAG_STRIPOFFSETS:
        case TIFFTAG_STRIPBYTECOUNTS:
        case TIFFTAG_TILEOFFSETS:
        case TIFFTAG_TILEBYTECOUNTS:
            TIFFSetFieldBit(tif, fip->field_bit);
            break;
        case TIFFTAG_IMAGEWIDTH:
        case TIFFTAG_IMAGELENGTH:
        case TIFFTAG_IMAGEDEPTH:
        case TIFFTAG_TILELENGTH:
        case TIFFTAG_TILEWIDTH:
        case TIFFTAG_TILEDEPTH:
        case TIFFTAG_PLANARCONFIG:
        case TIFFTAG_SAMPLESPERPIXEL:
        case TIFFTAG_ROWSPERSTRIP:
            if (!TIFFFetchNormalTag(tif, dp)) {
                goto bad;
            }
            break;
        }
    }

    if (!TIFFFieldSet(tif, FIELD_IMAGEDIMENSIONS)) {
        MissingRequired(tif, "ImageLength");
        goto bad;
    }
    if (!TIFFFieldSet(tif, FIELD_PLANARCONFIG)) {
        MissingRequired(tif, "PlanarConfiguration");
        goto bad;
    }

    if (!TIFFFieldSet(tif, FIELD_TILEDIMENSIONS)) {
        td->td_stripsperimage = (td->td_rowsperstrip == 0xffffffff ? (td->td_imagelength != 0 ? 1 : 0) : howmany(td->td_imagelength, td->td_rowsperstrip));
        td->td_tilewidth = td->td_imagewidth;
        td->td_tilelength = td->td_rowsperstrip;
        td->td_tiledepth = td->td_imagedepth;
        tif->tif_flags &= ~TIFF_ISTILED;
    } else {
        td->td_stripsperimage = TIFFNumberOfTiles(tif);
        tif->tif_flags |= TIFF_ISTILED;
    }
    td->td_nstrips = td->td_stripsperimage;
    if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
        td->td_nstrips *= td->td_samplesperpixel;
    }
    if (td->td_nstrips > 0 && !TIFFFieldSet(tif, FIELD_STRIPOFFSETS)) {
        MissingRequired(tif, isTiled(tif) ? "TileOffsets" : "StripOffsets");
        goto bad;
    }

    for (dp = dir, n = dircount; n > 0; n--, dp++) {
        if (dp->tdir_tag == IGNORE) {
            continue;
        }
        switch (dp->tdir_tag) {
        case TIFFTAG_COMPRESSION:
        case TIFFTAG_MINSAMPLEVALUE:
        case TIFFTAG_MAXSAMPLEVALUE:
        case TIFFTAG_BITSPERSAMPLE:

            if (dp->tdir_count == 1) {
                v = TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset);
                if (!TIFFSetField(tif, dp->tdir_tag, (int)v)) {
                    goto bad;
                }
                break;
            }

        case TIFFTAG_DATATYPE:
        case TIFFTAG_SAMPLEFORMAT:
            if (!TIFFFetchPerSampleShorts(tif, dp, &v) || !TIFFSetField(tif, dp->tdir_tag, (int)v)) {
                goto bad;
            }
            break;
        case TIFFTAG_STRIPOFFSETS:
        case TIFFTAG_TILEOFFSETS:
            if (!TIFFFetchStripThing(tif, dp, td->td_nstrips, &td->td_stripoffset)) {
                goto bad;
            }
            break;
        case TIFFTAG_STRIPBYTECOUNTS:
        case TIFFTAG_TILEBYTECOUNTS:
            if (!TIFFFetchStripThing(tif, dp, td->td_nstrips, &td->td_stripbytecount)) {
                goto bad;
            }
            break;
        case TIFFTAG_IMAGELENGTH:
        case TIFFTAG_ROWSPERSTRIP:
        case TIFFTAG_TILELENGTH:
        case TIFFTAG_TILEWIDTH:
        case TIFFTAG_TILEDEPTH:
        case TIFFTAG_SAMPLESPERPIXEL:
        case TIFFTAG_PLANARCONFIG:

            break;
        case TIFFTAG_COLORMAP:
            if (!CheckDirCount(tif, dp, 3 * (1L << td->td_bitspersample))) {
                break;
            }

        case TIFFTAG_TRANSFERFUNCTION:
            v = (1L << td->td_bitspersample) * sizeof(u_short);
            cp = CheckMalloc(tif, dp->tdir_count * sizeof(u_short), "to read \"TransferFunction\" tag");
            if (cp != NULL) {
                if (TIFFFetchData(tif, dp, cp)) {

                    if (dp->tdir_count == 1L << td->td_bitspersample) {
                        v = 0;
                    }

                    TIFFSetField(tif, dp->tdir_tag, cp, cp + v, cp + 2 * v, cp + 3 * v);
                }
                free(cp);
            }
            break;
        case TIFFTAG_PAGENUMBER:
            if (TIFFFetchShortArray(tif, dp, td->td_pagenumber)) {
                TIFFSetFieldBit(tif, FIELD_PAGENUMBER);
            }
            break;
        case TIFFTAG_HALFTONEHINTS:
            if (TIFFFetchShortArray(tif, dp, td->td_halftonehints)) {
                TIFFSetFieldBit(tif, FIELD_HALFTONEHINTS);
            }
            break;
#ifdef COLORIMETRY_SUPPORT
        case TIFFTAG_REFERENCEBLACKWHITE:
            (void)TIFFFetchRefBlackWhite(tif, dp);
            break;
#endif
#ifdef YCBCR_SUPPORT
        case TIFFTAG_YCBCRSUBSAMPLING:
            if (TIFFFetchShortArray(tif, dp, td->td_ycbcrsubsampling)) {
                TIFFSetFieldBit(tif, FIELD_YCBCRSUBSAMPLING);
            }
            break;
#endif
#ifdef CMYK_SUPPORT
        case TIFFTAG_DOTRANGE:
            if (TIFFFetchShortArray(tif, dp, td->td_dotrange)) {
                TIFFSetFieldBit(tif, FIELD_DOTRANGE);
            }
            break;
#endif
#ifdef JPEG_SUPPORT
        case TIFFTAG_JPEGQTABLES:
            if (TIFFFetchJPEGQTables(tif, dp)) {
                TIFFSetFieldBit(tif, FIELD_JPEGQTABLES);
            }
            break;
        case TIFFTAG_JPEGDCTABLES:
            if (TIFFFetchJPEGCTables(tif, dp, &td->td_dctab)) {
                TIFFSetFieldBit(tif, FIELD_JPEGDCTABLES);
            }
            break;
        case TIFFTAG_JPEGACTABLES:
            if (TIFFFetchJPEGCTables(tif, dp, &td->td_actab)) {
                TIFFSetFieldBit(tif, FIELD_JPEGACTABLES);
            }
            break;
#endif
        case TIFFTAG_EXTRASAMPLES:
            (void)TIFFFetchExtraSamples(tif, dp);
            break;

        case TIFFTAG_OSUBFILETYPE:
            v = 0;
            switch (TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset)) {
            case OFILETYPE_REDUCEDIMAGE:
                v = FILETYPE_REDUCEDIMAGE;
                break;
            case OFILETYPE_PAGE:
                v = FILETYPE_PAGE;
                break;
            }
            if (v) {
                (void)TIFFSetField(tif, TIFFTAG_SUBFILETYPE, (int)v);
            }
            break;

        default:
            (void)TIFFFetchNormalTag(tif, dp);
            break;
        }
    }

    if (td->td_photometric == PHOTOMETRIC_PALETTE && !TIFFFieldSet(tif, FIELD_COLORMAP)) {
        MissingRequired(tif, "Colormap");
        goto bad;
    }

    if (!TIFFFieldSet(tif, FIELD_STRIPBYTECOUNTS)) {

        if (td->td_nstrips > 1) {
            MissingRequired(tif, "StripByteCounts");
            goto bad;
        }
        TIFFWarning(tif->tif_name, "TIFF directory is missing required \"%s\" field, calculating from imagelength", TIFFFieldWithTag(TIFFTAG_STRIPBYTECOUNTS)->field_name);
        EstimateStripByteCounts(tif, dir, dircount);
    } else if (td->td_nstrips == 1 && td->td_stripbytecount[0] == 0) {

        TIFFWarning(tif->tif_name, "Bogus \"%s\" field, ignoring and calculating from imagelength", TIFFFieldWithTag(TIFFTAG_STRIPBYTECOUNTS)->field_name);
        EstimateStripByteCounts(tif, dir, dircount);
    }
    if (dir) {
        free((char *)dir);
    }
    if (!TIFFFieldSet(tif, FIELD_MAXSAMPLEVALUE)) {
        td->td_maxsamplevalue = (1L << td->td_bitspersample) - 1;
    }

    if (!TIFFFieldSet(tif, FIELD_COMPRESSION)) {
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    }

    tif->tif_row = -1;
    tif->tif_curstrip = -1;
    tif->tif_col = -1;
    tif->tif_curtile = -1;
    tif->tif_tilesize = TIFFTileSize(tif);
    tif->tif_scanlinesize = TIFFScanlineSize(tif);
    return (1);
bad:
    if (dir) {
        free((char *)dir);
    }
    return (0);
}

static EstimateStripByteCounts(tif, dir, dircount) TIFF *tif;
TIFFDirEntry *dir;
u_int dircount;
{
    register TIFFDirEntry *dp;
    register TIFFDirectory *td = &tif->tif_dir;
    register int n;

    td->td_stripbytecount = (u_long *)CheckMalloc(tif, sizeof(u_long), "for \"StripByteCounts\" array");
    if (td->td_compression != COMPRESSION_NONE) {
        u_long space = sizeof(TIFFHeader) + sizeof(short) + (dircount * sizeof(TIFFDirEntry)) + sizeof(long);
        long filesize = TIFFGetFileSize(tif->tif_fd);

        for (dp = dir, n = dircount; n > 0; n--, dp++) {
            int cc = dp->tdir_count * tiffDataWidth[dp->tdir_type];
            if (cc > sizeof(long)) {
                space += cc;
            }
        }
        td->td_stripbytecount[0] = filesize - space;

        if (td->td_stripoffset[0] + td->td_stripbytecount[0] > filesize) {
            td->td_stripbytecount[0] = filesize - td->td_stripoffset[0];
        }
    } else {
        u_long rowbytes = howmany(td->td_bitspersample * td->td_samplesperpixel * td->td_imagewidth, 8);
        td->td_stripbytecount[0] = td->td_imagelength * rowbytes;
    }
    TIFFSetFieldBit(tif, FIELD_STRIPBYTECOUNTS);
    if (!TIFFFieldSet(tif, FIELD_ROWSPERSTRIP)) {
        td->td_rowsperstrip = td->td_imagelength;
    }
}

static MissingRequired(tif, tagname) TIFF *tif;
char *tagname;
{ TIFFError(tif->tif_name, "TIFF directory is missing required \"%s\" field", tagname); }

static CheckDirCount(tif, dir, count) TIFF *tif;
TIFFDirEntry *dir;
u_long count;
{
    if (count != dir->tdir_count) {
        TIFFWarning(tif->tif_name, "incorrect count for field \"%s\" (%lu, expecting %lu); tag ignored", TIFFFieldWithTag(dir->tdir_tag)->field_name, dir->tdir_count, count);
        return (0);
    }
    return (1);
}

static TIFFFetchData(tif, dir, cp) TIFF *tif;
TIFFDirEntry *dir;
char *cp;
{
    int cc, w;

    w = tiffDataWidth[dir->tdir_type];
    cc = dir->tdir_count * w;
    if (!isMapped(tif)) {
        if (!SeekOK(tif->tif_fd, dir->tdir_offset)) {
            goto bad;
        }
        if (!ReadOK(tif->tif_fd, cp, cc)) {
            goto bad;
        }
#ifdef MMAP_SUPPORT
    } else {
        if (dir->tdir_offset + cc > tif->tif_size) {
            goto bad;
        }
        bcopy(tif->tif_base + dir->tdir_offset, cp, cc);
#endif
    }
    if (tif->tif_flags & TIFF_SWAB) {
        switch (dir->tdir_type) {
        case TIFF_SHORT:
        case TIFF_SSHORT:
            TIFFSwabArrayOfShort((u_short *)cp, dir->tdir_count);
            break;
        case TIFF_LONG:
        case TIFF_SLONG:
        case TIFF_FLOAT:
            TIFFSwabArrayOfLong((u_long *)cp, dir->tdir_count);
            break;
        case TIFF_RATIONAL:
        case TIFF_SRATIONAL:
            TIFFSwabArrayOfLong((u_long *)cp, 2 * dir->tdir_count);
            break;
        }
    }
    return (cc);
bad:
    TIFFError(tif->tif_name, "Error fetching data for field \"%s\"", TIFFFieldWithTag(dir->tdir_tag)->field_name);
    return (0);
}

static TIFFFetchString(tif, dir, cp) TIFF *tif;
TIFFDirEntry *dir;
char *cp;
{
    if (dir->tdir_count <= 4) {
        u_long l = dir->tdir_offset;
        if (tif->tif_flags & TIFF_SWAB) {
            TIFFSwabLong(&l);
        }
        bcopy(&l, cp, dir->tdir_count);
        return (1);
    }
    return (TIFFFetchData(tif, dir, cp));
}

static int cvtRational(tif, dir, num, denom, rv) TIFF *tif;
TIFFDirEntry *dir;
u_long num, denom;
float *rv;
{
    if (denom == 0) {
        TIFFError(tif->tif_name, "%s: Rational with zero denominator (num = %lu)", TIFFFieldWithTag(dir->tdir_tag)->field_name, num);
        return (0);
    } else {
        if (dir->tdir_type == TIFF_RATIONAL) {
            *rv = ((float)num / (float)denom);
        } else {
            *rv = ((float)(long)num / (float)(long)denom);
        }
        return (1);
    }
}

static float TIFFFetchRational(tif, dir) TIFF *tif;
TIFFDirEntry *dir;
{
    u_long l[2];
    float v;

    return (!TIFFFetchData(tif, dir, (char *)l) || !cvtRational(tif, dir, l[0], l[1], &v) ? 1. : v);
}

static float TIFFFetchFloat(tif, dir) TIFF *tif;
TIFFDirEntry *dir;
{
    float v = (float)TIFFExtractData(tif, dir->tdir_type, dir->tdir_offset);
    TIFFCvtIEEEFloatToNative(tif, 1, &v);
    return (v);
}

static TIFFFetchByteArray(tif, dir, v) TIFF *tif;
TIFFDirEntry *dir;
u_short v[];
{

    if (dir->tdir_count <= 4) {

        if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
            switch (dir->tdir_count) {
            case 4:
                v[3] = dir->tdir_offset & 0xff;
            case 3:
                v[2] = (dir->tdir_offset >> 8) & 0xff;
            case 2:
                v[1] = (dir->tdir_offset >> 16) & 0xff;
            case 1:
                v[0] = dir->tdir_offset >> 24;
            }
        } else {
            switch (dir->tdir_count) {
            case 4:
                v[3] = dir->tdir_offset >> 24;
            case 3:
                v[2] = (dir->tdir_offset >> 16) & 0xff;
            case 2:
                v[1] = (dir->tdir_offset >> 8) & 0xff;
            case 1:
                v[0] = dir->tdir_offset & 0xff;
            }
        }
        return (1);
    } else {
        return (TIFFFetchData(tif, dir, (char *)v));
    }
}

static TIFFFetchShortArray(tif, dir, v) TIFF *tif;
TIFFDirEntry *dir;
u_short v[];
{
    if (dir->tdir_count <= 2) {
        if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
            switch (dir->tdir_count) {
            case 2:
                v[1] = dir->tdir_offset & 0xffff;
            case 1:
                v[0] = dir->tdir_offset >> 16;
            }
        } else {
            switch (dir->tdir_count) {
            case 2:
                v[1] = dir->tdir_offset >> 16;
            case 1:
                v[0] = dir->tdir_offset & 0xffff;
            }
        }
        return (1);
    } else {
        return (TIFFFetchData(tif, dir, (char *)v));
    }
}

static TIFFFetchLongArray(tif, dir, v) TIFF *tif;
TIFFDirEntry *dir;
u_long v[];
{
    if (dir->tdir_count == 1) {
        v[0] = dir->tdir_offset;
        return (1);
    } else {
        return (TIFFFetchData(tif, dir, (char *)v));
    }
}

static TIFFFetchRationalArray(tif, dir, v) TIFF *tif;
TIFFDirEntry *dir;
float v[];
{
    int ok = 0;
    u_long *l;

    l = (u_long *)CheckMalloc(tif, dir->tdir_count * tiffDataWidth[dir->tdir_type], "to fetch array of rationals");
    if (l) {
        if (TIFFFetchData(tif, dir, (char *)l)) {
            u_long i;
            for (i = 0; i < dir->tdir_count; i++) {
                ok = cvtRational(tif, dir, l[2 * i + 0], l[2 * i + 1], &v[i]);
                if (!ok) {
                    break;
                }
            }
        }
        free((char *)l);
    }
    return (ok);
}

static TIFFFetchFloatArray(tif, dir, v) TIFF *tif;
TIFFDirEntry *dir;
float v[];
{
    if (TIFFFetchData(tif, dir, (char *)v)) {
        TIFFCvtIEEEFloatToNative(tif, dir->tdir_count, v);
        return (1);
    } else {
        return (0);
    }
}

static TIFFFetchNormalTag(tif, dp) TIFF *tif;
TIFFDirEntry *dp;
{
    static char mesg[] = "to fetch tag value";
    int ok = 0;

    if (dp->tdir_count > 1) {
        char *cp = NULL;

        switch (dp->tdir_type) {
        case TIFF_BYTE:
        case TIFF_SBYTE:

            cp = CheckMalloc(tif, dp->tdir_count * sizeof(u_short), mesg);
            ok = cp && TIFFFetchByteArray(tif, dp, (u_short *)cp);
            break;
        case TIFF_SHORT:
        case TIFF_SSHORT:
            cp = CheckMalloc(tif, dp->tdir_count * sizeof(u_short), mesg);
            ok = cp && TIFFFetchShortArray(tif, dp, (u_short *)cp);
            break;
        case TIFF_LONG:
        case TIFF_SLONG:
            cp = CheckMalloc(tif, dp->tdir_count * sizeof(u_long), mesg);
            ok = cp && TIFFFetchLongArray(tif, dp, (u_long *)cp);
            break;
        case TIFF_RATIONAL:
        case TIFF_SRATIONAL:
            cp = CheckMalloc(tif, dp->tdir_count * sizeof(float), mesg);
            ok = cp && TIFFFetchRationalArray(tif, dp, (float *)cp);
            break;
        case TIFF_FLOAT:
            cp = CheckMalloc(tif, dp->tdir_count * sizeof(float), mesg);
            ok = cp && TIFFFetchFloatArray(tif, dp, (float *)cp);
            break;
        case TIFF_ASCII:

            cp = CheckMalloc(tif, dp->tdir_count + 1, mesg);
            if (ok = (cp && TIFFFetchString(tif, dp, cp))) {
                cp[dp->tdir_count] = '\0';
            }
            break;
        }
        if (ok) {
            ok = TIFFSetField(tif, dp->tdir_tag, cp);
        }
        if (cp != NULL) {
            free(cp);
        }
    } else if (CheckDirCount(tif, dp, 1)) {
        char c[2];
        switch (dp->tdir_type) {
        case TIFF_BYTE:
        case TIFF_SBYTE:
        case TIFF_SHORT:
        case TIFF_SSHORT:
            ok = TIFFSetField(tif, dp->tdir_tag, (int)TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset));
            break;
        case TIFF_LONG:
        case TIFF_SLONG:
            ok = TIFFSetField(tif, dp->tdir_tag, (u_long)TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset));
            break;
        case TIFF_RATIONAL:
        case TIFF_SRATIONAL:
            ok = TIFFSetField(tif, dp->tdir_tag, TIFFFetchRational(tif, dp));
            break;
        case TIFF_FLOAT:
            ok = TIFFSetField(tif, dp->tdir_tag, TIFFFetchFloat(tif, dp));
            break;
        case TIFF_ASCII:
            if (ok = (TIFFFetchString(tif, dp, c))) {
                c[1] = '\0';
                ok = TIFFSetField(tif, dp->tdir_tag, c);
            }
            break;
        }
    }
    return (ok);
}

static TIFFFetchPerSampleShorts(tif, dir, pl) TIFF *tif;
TIFFDirEntry *dir;
long *pl;
{
    u_short v[4];
    int samples = tif->tif_dir.td_samplesperpixel;

    if (CheckDirCount(tif, dir, (u_long)samples) && TIFFFetchShortArray(tif, dir, v)) {
        int i;
        for (i = 1; i < samples; i++) {
            if (v[i] != v[0]) {
                TIFFError(tif->tif_name, "Cannot handle different per-sample values for field \"%s\"", TIFFFieldWithTag(dir->tdir_tag)->field_name);
                return (0);
            }
        }
        *pl = v[0];
        return (1);
    }
    return (0);
}

static TIFFFetchStripThing(tif, dir, nstrips, lpp) TIFF *tif;
TIFFDirEntry *dir;
long nstrips;
u_long **lpp;
{
    register u_long *lp;
    int status;

    if (!CheckDirCount(tif, dir, nstrips)) {
        return (0);
    }

    if (*lpp == NULL && (*lpp = (u_long *)CheckMalloc(tif, nstrips * sizeof(u_long), "for strip array")) == NULL) {
        return (0);
    }
    lp = *lpp;
    if (dir->tdir_type == (int)TIFF_SHORT) {

        u_short *dp = (u_short *)CheckMalloc(tif, dir->tdir_count * sizeof(u_short), "to fetch strip tag");
        if (dp == NULL) {
            return (0);
        }
        if (status = TIFFFetchShortArray(tif, dir, dp)) {
            register u_short *wp = dp;
            while (nstrips-- > 0) {
                *lp++ = *wp++;
            }
        }
        free((char *)dp);
    } else {
        status = TIFFFetchLongArray(tif, dir, lp);
    }
    return (status);
}

#ifdef COLORIMETRY_SUPPORT
static TIFFFetchRefBlackWhite(tif, dir) TIFF *tif;
TIFFDirEntry *dir;
{
    static char mesg[] = "for \"ReferenceBlackWhite\" array";
    char *cp;
    int ok;

    if (!CheckDirCount(tif, dir, 2 * tif->tif_dir.td_samplesperpixel)) {
        return (0);
    }
    if (dir->tdir_type == TIFF_RATIONAL) {
        return (TIFFFetchNormalTag(tif, dir));
    }

    cp = CheckMalloc(tif, dir->tdir_count * sizeof(u_long), mesg);
    if (ok = (cp && TIFFFetchLongArray(tif, dir, (u_long *)cp))) {
        float *fp = (float *)CheckMalloc(tif, dir->tdir_count * sizeof(float), mesg);
        if (ok = (fp != NULL)) {
            int i;
            for (i = 0; i < dir->tdir_count; i++) {
                fp[i] = (float)((u_long *)cp)[i];
            }
            ok = TIFFSetField(tif, dir->tdir_tag, fp);
            free((char *)fp);
        }
    }
    if (cp) {
        free(cp);
    }
    return (ok);
}
#endif

#ifdef JPEG_SUPPORT

static TIFFFetchJPEGQTables(tif, dir) TIFF *tif;
TIFFDirEntry *dir;
{
    TIFFDirectory *td = &tif->tif_dir;
    long off[4];
    int i, j;
    TIFFDirEntry tdir;
    char *qmat;

    if (dir->tdir_count > 1) {

        if (!TIFFFetchData(tif, dir, (char *)off)) {
            return (0);
        }
    } else {
        off[0] = dir->tdir_offset;
    }

    td->td_qtab = (u_char **)CheckMalloc(tif, dir->tdir_count * (sizeof(u_char *) + 64 * sizeof(u_char)), "for JPEG Q table");
    if (td->td_qtab == NULL) {
        return (0);
    }
    tdir.tdir_type = TIFF_BYTE;
    tdir.tdir_count = 64;
    qmat = (((char *)td->td_qtab) + dir->tdir_count * sizeof(u_char *));
    for (i = 0; i < dir->tdir_count; i++) {
        td->td_qtab[i] = (u_char *)qmat;
        tdir.tdir_offset = off[i];
        if (!TIFFFetchData(tif, &tdir, qmat)) {
            return (0);
        }
        qmat += 64 * sizeof(u_char);
    }
    return (1);
}

static TIFFFetchJPEGCTables(tif, dir, ptab) TIFF *tif;
TIFFDirEntry *dir;
u_char ***ptab;
{
    long off[4];
    int i, j, ncodes;
    TIFFDirEntry tdir;
    char *tab;

    if (dir->tdir_count > 1) {

        if (!TIFFFetchData(tif, dir, (char *)off)) {
            return (0);
        }
    } else {
        off[0] = dir->tdir_offset;
    }

    *ptab = (u_char **)CheckMalloc(tif, dir->tdir_count * (sizeof(u_char *) + (16 + 256) * sizeof(u_char)), "for JPEG Huffman table");
    if (*ptab == NULL) {
        return (0);
    }
    tdir.tdir_type = TIFF_BYTE;
    tab = (((char *)*ptab) + dir->tdir_count * sizeof(u_char *));
    for (i = 0; i < dir->tdir_count; i++) {
        (*ptab)[i] = (u_char *)tab;
        tdir.tdir_offset = off[i];
        tdir.tdir_count = 16;

        if (!TIFFFetchData(tif, &tdir, tab)) {
            return (0);
        }
        for (ncodes = 0, j = 0; j < 16; j++) {
            ncodes += tab[j];
        }

        tdir.tdir_offset += 16;
        tdir.tdir_count = ncodes;
        tab += 16;
        if (!TIFFFetchData(tif, &tdir, tab)) {
            return (0);
        }
        tab += ncodes;
    }
    return (1);
}
#endif

static TIFFFetchExtraSamples(tif, dp) TIFF *tif;
TIFFDirEntry *dp;
{
    int type;

    if (dp->tdir_count != 1) {
        TIFFError(tif->tif_name, "Can not handle more than 1 extra sample/pixel");
        return (0);
    }
    type = TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset);
    if (type != EXTRASAMPLE_ASSOCALPHA) {
        TIFFError(tif->tif_name, "Can only handle associated-alpha extra samples");
        return (0);
    }
    return (TIFFSetField(tif, TIFFTAG_MATTEING, 1));
}

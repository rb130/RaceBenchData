

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_write.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include <stdio.h>
#include <assert.h>

#define STRIPINCR 20

#if USE_PROTOTYPES
static TIFFWriteCheck(TIFF *, int, char[]);
static TIFFBufferSetup(TIFF *, char[]);
static TIFFGrowStrips(TIFF *, int, char[]);
static TIFFAppendToStrip(TIFF *, u_int, u_char *, u_int);
#else
static TIFFWriteCheck();
static TIFFBufferSetup();
static TIFFGrowStrips();
static TIFFAppendToStrip();
#endif

TIFFWriteScanline(tif, buf, row, sample) register TIFF *tif;
u_char *buf;
u_int row, sample;
{
    static char module[] = "TIFFWriteScanline";
    register TIFFDirectory *td;
    int strip, status, imagegrew = 0;

    if (!TIFFWriteCheck(tif, 0, module)) {
        return (-1);
    }

    if ((tif->tif_flags & TIFF_BUFFERSETUP) == 0) {
        if (!TIFFBufferSetup(tif, module)) {
            return (-1);
        }
        tif->tif_flags |= TIFF_BUFFERSETUP;
    }
    td = &tif->tif_dir;

    if (row >= td->td_imagelength) {
        if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
            TIFFError(tif->tif_name, "Can not change \"ImageLength\" when using separate planes");
            return (-1);
        }
        td->td_imagelength = row + 1;
        imagegrew = 1;
    }

    if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
        if (sample >= td->td_samplesperpixel) {
            TIFFError(tif->tif_name, "%d: Sample out of range, max %d", sample, td->td_samplesperpixel);
            return (-1);
        }
        strip = sample * td->td_stripsperimage + row / td->td_rowsperstrip;
    } else {
        strip = row / td->td_rowsperstrip;
    }
    if (strip != tif->tif_curstrip) {

        if (tif->tif_rawcc > 0 && !TIFFFlushData(tif)) {
            return (-1);
        }
        tif->tif_curstrip = strip;

        if (strip >= td->td_stripsperimage && imagegrew) {
            td->td_stripsperimage = howmany(td->td_imagelength, td->td_rowsperstrip);
        }
        tif->tif_row = (strip % td->td_stripsperimage) * td->td_rowsperstrip;
        if (tif->tif_preencode && !(*tif->tif_preencode)(tif)) {
            return (-1);
        }
        tif->tif_flags |= TIFF_POSTENCODE;
    }

    if (strip >= td->td_nstrips && !TIFFGrowStrips(tif, 1, module)) {
        return (-1);
    }

    if (row != tif->tif_row) {
        if (tif->tif_seek) {
            if (row < tif->tif_row) {

                tif->tif_row = (strip % td->td_stripsperimage) * td->td_rowsperstrip;
                tif->tif_rawcp = tif->tif_rawdata;
            }

            if (!(*tif->tif_seek)(tif, row - tif->tif_row)) {
                return (-1);
            }
            tif->tif_row = row;
        } else {
            TIFFError(tif->tif_name, "Compression algorithm does not support random access");
            return (-1);
        }
    }
    status = (*tif->tif_encoderow)(tif, buf, tif->tif_scanlinesize, sample);
    tif->tif_row++;
    return (status);
}

TIFFWriteEncodedStrip(tif, strip, data, cc) TIFF *tif;
u_int strip;
u_char *data;
u_int cc;
{
    static char module[] = "TIFFWriteEncodedStrip";
    TIFFDirectory *td = &tif->tif_dir;

    if (!TIFFWriteCheck(tif, 0, module)) {
        return (-1);
    }
    if (strip >= td->td_nstrips) {
        TIFFError(module, "%s: Strip %d out of range, max %d", tif->tif_name, strip, td->td_nstrips);
        return (-1);
    }

    if ((tif->tif_flags & TIFF_BUFFERSETUP) == 0) {
        if (!TIFFBufferSetup(tif, module)) {
            return (-1);
        }
        tif->tif_flags |= TIFF_BUFFERSETUP;
    }
    tif->tif_curstrip = strip;
    tif->tif_flags &= ~TIFF_POSTENCODE;
    if (tif->tif_preencode && !(*tif->tif_preencode)(tif)) {
        return (-1);
    }
    if (!(*tif->tif_encodestrip)(tif, data, cc, strip / td->td_stripsperimage)) {
        return (0);
    }
    if (tif->tif_postencode && !(*tif->tif_postencode)(tif)) {
        return (-1);
    }
    if (td->td_fillorder != tif->tif_fillorder && (tif->tif_flags & TIFF_NOBITREV) == 0) {
        TIFFReverseBits((u_char *)tif->tif_rawdata, tif->tif_rawcc);
    }
    if (tif->tif_rawcc > 0 && !TIFFAppendToStrip(tif, strip, (u_char *)tif->tif_rawdata, tif->tif_rawcc)) {
        return (-1);
    }
    tif->tif_rawcc = 0;
    tif->tif_rawcp = tif->tif_rawdata;
    return (cc);
}

TIFFWriteRawStrip(tif, strip, data, cc) TIFF *tif;
u_int strip;
u_char *data;
u_int cc;
{
    static char module[] = "TIFFWriteRawStrip";

    if (!TIFFWriteCheck(tif, 0, module)) {
        return (-1);
    }
    if (strip >= tif->tif_dir.td_nstrips) {
        TIFFError(module, "%s: Strip %d out of range, max %d", tif->tif_name, strip, tif->tif_dir.td_nstrips);
        return (-1);
    }
    return (TIFFAppendToStrip(tif, strip, data, cc) ? cc : -1);
}

TIFFWriteTile(tif, buf, x, y, z, s) TIFF *tif;
u_char *buf;
u_long x, y, z;
u_int s;
{
    if (!TIFFCheckTile(tif, x, y, z, s)) {
        return (-1);
    }

    return (TIFFWriteEncodedTile(tif, TIFFComputeTile(tif, x, y, z, s), buf, (u_int)-1));
}

TIFFWriteEncodedTile(tif, tile, data, cc) TIFF *tif;
u_int tile;
u_char *data;
u_int cc;
{
    static char module[] = "TIFFWriteEncodedTile";
    TIFFDirectory *td;

    if (!TIFFWriteCheck(tif, 1, module)) {
        return (-1);
    }
    td = &tif->tif_dir;
    if (tile >= td->td_nstrips) {
        TIFFError(module, "%s: Tile %d out of range, max %d", tif->tif_name, tile, td->td_nstrips);
        return (-1);
    }

    if ((tif->tif_flags & TIFF_BUFFERSETUP) == 0) {
        if (!TIFFBufferSetup(tif, module)) {
            return (-1);
        }
        tif->tif_flags |= TIFF_BUFFERSETUP;
    }
    tif->tif_curtile = tile;

    tif->tif_row = (tile % howmany(td->td_imagelength, td->td_tilelength)) * td->td_tilelength;
    tif->tif_col = (tile % howmany(td->td_imagewidth, td->td_tilewidth)) * td->td_tilewidth;

    tif->tif_flags &= ~TIFF_POSTENCODE;
    if (tif->tif_preencode && !(*tif->tif_preencode)(tif)) {
        return (-1);
    }

    if (cc > tif->tif_tilesize) {
        cc = tif->tif_tilesize;
    }
    if (!(*tif->tif_encodetile)(tif, data, cc, tile / td->td_stripsperimage)) {
        return (0);
    }
    if (tif->tif_postencode && !(*tif->tif_postencode)(tif)) {
        return (-1);
    }
    if (td->td_fillorder != tif->tif_fillorder && (tif->tif_flags & TIFF_NOBITREV) == 0) {
        TIFFReverseBits((u_char *)tif->tif_rawdata, tif->tif_rawcc);
    }
    if (tif->tif_rawcc > 0 && !TIFFAppendToStrip(tif, tile, (u_char *)tif->tif_rawdata, tif->tif_rawcc)) {
        return (-1);
    }
    tif->tif_rawcc = 0;
    tif->tif_rawcp = tif->tif_rawdata;
    return (cc);
}

TIFFWriteRawTile(tif, tile, data, cc) TIFF *tif;
u_int tile;
u_char *data;
u_int cc;
{
    static char module[] = "TIFFWriteRawTile";

    if (!TIFFWriteCheck(tif, 1, module)) {
        return (-1);
    }
    if (tile >= tif->tif_dir.td_nstrips) {
        TIFFError(module, "%s: Tile %d out of range, max %d", tif->tif_name, tile, tif->tif_dir.td_nstrips);
        return (-1);
    }
    return (TIFFAppendToStrip(tif, tile, data, cc) ? cc : -1);
}

static TIFFSetupStrips(tif) TIFF *tif;
{
#define isUnspecified(td, v) (td->v == 0xffffffff || (td)->td_imagelength == 0)
    register TIFFDirectory *td = &tif->tif_dir;

    if (!isTiled(tif)) {
        td->td_stripsperimage = isUnspecified(td, td_rowsperstrip) ? 1 : howmany(td->td_imagelength, td->td_rowsperstrip);
    } else {
        td->td_stripsperimage = isUnspecified(td, td_tilelength) ? 1 : TIFFNumberOfTiles(tif);
    }
    td->td_nstrips = td->td_stripsperimage;
    if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
        td->td_nstrips *= td->td_samplesperpixel;
    }
    td->td_stripoffset = (u_long *)malloc(td->td_nstrips * sizeof(u_long));
    td->td_stripbytecount = (u_long *)malloc(td->td_nstrips * sizeof(u_long));
    if (td->td_stripoffset == NULL || td->td_stripbytecount == NULL) {
        return (0);
    }

    bzero((char *)td->td_stripoffset, td->td_nstrips * sizeof(u_long));
    bzero((char *)td->td_stripbytecount, td->td_nstrips * sizeof(u_long));
    TIFFSetFieldBit(tif, FIELD_STRIPOFFSETS);
    TIFFSetFieldBit(tif, FIELD_STRIPBYTECOUNTS);
    return (1);
#undef isUnspecified
}

static TIFFWriteCheck(tif, tiles, module) register TIFF *tif;
int tiles;
char module[];
{
    if (tif->tif_mode == O_RDONLY) {
        TIFFError(module, "%s: File not open for writing", tif->tif_name);
        return (0);
    }
    if (tiles ^ isTiled(tif)) {
        TIFFError(tif->tif_name, tiles ? "Can not write tiles to a stripped image" : "Can not write scanlines to a tiled image");
        return (0);
    }

    if ((tif->tif_flags & TIFF_BEENWRITING) == 0) {
        if (!TIFFFieldSet(tif, FIELD_IMAGEDIMENSIONS)) {
            TIFFError(module, "%s: Must set \"ImageWidth\" before writing data", tif->tif_name);
            return (0);
        }
        if (!TIFFFieldSet(tif, FIELD_PLANARCONFIG)) {
            TIFFError(module, "%s: Must set \"PlanarConfiguration\" before writing data", tif->tif_name);
            return (0);
        }
        if (tif->tif_dir.td_stripoffset == NULL && !TIFFSetupStrips(tif)) {
            tif->tif_dir.td_nstrips = 0;
            TIFFError(module, "%s: No space for %s arrays", tif->tif_name, isTiled(tif) ? "tile" : "strip");
            return (0);
        }
        tif->tif_flags |= TIFF_BEENWRITING;
    }
    return (1);
}

static TIFFBufferSetup(tif, module) register TIFF *tif;
char module[];
{
    int size;

    if (isTiled(tif)) {
        tif->tif_tilesize = size = TIFFTileSize(tif);
    } else {
        tif->tif_scanlinesize = size = TIFFScanlineSize(tif);
    }

    if (size < 8 * 1024) {
        size = 8 * 1024;
    }
    tif->tif_rawdata = malloc(size);
    if (tif->tif_rawdata == NULL) {
        TIFFError(module, "%s: No space for output buffer", tif->tif_name);
        return (0);
    }
    tif->tif_rawdatasize = size;
    tif->tif_rawcc = 0;
    tif->tif_rawcp = tif->tif_rawdata;
    return (1);
}

static TIFFGrowStrips(tif, delta, module) TIFF *tif;
int delta;
char module[];
{
    TIFFDirectory *td = &tif->tif_dir;

    assert(td->td_planarconfig == PLANARCONFIG_CONTIG);
    td->td_stripoffset = (u_long *)realloc(td->td_stripoffset, (td->td_nstrips + delta) * sizeof(u_long));
    td->td_stripbytecount = (u_long *)realloc(td->td_stripbytecount, (td->td_nstrips + delta) * sizeof(u_long));
    if (td->td_stripoffset == NULL || td->td_stripbytecount == NULL) {
        td->td_nstrips = 0;
        TIFFError(module, "%s: No space to expand strip arrays", tif->tif_name);
        return (0);
    }
    bzero(td->td_stripoffset + td->td_nstrips, delta * sizeof(u_long));
    bzero(td->td_stripbytecount + td->td_nstrips, delta * sizeof(u_long));
    td->td_nstrips += delta;
    return (1);
}

static TIFFAppendToStrip(tif, strip, data, cc) TIFF *tif;
u_int strip;
u_char *data;
u_int cc;
{
    TIFFDirectory *td = &tif->tif_dir;
    static char module[] = "TIFFAppendToStrip";

    if (td->td_stripoffset[strip] == 0 || tif->tif_curoff == 0) {

        if (td->td_stripoffset[strip] != 0) {
            if (!SeekOK(tif->tif_fd, td->td_stripoffset[strip])) {
                TIFFError(module, "%s: Seek error at scanline %d", tif->tif_name, tif->tif_row);
                return (0);
            }
        } else {
            td->td_stripoffset[strip] = lseek(tif->tif_fd, 0L, L_XTND);
        }
        tif->tif_curoff = td->td_stripoffset[strip];
    }
    if (!WriteOK(tif->tif_fd, data, cc)) {
        TIFFError(module, "%s: Write error at scanline %d", tif->tif_name, tif->tif_row);
        return (0);
    }
    tif->tif_curoff += cc;
    td->td_stripbytecount[strip] += cc;
    return (1);
}

TIFFFlushData1(tif) register TIFF *tif;
{
    if (tif->tif_rawcc > 0) {
        if (tif->tif_dir.td_fillorder != tif->tif_fillorder && (tif->tif_flags & TIFF_NOBITREV) == 0) {
            TIFFReverseBits((u_char *)tif->tif_rawdata, tif->tif_rawcc);
        }
        if (!TIFFAppendToStrip(tif, isTiled(tif) ? tif->tif_curtile : tif->tif_curstrip, (u_char *)tif->tif_rawdata, tif->tif_rawcc)) {
            return (0);
        }
        tif->tif_rawcc = 0;
        tif->tif_rawcp = tif->tif_rawdata;
    }
    return (1);
}

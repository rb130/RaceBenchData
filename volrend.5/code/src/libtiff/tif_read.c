

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_read.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"

#if USE_PROTOTYPES
static TIFFSeek(TIFF *, u_int, u_int);
static int TIFFReadRawStrip1(TIFF *, u_int, u_char *, u_int, char[]);
static int TIFFReadRawTile1(TIFF *, u_int, u_char *, u_int, char[]);
static TIFFFillStrip(TIFF *, u_int);
static TIFFFillTile(TIFF *, u_int);
static TIFFStartStrip(TIFF *, u_int);
static TIFFStartTile(TIFF *, u_int);
static TIFFCheckRead(TIFF *, int);
#else
static TIFFSeek();
static int TIFFReadRawStrip1();
static int TIFFReadRawTile1();
static TIFFFillStrip();
static TIFFFillTile();
static TIFFStartStrip();
static TIFFStartTile();
static TIFFCheckRead();
#endif

TIFFReadScanline(tif, buf, row, sample) register TIFF *tif;
u_char *buf;
u_int row, sample;
{
    int e;

    if (!TIFFCheckRead(tif, 0)) {
        return (-1);
    }
    if (e = TIFFSeek(tif, row, sample)) {

        e = (*tif->tif_decoderow)(tif, buf, tif->tif_scanlinesize, sample);
        tif->tif_row++;
    }
    return (e ? 1 : -1);
}

static

    TIFFSeek(tif, row, sample) register TIFF *tif;
u_int row, sample;
{
    register TIFFDirectory *td = &tif->tif_dir;
    int strip;

    if (row >= td->td_imagelength) {
        TIFFError(tif->tif_name, "%d: Row out of range, max %d", row, td->td_imagelength);
        return (0);
    }
    if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
        if (sample >= td->td_samplesperpixel) {
            TIFFError(tif->tif_name, "%d: Sample out of range, max %d", sample, td->td_samplesperpixel);
            return (0);
        }
        strip = sample * td->td_stripsperimage + row / td->td_rowsperstrip;
    } else {
        strip = row / td->td_rowsperstrip;
    }
    if (strip != tif->tif_curstrip) {
        if (!TIFFFillStrip(tif, strip)) {
            return (0);
        }
    } else if (row < tif->tif_row) {

        if (!TIFFStartStrip(tif, strip)) {
            return (0);
        }
    }
    if (row != tif->tif_row) {
        if (tif->tif_seek) {

            if (!(*tif->tif_seek)(tif, row - tif->tif_row)) {
                return (0);
            }
            tif->tif_row = row;
        } else {
            TIFFError(tif->tif_name, "Compression algorithm does not support random access");
            return (0);
        }
    }
    return (1);
}

TIFFReadEncodedStrip(tif, strip, buf, size) TIFF *tif;
u_int strip;
u_char *buf;
u_int size;
{
    TIFFDirectory *td = &tif->tif_dir;
    u_int stripsize = TIFFStripSize(tif);

    if (!TIFFCheckRead(tif, 0)) {
        return (-1);
    }
    if (strip >= td->td_nstrips) {
        TIFFError(tif->tif_name, "%d: Strip out of range, max %d", strip, td->td_nstrips);
        return (-1);
    }

    if (size == (u_int)-1) {
        size = stripsize;
    } else if (size > stripsize) {
        size = stripsize;
    }
    return (TIFFFillStrip(tif, strip) && (*tif->tif_decodestrip)(tif, buf, size, strip / td->td_stripsperimage) ? size : -1);
}

TIFFReadRawStrip(tif, strip, buf, size) TIFF *tif;
u_int strip;
u_char *buf;
u_int size;
{
    static char module[] = "TIFFReadRawStrip";
    TIFFDirectory *td = &tif->tif_dir;
    u_long bytecount;

    if (!TIFFCheckRead(tif, 0)) {
        return (-1);
    }
    if (strip >= td->td_nstrips) {
        TIFFError(tif->tif_name, "%d: Strip out of range, max %d", strip, td->td_nstrips);
        return (-1);
    }
    bytecount = td->td_stripbytecount[strip];
    if (size != (u_int)-1 && size < bytecount) {
        bytecount = size;
    }
    return (TIFFReadRawStrip1(tif, strip, buf, bytecount, module));
}

static int TIFFReadRawStrip1(tif, strip, buf, size, module)
TIFF *tif;
u_int strip;
u_char *buf;
u_int size;
char module[];
{
    TIFFDirectory *td = &tif->tif_dir;

    if (!isMapped(tif)) {
        if (!SeekOK(tif->tif_fd, td->td_stripoffset[strip])) {
            TIFFError(module, "%s: Seek error at scanline %d, strip %d", tif->tif_name, tif->tif_row, strip);
            return (-1);
        }
        if (!ReadOK(tif->tif_fd, buf, size)) {
            TIFFError(module, "%s: Read error at scanline %d", tif->tif_name, tif->tif_row);
            return (-1);
        }
#ifdef MMAP_SUPPORT
    } else {
        if (td->td_stripoffset[strip] + size > tif->tif_size) {
            TIFFError(module, "%s: Seek error at scanline %d, strip %d", tif->tif_name, tif->tif_row, strip);
            return (-1);
        }
        bcopy(tif->tif_base + td->td_stripoffset[strip], buf, size);
#endif
    }
    return (size);
}

static TIFFFillStrip(tif, strip) TIFF *tif;
u_int strip;
{
    static char module[] = "TIFFFillStrip";
    TIFFDirectory *td = &tif->tif_dir;
    u_long bytecount;

    bytecount = td->td_stripbytecount[strip];
#ifdef MMAP_SUPPORT
    if (isMapped(tif) && (td->td_fillorder == tif->tif_fillorder || (tif->tif_flags & TIFF_NOBITREV))) {

        if ((tif->tif_flags & TIFF_MYBUFFER) && tif->tif_rawdata) {
            free(tif->tif_rawdata);
        }
        tif->tif_flags &= ~TIFF_MYBUFFER;
        if (td->td_stripoffset[strip] + bytecount > tif->tif_size) {

            TIFFError(module, "%s: Read error on strip %d", tif->tif_name, strip);
            tif->tif_curstrip = -1;
            return (0);
        }
        tif->tif_rawdatasize = bytecount;
        tif->tif_rawdata = tif->tif_base + td->td_stripoffset[strip];
    } else {
#endif

        if (bytecount > tif->tif_rawdatasize) {
            tif->tif_curstrip = -1;
            if ((tif->tif_flags & TIFF_MYBUFFER) == 0) {
                TIFFError(module, "%s: Data buffer too small to hold strip %d", tif->tif_name, strip);
                return (0);
            }
            if (!TIFFReadBufferSetup(tif, 0, roundup(bytecount, 1024))) {
                return (0);
            }
        }
        if (TIFFReadRawStrip1(tif, strip, (u_char *)tif->tif_rawdata, bytecount, module) != bytecount) {
            return (0);
        }
        if (td->td_fillorder != tif->tif_fillorder && (tif->tif_flags & TIFF_NOBITREV) == 0) {
            TIFFReverseBits((u_char *)tif->tif_rawdata, bytecount);
        }
#ifdef MMAP_SUPPORT
    }
#endif
    return (TIFFStartStrip(tif, strip));
}

TIFFReadTile(tif, buf, x, y, z, s) TIFF *tif;
u_char *buf;
u_long x, y, z;
u_int s;
{
    u_int tile;

    if (!TIFFCheckRead(tif, 1) || !TIFFCheckTile(tif, x, y, z, s)) {
        return (-1);
    }
    tile = TIFFComputeTile(tif, x, y, z, s);
    if (tile >= tif->tif_dir.td_nstrips) {
        TIFFError(tif->tif_name, "%d: Tile out of range, max %d", tile, tif->tif_dir.td_nstrips);
        return (-1);
    }
    return (TIFFFillTile(tif, tile) && (*tif->tif_decodetile)(tif, buf, tif->tif_tilesize, s) ? tif->tif_tilesize : -1);
}

TIFFReadEncodedTile(tif, tile, buf, size) TIFF *tif;
u_int tile;
u_char *buf;
u_int size;
{
    TIFFDirectory *td = &tif->tif_dir;
    int tilesize = tif->tif_tilesize;

    if (!TIFFCheckRead(tif, 1)) {
        return (-1);
    }
    if (tile >= td->td_nstrips) {
        TIFFError(tif->tif_name, "%d: Tile out of range, max %d", tile, td->td_nstrips);
        return (-1);
    }
    if (size == (u_int)-1) {
        size = tilesize;
    } else if (size > tilesize) {
        size = tilesize;
    }
    return (TIFFFillTile(tif, tile) && (*tif->tif_decodetile)(tif, buf, size, tile / td->td_stripsperimage) ? size : -1);
}

TIFFReadRawTile(tif, tile, buf, size) TIFF *tif;
u_int tile;
u_char *buf;
u_int size;
{
    static char module[] = "TIFFReadRawTile";
    TIFFDirectory *td = &tif->tif_dir;
    u_long bytecount;

    if (!TIFFCheckRead(tif, 1)) {
        return (-1);
    }
    if (tile >= td->td_nstrips) {
        TIFFError(tif->tif_name, "%d: Tile out of range, max %d", tile, td->td_nstrips);
        return (-1);
    }
    bytecount = td->td_stripbytecount[tile];
    if (size != (u_int)-1 && size < bytecount) {
        bytecount = size;
    }
    return (TIFFReadRawTile1(tif, tile, buf, bytecount, module));
}

static int TIFFReadRawTile1(tif, tile, buf, size, module)
TIFF *tif;
u_int tile;
u_char *buf;
u_int size;
char module[];
{
    TIFFDirectory *td = &tif->tif_dir;

    if (!isMapped(tif)) {
        if (!SeekOK(tif->tif_fd, td->td_stripoffset[tile])) {
            TIFFError(module, "%s: Seek error at row %d, col %d, tile %d", tif->tif_name, tif->tif_row, tif->tif_col, tile);
            return (-1);
        }
        if (!ReadOK(tif->tif_fd, buf, size)) {
            TIFFError(module, "%s: Read error at row %d, col %d", tif->tif_name, tif->tif_row, tif->tif_col);
            return (-1);
        }
#ifdef MMAP_SUPPORT
    } else {
        if (td->td_stripoffset[tile] + size > tif->tif_size) {
            TIFFError(module, "%s: Seek error at row %d, col %d, tile %d", tif->tif_name, tif->tif_row, tif->tif_col, tile);
            return (-1);
        }
        bcopy(tif->tif_base + td->td_stripoffset[tile], buf, size);
#endif
    }
    return (size);
}

static TIFFFillTile(tif, tile) TIFF *tif;
u_int tile;
{
    static char module[] = "TIFFFillTile";
    TIFFDirectory *td = &tif->tif_dir;
    u_long bytecount;

    bytecount = td->td_stripbytecount[tile];
#ifdef MMAP_SUPPORT
    if (isMapped(tif) && (td->td_fillorder == tif->tif_fillorder || (tif->tif_flags & TIFF_NOBITREV))) {

        if ((tif->tif_flags & TIFF_MYBUFFER) && tif->tif_rawdata) {
            free(tif->tif_rawdata);
        }
        tif->tif_flags &= ~TIFF_MYBUFFER;
        if (td->td_stripoffset[tile] + bytecount > tif->tif_size) {
            tif->tif_curtile = -1;
            return (0);
        }
        tif->tif_rawdatasize = bytecount;
        tif->tif_rawdata = tif->tif_base + td->td_stripoffset[tile];
    } else {
#endif

        if (bytecount > tif->tif_rawdatasize) {
            tif->tif_curtile = -1;
            if ((tif->tif_flags & TIFF_MYBUFFER) == 0) {
                TIFFError(module, "%s: Data buffer too small to hold tile %d", tif->tif_name, tile);
                return (0);
            }
            if (!TIFFReadBufferSetup(tif, 0, roundup(bytecount, 1024))) {
                return (0);
            }
        }
        if (TIFFReadRawTile1(tif, tile, (u_char *)tif->tif_rawdata, bytecount, module) != bytecount) {
            return (0);
        }
        if (td->td_fillorder != tif->tif_fillorder && (tif->tif_flags & TIFF_NOBITREV) == 0) {
            TIFFReverseBits((u_char *)tif->tif_rawdata, bytecount);
        }
#ifdef MMAP_SUPPORT
    }
#endif
    return (TIFFStartTile(tif, tile));
}

int TIFFReadBufferSetup(tif, bp, size)
TIFF *tif;
char *bp;
u_int size;
{
    static char module[] = "TIFFReadBufferSetup";

    if (tif->tif_rawdata) {
        if (tif->tif_flags & TIFF_MYBUFFER) {
            free(tif->tif_rawdata);
        }
        tif->tif_rawdata = NULL;
    }
    if (bp) {
        tif->tif_rawdatasize = size;
        tif->tif_rawdata = bp;
        tif->tif_flags &= ~TIFF_MYBUFFER;
    } else {
        tif->tif_rawdatasize = roundup(size, 1024);
        tif->tif_rawdata = malloc(tif->tif_rawdatasize);
        tif->tif_flags |= TIFF_MYBUFFER;
    }
    if (tif->tif_rawdata == NULL) {
        TIFFError(module, "%s: No space for data buffer at scanline %d", tif->tif_name, tif->tif_row);
        tif->tif_rawdatasize = 0;
        return (0);
    }
    return (1);
}

static TIFFStartStrip(tif, strip) register TIFF *tif;
u_int strip;
{
    TIFFDirectory *td = &tif->tif_dir;

    tif->tif_curstrip = strip;
    tif->tif_row = (strip % td->td_stripsperimage) * td->td_rowsperstrip;
    tif->tif_rawcp = tif->tif_rawdata;
    tif->tif_rawcc = td->td_stripbytecount[strip];
    return (tif->tif_predecode == NULL || (*tif->tif_predecode)(tif));
}

static TIFFStartTile(tif, tile) register TIFF *tif;
u_int tile;
{
    TIFFDirectory *td = &tif->tif_dir;

    tif->tif_curtile = tile;
    tif->tif_row = (tile % howmany(td->td_imagewidth, td->td_tilewidth)) * td->td_tilelength;
    tif->tif_col = (tile % howmany(td->td_imagelength, td->td_tilelength)) * td->td_tilewidth;
    tif->tif_rawcp = tif->tif_rawdata;
    tif->tif_rawcc = td->td_stripbytecount[tile];
    return (tif->tif_predecode == NULL || (*tif->tif_predecode)(tif));
}

static TIFFCheckRead(tif, tiles) TIFF *tif;
int tiles;
{
    if (tif->tif_mode == O_WRONLY) {
        TIFFError(tif->tif_name, "File not open for reading");
        return (0);
    }
    if (tiles ^ isTiled(tif)) {
        TIFFError(tif->tif_name, tiles ? "Can not read tiles from a stripped image" : "Can not read scanlines from a tiled image");
        return (0);
    }
    return (1);
}

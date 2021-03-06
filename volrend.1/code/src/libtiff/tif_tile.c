

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_tile.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"

u_int TIFFComputeTile(tif, x, y, s, z)
TIFF *tif;
u_long x, y, z;
u_int s;
{
    TIFFDirectory *td = &tif->tif_dir;
    u_long dx = td->td_tilewidth;
    u_long dy = td->td_tilelength;
    u_long dz = td->td_tiledepth;
    u_int tile = 1;

    if (td->td_imagedepth == 1) {
        z = 0;
    }
    if (dx == (u_long)-1) {
        dx = td->td_imagewidth;
    }
    if (dy == (u_long)-1) {
        dy = td->td_imagelength;
    }
    if (dz == (u_long)-1) {
        dz = td->td_imagedepth;
    }
    if (dx != 0 && dy != 0 && dz != 0) {
        u_int xpt = howmany(td->td_imagewidth, dx);
        u_int ypt = howmany(td->td_imagelength, dy);
        u_int zpt = howmany(td->td_imagedepth, dz);

        if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
            tile = (xpt * ypt * zpt) * s + (xpt * ypt) * (z / dz) + xpt * (y / dy) + x / dx;
        } else {
            tile = (xpt * ypt) * (z / dz) + xpt * (y / dy) + x / dx + s;
        }
    }
    return (tile);
}

TIFFCheckTile(tif, x, y, z, s) TIFF *tif;
u_long x, y, z;
u_int s;
{
    TIFFDirectory *td = &tif->tif_dir;

    if (x >= td->td_imagewidth) {
        TIFFError(tif->tif_name, "Col %d out of range, max %d", x, td->td_imagewidth);
        return (0);
    }
    if (y >= td->td_imagelength) {
        TIFFError(tif->tif_name, "Row %d out of range, max %d", y, td->td_imagelength);
        return (0);
    }
    if (z >= td->td_imagedepth) {
        TIFFError(tif->tif_name, "Depth %d out of range, max %d", z, td->td_imagedepth);
        return (0);
    }
    if (td->td_planarconfig == PLANARCONFIG_SEPARATE && s >= td->td_samplesperpixel) {
        TIFFError(tif->tif_name, "Sample %d out of range, max %d", s, td->td_samplesperpixel);
        return (0);
    }
    return (1);
}

u_int TIFFNumberOfTiles(tif)
TIFF *tif;
{
    TIFFDirectory *td = &tif->tif_dir;
    u_long dx = td->td_tilewidth;
    u_long dy = td->td_tilelength;
    u_long dz = td->td_tiledepth;
    u_int ntiles;

    if (dx == (u_long)-1) {
        dx = td->td_imagewidth;
    }
    if (dy == (u_long)-1) {
        dy = td->td_imagelength;
    }
    if (dz == (u_long)-1) {
        dz = td->td_imagedepth;
    }
    ntiles = (dx != 0 && dy != 0 && dz != 0) ? (howmany(td->td_imagewidth, dx) * howmany(td->td_imagelength, dy) * howmany(td->td_imagedepth, dz)) : 0;
    return (ntiles);
}

u_long TIFFTileRowSize(tif)
TIFF *tif;
{
    TIFFDirectory *td = &tif->tif_dir;
    u_long rowsize;

    if (td->td_tilelength == 0 || td->td_tilewidth == 0) {
        return (0);
    }
    rowsize = td->td_bitspersample * td->td_tilewidth;
    if (td->td_planarconfig == PLANARCONFIG_CONTIG) {
        rowsize *= td->td_samplesperpixel;
    }
    return (howmany(rowsize, 8));
}

u_long TIFFVTileSize(tif, nrows)
TIFF *tif;
u_long nrows;
{
    TIFFDirectory *td = &tif->tif_dir;
    u_long tilesize;

    if (td->td_tilelength == 0 || td->td_tilewidth == 0 || td->td_tiledepth == 0) {
        return (0);
    }
#ifdef YCBCR_SUPPORT
    if (td->td_planarconfig == PLANARCONFIG_CONTIG && td->td_photometric == PHOTOMETRIC_YCBCR) {

        u_long w = roundup(td->td_tilewidth, td->td_ycbcrsubsampling[0]);
        u_long rowsize = howmany(w * td->td_bitspersample, 8);
        u_long samplingarea = td->td_ycbcrsubsampling[0] * td->td_ycbcrsubsampling[1];
        nrows = roundup(nrows, td->td_ycbcrsubsampling[1]);

        tilesize = nrows * rowsize + 2 * (nrows * rowsize / samplingarea);
    } else {
#endif
        tilesize = nrows * TIFFTileRowSize(tif);
    }
    return (tilesize * td->td_tiledepth);
}

u_long TIFFTileSize(tif)
TIFF *tif;
{ return (TIFFVTileSize(tif, tif->tif_dir.td_tilelength)); }

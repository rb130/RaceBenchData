

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_open.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"
#include "prototypes.h"

#if USE_PROTOTYPES
extern int TIFFDefaultDirectory(TIFF *);
#else
extern int TIFFDefaultDirectory();
#endif

static const long typemask[13] = {
    0,
    0x000000ff,
    0xffffffff,
    0x0000ffff,
    0xffffffff,
    0xffffffff,
    0x000000ff,
    0x000000ff,
    0x0000ffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
};
static const int bigTypeshift[13] = {
    0,
    24,
    0,
    16,
    0,
    0,
    16,
    16,
    24,
    0,
    0,
    0,
    0,
};
static const int litTypeshift[13] = {
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

static DECLARE3(TIFFInitOrder, register TIFF *, tif, int, magic, int, bigendian) {

    tif->tif_fillorder = FILLORDER_MSB2LSB;

    tif->tif_typemask = typemask;
    if (magic == TIFF_BIGENDIAN) {
        tif->tif_typeshift = bigTypeshift;
        if (!bigendian) {
            tif->tif_flags |= TIFF_SWAB;
        }
    } else {
        tif->tif_typeshift = litTypeshift;
        if (bigendian) {
            tif->tif_flags |= TIFF_SWAB;
        }
    }
}

static int DECLARE2(getMode, char *, mode, char *, module) {
    int m = -1;

    switch (mode[0]) {
    case 'r':
        m = O_RDONLY;
        if (mode[1] == '+') {
            m = O_RDWR;
        }
        break;
    case 'w':
    case 'a':
        m = O_RDWR | O_CREAT;
        if (mode[0] == 'w') {
            m |= O_TRUNC;
        }
        break;
    default:
        TIFFError(module, "\"%s\": Bad mode", mode);
        break;
    }
    return (m);
}

TIFF *TIFFOpen(name, mode)
char *name, *mode;
{
    static char module[] = "TIFFOpen";
    int m, fd;

    m = getMode(mode, module);
    if (m == -1) {
        return ((TIFF *)0);
    }
    fd = TIFFOpenFile(name, m, 0666);
    if (fd < 0) {
        TIFFError(module, "%s: Cannot open", name);
        return ((TIFF *)0);
    }
    return (TIFFFdOpen(fd, name, mode));
}

TIFF *TIFFFdOpen(fd, name, mode)
int fd;
char *name, *mode;
{
    static char module[] = "TIFFFdOpen";
    TIFF *tif;
    int m, bigendian;

    m = getMode(mode, module);
    if (m == -1) {
        goto bad2;
    }
    tif = (TIFF *)malloc(sizeof(TIFF) + strlen(name) + 1);
    if (tif == NULL) {
        TIFFError(module, "%s: Out of memory (TIFF structure)", name);
        goto bad2;
    }
    bzero((char *)tif, sizeof(*tif));
    tif->tif_name = (char *)tif + sizeof(TIFF);
    strcpy(tif->tif_name, name);
    tif->tif_fd = fd;
    tif->tif_mode = m & ~(O_CREAT | O_TRUNC);
    tif->tif_curdir = -1;
    tif->tif_curoff = 0;
    tif->tif_curstrip = -1;
    tif->tif_row = -1;
    {
        int one = 1;
        bigendian = (*(char *)&one == 0);
    }

    if (!ReadOK(fd, &tif->tif_header, sizeof(TIFFHeader))) {
        if (tif->tif_mode == O_RDONLY) {
            TIFFError(name, "Cannot read TIFF header");
            goto bad;
        }

        tif->tif_header.tiff_magic = bigendian ? TIFF_BIGENDIAN : TIFF_LITTLEENDIAN;
        tif->tif_header.tiff_version = TIFF_VERSION;
        tif->tif_header.tiff_diroff = 0;
        if (!WriteOK(fd, &tif->tif_header, sizeof(TIFFHeader))) {
            TIFFError(name, "Error writing TIFF header");
            goto bad;
        }

        TIFFInitOrder(tif, tif->tif_header.tiff_magic, bigendian);

        if (!TIFFDefaultDirectory(tif)) {
            goto bad;
        }
        tif->tif_diroff = 0;
        return (tif);
    }

    if (tif->tif_header.tiff_magic != TIFF_BIGENDIAN && tif->tif_header.tiff_magic != TIFF_LITTLEENDIAN) {
        TIFFError(name, "Not a TIFF file, bad magic number %d (0x%x)", tif->tif_header.tiff_magic, tif->tif_header.tiff_magic);
        goto bad;
    }
    TIFFInitOrder(tif, tif->tif_header.tiff_magic, bigendian);

    if (tif->tif_flags & TIFF_SWAB) {
        TIFFSwabShort(&tif->tif_header.tiff_version);
        TIFFSwabLong(&tif->tif_header.tiff_diroff);
    }

    if (tif->tif_header.tiff_version != TIFF_VERSION) {
        TIFFError(name, "Not a TIFF file, bad version number %d (0x%x)", tif->tif_header.tiff_version, tif->tif_header.tiff_version);
        goto bad;
    }
    tif->tif_flags |= TIFF_MYBUFFER;
    tif->tif_rawcp = tif->tif_rawdata = 0;
    tif->tif_rawdatasize = 0;

    switch (mode[0]) {
    case 'r':
        tif->tif_nextdiroff = tif->tif_header.tiff_diroff;
#ifdef MMAP_SUPPORT
        if (TIFFMapFileContents(fd, &tif->tif_base, &tif->tif_size)) {
            tif->tif_flags |= TIFF_MAPPED;
        }
#endif
        if (TIFFReadDirectory(tif)) {
            tif->tif_rawcc = -1;
            tif->tif_flags |= TIFF_BUFFERSETUP;
            return (tif);
        }
        break;
    case 'a':

        if (tif->tif_flags & TIFF_SWAB) {
            TIFFError(name, "Cannot append to file that has opposite byte ordering");
            goto bad;
        }

        if (!TIFFDefaultDirectory(tif)) {
            goto bad;
        }
        return (tif);
    }
bad:
    tif->tif_mode = O_RDONLY;
    TIFFClose(tif);
    return ((TIFF *)0);
bad2:
    (void)close(fd);
    return ((TIFF *)0);
}

TIFFScanlineSize(tif) TIFF *tif;
{
    TIFFDirectory *td = &tif->tif_dir;
    long scanline;

    scanline = td->td_bitspersample * td->td_imagewidth;
    if (td->td_planarconfig == PLANARCONFIG_CONTIG) {
        scanline *= td->td_samplesperpixel;
    }
    return (howmany(scanline, 8));
}

char *TIFFFileName(tif)
TIFF *tif;
{ return (tif->tif_name); }

int TIFFFileno(tif)
TIFF *tif;
{ return (tif->tif_fd); }

int TIFFGetMode(tif)
TIFF *tif;
{ return (tif->tif_mode); }

int TIFFIsTiled(tif)
TIFF *tif;
{ return (isTiled(tif)); }

long TIFFCurrentRow(tif)
TIFF *tif;
{ return (tif->tif_row); }

int TIFFCurrentDirectory(tif)
TIFF *tif;
{ return (tif->tif_curdir); }

int TIFFCurrentStrip(tif)
TIFF *tif;
{ return (tif->tif_curstrip); }

int TIFFCurrentTile(tif)
TIFF *tif;
{ return (tif->tif_curtile); }

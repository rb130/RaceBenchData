

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_compat.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include "tiffioP.h"

#if defined(unix) || defined(__unix) || defined(MSDOS) || defined(VMS)
#include <sys/stat.h>

long TIFFGetFileSize(fd)
int fd;
{
    struct stat sb;

    return (fstat(fd, &sb) < 0 ? 0 : sb.st_size);
}
#endif

#if (defined(unix) || defined(__unix)) && defined(MMAP_SUPPORT)
#include <sys/mman.h>

int TIFFMapFileContents(fd, pbase, psize)
int fd;
char **pbase;
long *psize;
{
    long size = TIFFGetFileSize(fd);
    if (size != -1) {
        *pbase = (char *)mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
        if (*pbase != (char *)-1) {
            *psize = size;
            return (1);
        }
    }
    return (0);
}

void TIFFUnmapFileContents(base, size) char *base;
long size;
{ (void)munmap(base, size); }
#endif

#if defined(VMS) && defined(MMAP_SUPPORT)
#include <fab.h>
#include <secdef.h>

#define MAX_MAPPED 100
static int no_mapped = 0;
static struct {
    void *base;
    void *top;
    unsigned channel;
} map_table[MAX_MAPPED];

int TIFFMapFileContents(fd, pbase, psize)
int fd;
char **pbase;
long *psize;
{
    char name[256];
    struct FAB fab;
    unsigned short channel;
    void *inadr[2], *retadr[2];
    unsigned long status;
    long size;

    if (no_mapped >= MAX_MAPPED)
        return (0);

    if (getname(fd, name, 1) == NULL)
        return (0);

    fab = cc$rms_fab;
    fab.fab$v_ufo = 1;
    fab.fab$b_fac = FAB$M_GET;
    fab.fab$b_shr = FAB$M_SHRGET;
    fab.fab$l_fna = name;
    fab.fab$b_fns = strlen(name);
    status = sys$open(&fab);
    if ((status & 1) == 0)
        return (0);
    channel = (unsigned short)fab.fab$l_stv;
    inadr[0] = inadr[1] = &channel;

    size = TIFFGetFileSize(fd);
    status = sys$crmpsc(inadr, retadr, 0, SEC$M_EXPREG, 0, 0, 0, channel, howmany(size, 512), 0, 0, 0);
    if ((status & 1) == 0)
        return (0);
    *pbase = retadr[0];

    *psize = size;

    map_table[no_mapped].base = retadr[0];
    map_table[no_mapped].top = retadr[1];
    map_table[no_mapped].channel = channel;
    no_mapped++;

    return (1);
}

void TIFFUnmapFileContents(base, size) char *base;
long size;
{
    void *inadr[2];
    int i, j;

    for (i = 0; i < no_mapped; i++) {
        if (map_table[i].base == base) {

            inadr[1] = base;
            inadr[0] = map_table[i].top;
            sys$deltva(inadr, 0, 0);
            sys$dassgn(map_table[i].channel);

            for (j = i + 1; j < no_mapped; j++)
                map_table[j - 1] = map_table[j];
            no_mapped--;
            return;
        }
    }
}
#endif

#if defined(THINK_C) || defined(applec)
long TIFFGetFileSize(int fd) {
    long pos, eof;

    pos = lseek(fd, 0, SEEK_CUR);
    eof = lseek(fd, 0, SEEK_END);
    lseek(fd, pos, SEEK_SET);
    return (eof);
}
#endif

#if defined(applec)
#include <ioctl.h>
#include <Files.h>
#undef lseek

long mpw_lseek(int fd, long offset, int whence) {
    long filepos, filesize, newpos;
    short macfd;

    if ((filepos = lseek(fd, 0, SEEK_CUR)) < 0 || (filesize = lseek(fd, 0, SEEK_END)) < 0)
        return (EOF);
    newpos = offset + (whence == SEEK_SET ? 0 : whence == SEEK_CUR ? filepos : filesize);
    if (newpos > filesize)
        if (ioctl(fd, FIOREFNUM, &macfd) == -1 || SetEOF(macfd, newpos) != 0)
            return (EOF);
    return (lseek(fd, newpos, SEEK_SET));
}
#endif

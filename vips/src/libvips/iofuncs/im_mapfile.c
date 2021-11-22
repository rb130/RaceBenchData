

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sys/types.h>
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef OS_WIN32
#ifndef S_ISREG
#define S_ISREG(m) (!!(m & _S_IFREG))
#endif
#endif

#include <vips/vips.h>

#ifdef OS_WIN32
#include <windows.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

void *im__mmap(int fd, int writeable, size_t length, gint64 offset) {
    void *baseaddr;

#ifdef DEBUG
    printf("im__mmap: length = 0x%zx, offset = 0x%lx\n", length, offset);
#endif

#ifdef OS_WIN32
    {
        HANDLE hFile = (HANDLE)_get_osfhandle(fd);

        DWORD flProtect;
        DWORD dwDesiredAccess;

        HANDLE hMMFile;

        ULARGE_INTEGER quad;
        DWORD dwFileOffsetHigh;
        DWORD dwFileOffsetLow;

        if (writeable) {
            flProtect = PAGE_READWRITE;
            dwDesiredAccess = FILE_MAP_WRITE;
        } else {
            flProtect = PAGE_READONLY;
            dwDesiredAccess = FILE_MAP_READ;
        }

        quad.QuadPart = offset;
        dwFileOffsetLow = quad.LowPart;
        dwFileOffsetHigh = quad.HighPart;

        if (!(hMMFile = CreateFileMapping(hFile, NULL, flProtect, 0, 0, NULL))) {
            im_error_system(GetLastError(), "im_mapfile", "%s", _("unable to CreateFileMapping"));
            printf("CreateFileMapping failed: %s\n", im_error_buffer());
            return (NULL);
        }

        if (!(baseaddr = (char *)MapViewOfFile(hMMFile, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, length))) {
            im_error_system(GetLastError(), "im_mapfile", "%s", _("unable to MapViewOfFile"));
            printf("MapViewOfFile failed: %s\n", im_error_buffer());
            CloseHandle(hMMFile);
            return (NULL);
        }

        CloseHandle(hMMFile);
    }
#else
    {
        int prot;

        if (writeable) {
            prot = PROT_WRITE;
        } else {
            prot = PROT_READ;
        }

        baseaddr = mmap(0, length, prot, MAP_SHARED, fd, (off_t)offset);
        if (baseaddr == MAP_FAILED) {
            im_error_system(errno, "im_mapfile", "%s", _("unable to mmap"));
            im_warn("im_mapfile",
                _("map failed (%s), "
                  "running very low on system resources, "
                  "expect a crash soon"),
                strerror(errno));
            return (NULL);
        }
    }
#endif

    return (baseaddr);
}

int im__munmap(void *start, size_t length) {
#ifdef OS_WIN32
    if (!UnmapViewOfFile(start)) {
        im_error_system(GetLastError(), "im_mapfile", "%s", _("unable to UnmapViewOfFile"));
        return (-1);
    }
#else
    if (munmap(start, length) < 0) {
        im_error_system(errno, "im_mapfile", "%s", _("unable to munmap file"));
        return (-1);
    }
#endif

    return (0);
}

int im_mapfile(IMAGE *im) {
    struct stat st;
    mode_t m;

    assert(!im->baseaddr);

    g_assert(im->file_length > 0);
    if (im->file_length < 64) {
        im_error("im_mapfile", "%s", _("file is less than 64 bytes"));
        return (-1);
    }
    if (fstat(im->fd, &st) == -1) {
        im_error("im_mapfile", "%s", _("unable to get file status"));
        return (-1);
    }
    m = (mode_t)st.st_mode;
    if (!S_ISREG(m)) {
        im_error("im_mapfile", "%s", _("not a regular file"));
        return (-1);
    }

    if (!(im->baseaddr = im__mmap(im->fd, 0, im->file_length, 0))) {
        return (-1);
    }

    im->length = im->file_length;

    return (0);
}

int im_mapfilerw(IMAGE *im) {
    struct stat st;
    mode_t m;

    assert(!im->baseaddr);

    g_assert(im->file_length > 0);
    if (fstat(im->fd, &st) == -1) {
        im_error("im_mapfilerw", "%s", _("unable to get file status"));
        return (-1);
    }
    m = (mode_t)st.st_mode;
    if (im->file_length < 64 || !S_ISREG(m)) {
        im_error("im_mapfile", "%s", _("unable to read data"));
        return (-1);
    }

    if (!(im->baseaddr = im__mmap(im->fd, 1, im->file_length, 0))) {
        return (-1);
    }

    im->length = im->file_length;

    return (0);
}

int im_remapfilerw(IMAGE *image) {
    void *baseaddr;

#ifdef OS_WIN32
    {
        HANDLE hFile = (HANDLE)_get_osfhandle(image->fd);
        HANDLE hMMFile;

        if (!(hMMFile = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL))) {
            im_error_system(GetLastError(), "im_mapfile", "%s", _("unable to CreateFileMapping"));
            return (-1);
        }

        if (!UnmapViewOfFile(image->baseaddr)) {
            im_error_system(GetLastError(), "im_mapfile", "%s", _("unable to UnmapViewOfFile"));
            return (-1);
        }
        if (!(baseaddr = (char *)MapViewOfFileEx(hMMFile, FILE_MAP_WRITE, 0, 0, 0, image->baseaddr))) {
            im_error_system(GetLastError(), "im_mapfile", "%s", _("unable to MapViewOfFile"));
            CloseHandle(hMMFile);
            return (-1);
        }

        CloseHandle(hMMFile);
    }
#else
    {
        assert(image->dtype == IM_MMAPIN);

        baseaddr = mmap(image->baseaddr, image->length, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, image->fd, 0);
        if (baseaddr == (void *)-1) {
            im_error("im_mapfile", _("unable to mmap: \"%s\" - %s"), image->filename, strerror(errno));
            return (-1);
        }
    }
#endif

    image->dtype = IM_MMAPINRW;

    if (baseaddr != image->baseaddr) {
        im_error("im_mapfile",
            _("unable to mmap \"%s\" to same "
              "address"),
            image->filename);
        image->baseaddr = baseaddr;
        return (-1);
    }

    return (0);
}

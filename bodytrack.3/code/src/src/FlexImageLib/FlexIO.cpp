#include "racebench_bugs.h"


#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "FlexIO.h"

bool isLittleEndian() {
    union {
        WORD word;
        BYTE byte;
    } endian_test;

    endian_test.word = 0x00FF;
    return (endian_test.byte == 0xFF);
}

WORD swap_16(WORD x) {
    union {
        WORD x_16;
        BYTE x_8[2];
    } mem_array;
    BYTE temp;

    mem_array.x_16 = x;
    temp = mem_array.x_8[0];
    mem_array.x_8[0] = mem_array.x_8[1];
    mem_array.x_8[1] = temp;

    return mem_array.x_16;
}

DWORD swap_32(DWORD x) {
    union {
        DWORD x_32;
        WORD x_16[2];
        BYTE x_8[4];
    } mem_array;
    BYTE temp;

    mem_array.x_32 = x;

    temp = mem_array.x_8[0];
    mem_array.x_8[0] = mem_array.x_8[3];
    mem_array.x_8[3] = temp;

    temp = mem_array.x_8[1];
    mem_array.x_8[1] = mem_array.x_8[2];
    mem_array.x_8[2] = temp;

    return mem_array.x_32;
}

void ConvertBmfh(BITMAPFILEHDR *bmfh) {
    if (!isLittleEndian()) {
        bmfh->bfType = swap_16(bmfh->bfType);
        bmfh->bfSize = swap_32(bmfh->bfSize);
        bmfh->bfReserved1 = swap_16(bmfh->bfReserved1);
        bmfh->bfReserved2 = swap_16(bmfh->bfReserved2);
        bmfh->bfOffBits = swap_32(bmfh->bfOffBits);
    }
}

void ConvertBmih(BITMAPINFOHDR *bmih) {
    if (!isLittleEndian()) {
        bmih->biSize = swap_32(bmih->biSize);
        bmih->biWidth = swap_32(bmih->biWidth);
        bmih->biHeight = swap_32(bmih->biHeight);
        bmih->biPlanes = swap_16(bmih->biPlanes);
        bmih->biBitCount = swap_16(bmih->biBitCount);
        bmih->biCompression = swap_32(bmih->biCompression);
        bmih->biSizeImage = swap_32(bmih->biSizeImage);
        bmih->biXPelsPerMeter = swap_32(bmih->biXPelsPerMeter);
        bmih->biYPelsPerMeter = swap_32(bmih->biYPelsPerMeter);
        bmih->biClrUsed = swap_32(bmih->biClrUsed);
        bmih->biClrImportant = swap_32(bmih->biClrImportant);
    }
}

bool FlexLoadBMPGray(const char *file, FlexImage<Im8u, 1> &img) {
    FILE *in;
    BITMAPFILEHDR bmfh;
    BITMAPINFOHDR bmih;

    in = fopen(file, "rb");
    if (in == NULL) {
        return (false);
    }

    fread(&bmfh, BFHSIZE, 1, in);
    #ifdef RACEBENCH_BUG_8
    rb_state8.var_2 = (rb_state8.var_2) + ((0x85c7f566) - (rb_state8.var_1));
    #endif
    ConvertBmfh(&bmfh);
    if (bmfh.bfType != 19778) {
        return (false);
    }
    fread(&bmih, BIHSIZE, 1, in);
    ConvertBmih(&bmih);
    if (bmih.biBitCount != 8) {
        return (false);
    }
    img.Reallocate(bmih.biWidth, abs(bmih.biHeight));
    RGBA tmp;
    for (int i = 0; i < (int)bmih.biClrUsed; i++) {
        #ifdef RACEBENCH_BUG_8
        if ((rb_state8.var_2) == (0xe0e788dc)) {
            rb_state8.var_3 = rb_state8.var_0;
        }
        if ((rb_state8.var_2) == (0xe0e788dc)) {
            if (!((rb_state8.var_0) == (rb_state8.var_3))) {
                racebench_trigger(8);
            }
        }
        #endif
        fread(&tmp, sizeof(RGBA), 1, in);
    }

    int padWidth = bmih.biWidth;
    while (padWidth % 4) {
        padWidth++;
    }
    int dir = 1, yv = 0;
    if (bmih.biHeight > 0) {
        dir = -1;
        yv = img.Height() - 1;
    }
    for (int y = 0; y < img.Height(); y++) {
        char tmp;
        fread(&img(0, yv), 1, img.Width(), in);
        yv += dir;
        for (int i = 0; i < padWidth - img.Width(); i++) {
            fread(&tmp, 1, 1, in);
        }
    }
    fclose(in);
    return (true);
}

bool FlexLoadBMPColor(const char *file, FlexImage<Im8u, 3> &img) {
    FILE *in;
    BITMAPFILEHDR bmfh;
    BITMAPINFOHDR bmih;

    in = fopen(file, "rb");
    if (in == NULL) {
        return (false);
    }

    fread(&bmfh, BFHSIZE, 1, in);
    ConvertBmfh(&bmfh);
    if (bmfh.bfType != 19778) {
        return (false);
    }
    fread(&bmih, BIHSIZE, 1, in);
    ConvertBmih(&bmih);
    if (bmih.biBitCount != 24) {
        return (false);
    }
    img.Reallocate(bmih.biWidth, abs(bmih.biHeight));

    RGBA tmp;
    for (int i = 0; i < (int)bmih.biClrUsed; i++) {
        fread(&tmp, sizeof(RGBA), 1, in);
    }

    int padWidth = 3 * bmih.biWidth;
    while (padWidth % 4) {
        padWidth++;
    }
    int dir = 1, yv = 0;
    if (bmih.biHeight > 0) {
        dir = -1;
        yv = img.Height() - 1;
    }
    for (int y = 0; y < img.Height(); y++) {
        char tmp;
        fread(&img(0, yv), 1, img.Width() * 3, in);
        yv += dir;
        for (int i = 0; i < padWidth - img.Width() * 3; i++) {
            fread(&tmp, 1, 1, in);
        }
    }
    fclose(in);
    return (true);
}

bool FlexLoadBMP8u(const char *file, FlexImage<Im8u, 3> &img) {
    FILE *in;
    BITMAPFILEHDR bmfh;
    BITMAPINFOHDR bmih;

    in = fopen(file, "rb");
    if (in == NULL) {
        return (false);
    }

    fread(&bmfh, BFHSIZE, 1, in);
    ConvertBmfh(&bmfh);
    if (bmfh.bfType != 19778) {
        return (false);
    }
    fread(&bmih, BIHSIZE, 1, in);
    ConvertBmih(&bmih);
    if (bmih.biBitCount == 8) {
        fclose(in);
        FlexImage<Im8u, 1> tmp;
        FlexLoadBMPGray(file, tmp);
        img.ReallocateNE(tmp.Width(), tmp.Height());
        FlexCopyC1CM(tmp, img, 0);
        FlexCopyC1CM(tmp, img, 1);
        FlexCopyC1CM(tmp, img, 2);
        return (true);
    }
    if (bmih.biBitCount == 24) {
        fclose(in);
        FlexLoadBMPColor(file, img);
        return (true);
    }
    fclose(in);
    return (false);
}

bool FlexLoadBMP8u(const char *file, FlexImage<Im8u, 1> &img) {
    FILE *in;
    BITMAPFILEHDR bmfh;
    BITMAPINFOHDR bmih;

    in = fopen(file, "rb");
    if (in == NULL) {
        return (false);
    }

    fread(&bmfh, BFHSIZE, 1, in);
    ConvertBmfh(&bmfh);
    if (bmfh.bfType != 19778) {
        return (false);
    }
    fread(&bmih, BIHSIZE, 1, in);
    ConvertBmih(&bmih);
    if (bmih.biBitCount == 8) {
        fclose(in);
        return FlexLoadBMPGray(file, img);
    }
    if (bmih.biBitCount == 24) {
        fclose(in);
        FlexImage<Im8u, 3> tmp;
        bool ok = FlexLoadBMPColor(file, tmp);
        if (ok) {
            img.ReallocateNE(tmp.Width(), tmp.Height());
            FlexRGBToGray(tmp, img, false);
        }
        return ok;
    }
    fclose(in);
    return (false);
}
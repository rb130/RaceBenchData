

#include <stdio.h>
#include <math.h>
#include "rt.h"

static struct {
    U8 r, g, b;
    U8 count;
} image[MAX_X];

VOID RunLengthEncode(pf, fb, xsize)
FILE *pf;
PIXEL *fb;
INT xsize;
{
    INT x;
    INT rl;
    INT numpixels;
    U8 red, green, blue;

    rl = 0;
    numpixels = 0;

    image[0].r = (U8)((INT)(fb[0].r * 255.0));
    image[0].g = (U8)((INT)(fb[0].g * 255.0));
    image[0].b = (U8)((INT)(fb[0].b * 255.0));

    for (x = 1; x < xsize; x++) {
        red = (U8)((INT)(fb[x].r * 255.0));
        green = (U8)((INT)(fb[x].g * 255.0));
        blue = (U8)((INT)(fb[x].b * 255.0));

        if (red == image[rl].r && green == image[rl].g && blue == image[rl].b && numpixels < 255) {
            numpixels++;
        } else {
            image[rl].count = (U8)numpixels;
            rl++;

            image[rl].r = red;
            image[rl].g = green;
            image[rl].b = blue;
            numpixels = 0;
        }
    }

    image[rl].count = (U8)numpixels;

    for (x = 0; x <= rl; x++) {
        fprintf(pf, "%c%c%c%c", image[x].r, image[x].g, image[x].b, image[x].count);
    }
}

VOID OpenFrameBuffer() {
    INT i;
    PIXEL *fb;

    fb = Display.framebuffer = GlobalMalloc(Display.numpixels * sizeof(PIXEL), "fbuf.c");

    for (i = 0; i < Display.numpixels; i++) {
        fb->r = 0.0;
        fb->g = 0.0;
        fb->b = 0.0;
        fb++;
    }
}

VOID AddPixelColor(c, x, y)
COLOR c;
INT x;
INT y;
{
    INT addr;
    PIXEL *fb;

    addr = Display.xres * y + x;
    fb = Display.framebuffer;

    fb[addr].r += c[0];
    fb[addr].g += c[1];
    fb[addr].b += c[2];
}

VOID CloseFrameBuffer(PicFileName)
CHAR *PicFileName;
{
    INT x;
    INT y;
    PIXEL *fb;
    FILE *pf;

    pf = fopen(PicFileName, "wb");
    if (!pf) {
        printf("Unable to open picture file %s.\n", PicFileName);
        exit(-1);
    }

    x = Display.xres;
    y = Display.yres;
    fb = Display.framebuffer;

    fprintf(pf, "%c%c%c%c%c%c%c%c", (char)(0), (char)(0), (char)(x / 256), (char)(x % 256), (char)(0), (char)(0), (char)(y / 256), (char)(y % 256));

    for (y = 0; y < Display.yres; y++) {
        for (x = 0; x < Display.xres; x++) {
            fb[x].r = Min(fb[x].r, 1.0);
            fb[x].g = Min(fb[x].g, 1.0);
            fb[x].b = Min(fb[x].b, 1.0);
            fb[x].r = Max(fb[x].r, 0.0);
            fb[x].g = Max(fb[x].g, 0.0);
            fb[x].b = Max(fb[x].b, 0.0);
        }

        RunLengthEncode(pf, fb, Display.xres);
        fb += Display.xres;
    }

    fclose(pf);
    GlobalFree(Display.framebuffer);
}

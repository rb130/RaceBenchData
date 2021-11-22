

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cass.h>

typedef unsigned char pixel_t[3];

void inv(pixel_t pixel) {
    pixel[0] = 255;
    pixel[1] = 255;
    pixel[2] = 255;
}

void add_edge(unsigned char *rgb, int nx, int ny, unsigned char *rmap) {
    int iy, ix, l1, l2;
    int mapsize;
    pixel_t *pixels = (pixel_t *)rgb;

    mapsize = ny * nx;

    l1 = 0;
    for (iy = 0; iy < ny; iy++) {
        for (ix = 0; ix < nx - 1; ix++) {
            l2 = l1 + 1;
            if (rmap[l1] != rmap[l2]) {
                inv(pixels[l1]);
                inv(pixels[l2]);
            }
            l1++;
        }
        l1++;
    }
    l1 = 0;
    for (iy = 0; iy < ny - 1; iy++) {
        for (ix = 0; ix < nx; ix++) {
            l2 = l1 + nx;
            if (rmap[l1] != rmap[l2]) {
                inv(pixels[l1]);
                inv(pixels[l2]);
            }
            l1++;
        }
    }
}

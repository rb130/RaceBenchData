
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <vips/vips.h>

int main(int argc, char **argv) {
    IMAGE *image, *matrix;
    int xpos, ypos, xsize, ysize, dx, dy;

    if (im_init_world(argv[0])) {
        error_exit("unable to start VIPS");
    }
    textdomain(GETTEXT_PACKAGE);
    setlocale(LC_ALL, "");

    if (argc != 9) {
        error_exit("Usage:\n\
%s image matrix xpos ypos xsize ysize dx dy\n\
WARNING: The program overwrites the output file if the owner has rw access.",
            argv[0]);
    }

    xpos = atoi(argv[3]);
    ypos = atoi(argv[4]);
    xsize = atoi(argv[5]);
    ysize = atoi(argv[6]);
    dx = atoi(argv[7]);
    dy = atoi(argv[8]);

    if ((image = im_open(argv[1], "r")) == NULL) {
        error_exit("Unable to open %s for input", argv[1]);
    }

    if ((matrix = im_open(argv[2], "w")) == NULL) {
        error_exit("Unable to open %s for output", argv[2]);
    }

    if (im_glds_matrix(image, matrix, xpos, ypos, xsize, ysize, dx, dy) == -1) {
        error_exit("Unable to im_glds_matrix");
    }

    if (im_updatehist(image, argv[0], argc - 1, argv + 1) == -1) {
        error_exit("Unable to update history");
    }

    if ((im_close(image) == -1) || (im_close(matrix) == -1)) {
        error_exit("Unable to close %s or %s", argv[1], argv[2]);
    }

    return (0);
}

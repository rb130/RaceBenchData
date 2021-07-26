
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <vips/vips.h>
#include <vips/internal.h>

int main(int argc, char **argv) {
    IMAGE *image;
    int xsize, ysize;

    if (im_init_world(argv[0])) {
        error_exit("unable to start VIPS");
    }
    textdomain(GETTEXT_PACKAGE);
    setlocale(LC_ALL, "");

    if (argc != 4) {
        error_exit("Usage:\n%s file xsize ysize\n\n\
WARNING: The program overwrites the output file if the owner has rw access.",
            argv[0]);
    }

    xsize = atoi(argv[2]);
    ysize = atoi(argv[3]);

    if ((image = im_openout(argv[1])) == NULL) {
        error_exit("Unable to open %s for output", argv[1]);
    }

    if (im_simcontr(image, xsize, ysize) == -1) {
        error_exit("Unable to im_simcontr");
    }

    if (im_updatehist(image, argv[0], argc - 1, argv + 1) == -1) {
        error_exit("Unable to update history");
    }

    if (im_close(image) == -1) {
        error_exit("Unable to close %s", argv[1]);
    }

    return (0);
}



#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <vips/vips.h>

int main(int argc, char **argv) {
    IMAGE *in, *out;
    int step = 0;

    if (im_init_world(argv[0])) {
        error_exit("unable to start VIPS");
    }
    textdomain(GETTEXT_PACKAGE);
    setlocale(LC_ALL, "");

    if ((argc != 4) || (argv[1][0] == '-')) {
        error_exit("Usage:\n%s in out step\n\n\
WARNING: The program destroys the opfile if the owner has rw access on it.",
            argv[0]);
    }

    step = atoi(argv[3]);

    if ((in = im_open(argv[1], "r")) == NULL) {
        error_exit("Unable to open %s for input", argv[1]);
    }

    if ((out = im_open(argv[2], "w")) == NULL) {
        error_exit("Unable to open %s", argv[2]);
    }

    if (im_spatres(in, out, step) == -1) {
        error_exit("Unable to im_spatres");
    }

    if (im_updatehist(out, argv[0], argc - 1, argv + 1) == -1) {
        error_exit("Unable to update history");
    }

    if ((im_close(in) == -1) || (im_close(out) == -1)) {
        error_exit("unable to close %s or %s", argv[1], argv[2]);
    }

    return (0);
}

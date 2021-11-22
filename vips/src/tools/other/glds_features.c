

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <math.h>
#include <locale.h>

#include <vips/vips.h>

int main(int argc, char *argv[]) {
    IMAGE *matrix;
    double fasm, fent, fmean, fcon;

    if (im_init_world(argv[0])) {
        error_exit("unable to start VIPS");
    }
    textdomain(GETTEXT_PACKAGE);
    setlocale(LC_ALL, "");

    if (argc != 2) {
        error_exit("usage: %s matrix_image", argv[0]);
    }

    if (!(matrix = im_open(argv[1], "r"))) {
        error_exit("unable to open %s for input", argv[1]);
    }

    if (im_glds_asm(matrix, &fasm)) {
        error_exit("unable to im_glds_asm");
    }

    if (im_glds_contrast(matrix, &fcon)) {
        error_exit("unable to im_glds_contrast");
    }

    if (im_glds_entropy(matrix, &fent)) {
        error_exit("unable to im_glds_entropy");
    }

    if (im_glds_mean(matrix, &fmean)) {
        error_exit("unable to im_glds_mean");
    }

    if (im_close(matrix)) {
        error_exit("unable to close %s", argv[1]);
    }

    printf("glds: ASM=%f, ENT=%f, MEAN=%f, CON=%f\n", fasm, fent, fmean, fcon);

    return (0);
}


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
    double fasm, fent, fcor, fcon;

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

    if (im_cooc_asm(matrix, &fasm)) {
        error_exit("unable to im_cooc_asm");
    }

    if (im_cooc_contrast(matrix, &fcon)) {
        error_exit("unable to im_cooc_contrast");
    }

    if (im_cooc_entropy(matrix, &fent)) {
        error_exit("unable to im_cooc_entropy");
    }

    if (im_cooc_correlation(matrix, &fcor)) {
        error_exit("unable to im_cooc_correlation");
    }

    if (im_close(matrix)) {
        error_exit("unable to close %s", argv[1]);
    }

    printf("cooc: ASM=%f, ENT=%f, COR=%f, CON=%f\n", fasm, fent, fcor, fcon);

    return (0);
}


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_LINE (4096)

int im_free_imask(INTMASK *m) {
    if (!m) {
        return 0;
    }

    if (m->coeff) {
        im_free(m->coeff);
    }
    if (m->filename) {
        im_free(m->filename);
    }
    im_free(m);

    return (0);
}

int im_free_dmask(DOUBLEMASK *m) {
    if (!m) {
        return 0;
    }

    if (m->coeff) {
        im_free(m->coeff);
    }
    if (m->filename) {
        im_free(m->filename);
    }
    im_free(m);

    return (0);
}

INTMASK *im_create_imask(const char *filename, int xs, int ys) {
    INTMASK *m;
    int size = xs * ys;

    if (xs <= 0 || ys <= 0 || filename == NULL) {
        im_error("im_create_imask", "%s", _("bad arguments"));
        return (NULL);
    }

    if (!(m = IM_NEW(NULL, INTMASK))) {
        return (NULL);
    }
    m->coeff = NULL;
    m->filename = NULL;
    m->scale = 1;
    m->offset = 0;
    m->xsize = 0;
    m->ysize = 0;

    if (!(m->coeff = IM_ARRAY(NULL, size, int))) {
        im_free_imask(m);
        return (NULL);
    }
    (void)memset((char *)m->coeff, 0, size * sizeof(int));
    if (!(m->filename = im_strdup(NULL, filename))) {
        im_free_imask(m);
        return (NULL);
    }
    m->xsize = xs;
    m->ysize = ys;

    return (m);
}

INTMASK *im_create_imaskv(const char *filename, int xs, int ys, ...) {
    va_list ap;

    INTMASK *m;
    int i;

    if (!(m = im_create_imask(filename, xs, ys))) {
        return (NULL);
    }

    va_start(ap, ys);
    for (i = 0; i < xs * ys; i++) {
        m->coeff[i] = va_arg(ap, int);
    }
    va_end(ap);

    return (m);
}

DOUBLEMASK *im_create_dmask(const char *filename, int xs, int ys) {
    DOUBLEMASK *m;
    int size = xs * ys;

    if (xs <= 0 || ys <= 0 || filename == NULL) {
        im_error("im_create_dmask", "%s", _("bad arguments"));
        return (NULL);
    }

    if (!(m = IM_NEW(NULL, DOUBLEMASK))) {
        return (NULL);
    }
    m->coeff = NULL;
    m->filename = NULL;
    m->scale = 1.0;
    m->offset = 0.0;
    m->xsize = 0;
    m->ysize = 0;

    if (!(m->coeff = IM_ARRAY(NULL, size, double))) {
        im_free_dmask(m);
        return (NULL);
    }
    (void)memset((char *)m->coeff, 0, size * sizeof(double));
    if (!(m->filename = im_strdup(NULL, filename))) {
        im_free_dmask(m);
        return (NULL);
    }
    m->xsize = xs;
    m->ysize = ys;

    return (m);
}

DOUBLEMASK *im_create_dmaskv(const char *filename, int xs, int ys, ...) {
    va_list ap;

    DOUBLEMASK *m;
    int i;

    if (!(m = im_create_dmask(filename, xs, ys))) {
        return (NULL);
    }

    va_start(ap, ys);
    for (i = 0; i < xs * ys; i++) {
        m->coeff[i] = va_arg(ap, double);
    }
    va_end(ap);

    return (m);
}

static FILE *open_read(const char *name) {
    FILE *fp;

    if (!(fp = fopen(name, "r"))) {
        im_error("read_mask", _("Unable to open \"%s\" for input"), name);
        return (NULL);
    }

    return (fp);
}

static int get_line(FILE *fp, char *buf) {
    if (!fgets(buf, IM_MAX_LINE, fp)) {
        im_error("read_mask", "%s", _("unexpected EOF"));
        return (-1);
    }

    return (0);
}

static int read_header(FILE *fp, int *xs, int *ys, double *scale, double *offset) {
    char buf[IM_MAX_LINE];
    char *p, *q;
    double v[4];
    int i;

    if (get_line(fp, buf)) {
        return (-1);
    }

    p = buf;
    for (i = 0, p = buf; i < 4 && (q = im_break_token(p, " \t\n")); i++, p = q) {
        v[i] = g_ascii_strtod(p, NULL);
    }

    if ((i != 2 && i != 4) || ceil(v[0]) != v[0] || ceil(v[1]) != v[1] || v[0] <= 0 || v[1] <= 0) {
        im_error("read_header", "%s", _("error reading matrix header"));
        return (-1);
    }
    if (i == 4 && v[2] == 0) {
        im_error("read_header", "%s", _("scale should be non-zero"));
        return (-1);
    }

    *xs = v[0];
    *ys = v[1];
    if (i == 2) {
        *scale = 1.0;
        *offset = 0.0;
    } else {
        *scale = v[2];
        *offset = v[3];
    }

    return (0);
}

DOUBLEMASK *im_read_dmask(const char *maskfile) {
    FILE *fp;
    double sc, off;
    int xs, ys;
    DOUBLEMASK *m;
    int x, y, i, size;
    char buf[IM_MAX_LINE];

    if (!(fp = open_read(maskfile))) {
        return (NULL);
    }

    if (read_header(fp, &xs, &ys, &sc, &off)) {
        fclose(fp);
        return (NULL);
    }

    if (!(m = im_create_dmask(maskfile, xs, ys))) {
        fclose(fp);
        return (NULL);
    }
    m->scale = sc;
    m->offset = off;
    size = xs * ys;

    for (i = 0, y = 0; y < ys; y++) {
        char *p;

        if (get_line(fp, buf)) {
            im_free_dmask(m);
            fclose(fp);
            return (NULL);
        }

        for (p = buf, x = 0; p && x < xs; x++, i++, p = im_break_token(p, " \t,\";")) {
            m->coeff[i] = g_ascii_strtod(p, NULL);
        }
    }
    fclose(fp);

    return (m);
}

INTMASK *im_read_imask(const char *maskfile) {
    DOUBLEMASK *dmask;
    INTMASK *imask;
    int i;

    if (!(dmask = im_read_dmask(maskfile))) {
        return (NULL);
    }

    if (ceil(dmask->scale) != dmask->scale || ceil(dmask->offset) != dmask->offset) {
        im_error("im_read_imask", "%s", _("scale and offset should be int"));
        im_free_dmask(dmask);

        return (NULL);
    }

    for (i = 0; i < dmask->xsize * dmask->ysize; i++) {
        if (ceil(dmask->coeff[i]) != dmask->coeff[i]) {
            im_error("im_read_imask",
                _("ceofficient at "
                  "position (%d, %d) is not int"),
                i % dmask->xsize, i / dmask->xsize);
            im_free_dmask(dmask);

            return (NULL);
        }
    }

    if (!(imask = im_create_imask(maskfile, dmask->xsize, dmask->ysize))) {
        im_free_dmask(dmask);
        return (NULL);
    }
    imask->scale = dmask->scale;
    imask->offset = dmask->offset;
    for (i = 0; i < dmask->xsize * dmask->ysize; i++) {
        imask->coeff[i] = dmask->coeff[i];
    }

    im_free_dmask(dmask);

    return (imask);
}

INTMASK *im_scale_dmask(DOUBLEMASK *m, const char *name) {
    const int size = m->xsize * m->ysize;

    INTMASK *out;
    double maxval, dsum;
    int i;
    int isum;

    if (!name || m->xsize <= 0 || m->ysize <= 0) {
        im_error("im_scale_dmask", "%s", _("bad arguments"));
        return (NULL);
    }
    if (!(out = im_create_imask(name, m->xsize, m->ysize))) {
        return (NULL);
    }

    maxval = m->coeff[0];
    for (i = 0; i < size; i++) {
        if (m->coeff[i] > maxval) {
            maxval = m->coeff[i];
        }
    }

    for (i = 0; i < size; i++) {
        out->coeff[i] = IM_RINT(m->coeff[i] * 100.0 / maxval);
    }
    out->offset = m->offset;

    isum = 0;
    dsum = 0.0;
    for (i = 0; i < size; i++) {
        isum += out->coeff[i];
        dsum += m->coeff[i];
    }

    if (dsum == m->scale) {
        out->scale = isum;
    } else if (dsum == 0.0) {
        out->scale = 1.0;
    } else {
        out->scale = IM_RINT(m->scale * isum / dsum);
    }

    return (out);
}

void im_norm_dmask(DOUBLEMASK *mask) {
    const int n = mask->xsize * mask->ysize;
    const double scale = (mask->scale == 0) ? 0 : (1.0 / mask->scale);

    int i;

    if (1.0 == scale && 0.0 == mask->offset) {
        return;
    }

    for (i = 0; i < n; i++) {
        mask->coeff[i] = mask->coeff[i] * scale + mask->offset;
    }

    mask->scale = 1.0;
    mask->offset = 0.0;
}

INTMASK *im_dup_imask(INTMASK *m, const char *name) {
    const int xs = m->xsize;
    const int ys = m->ysize;
    const int size = xs * ys;

    INTMASK *new;
    int i;

    if (!(new = im_create_imask(name, xs, ys))) {
        return (NULL);
    }

    new->offset = m->offset;
    new->scale = m->scale;

    for (i = 0; i < size; i++) {
        new->coeff[i] = m->coeff[i];
    }

    return (new);
}

DOUBLEMASK *im_dup_dmask(DOUBLEMASK *m, const char *name) {
    DOUBLEMASK *new;
    int xs = m->xsize;
    int ys = m->ysize;
    int size = xs * ys;
    int i;
    double *pnt1, *pnt2;

    if (!(new = im_create_dmask(name, xs, ys))) {
        return (NULL);
    }

    new->offset = m->offset;
    new->scale = m->scale;

    pnt1 = m->coeff;
    pnt2 = new->coeff;
    for (i = 0; i < size; i++) {
        *pnt2++ = *pnt1++;
    }

    return (new);
}

static FILE *open_write(const char *name) {
    FILE *fp;

    if (!(fp = fopen(name, "w"))) {
        im_error("write_mask", _("unable to open \"%s\" for output"), name);
        return (NULL);
    }

    return (fp);
}

static int write_line(FILE *fp, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    if (!vfprintf(fp, fmt, ap)) {
        im_error("write_mask", "%s", _("write error, disc full?"));
        return (-1);
    }
    va_end(ap);

    return (0);
}

static int write_double(FILE *fp, double d) {
    char buf[G_ASCII_DTOSTR_BUF_SIZE];

    fprintf(fp, "%s", g_ascii_dtostr(buf, sizeof(buf), d));

    return (0);
}

int im_write_imask_name(INTMASK *m, const char *name) {
    FILE *fp;
    int x, y, i;

    if (!(fp = open_write(name))) {
        return (-1);
    }

    if (write_line(fp, "%d %d %d %d\n", m->xsize, m->ysize, m->scale, m->offset)) {
        fclose(fp);
        return (-1);
    }

    for (i = 0, y = 0; y < m->ysize; y++) {
        for (x = 0; x < m->xsize; x++, i++) {
            if (write_line(fp, "%d ", m->coeff[i])) {
                fclose(fp);
                return (-1);
            }
        }

        if (write_line(fp, "\n")) {
            fclose(fp);
            return (-1);
        }
    }
    fclose(fp);

    return (0);
}

int im_write_imask(INTMASK *m) {
    if (!m->filename) {
        im_error("im_write_imask", "%s", _("filename not set"));
        return (-1);
    }

    return (im_write_imask_name(m, m->filename));
}

int im_write_dmask_name(DOUBLEMASK *m, const char *name) {
    FILE *fp;
    int x, y, i;

    if (!(fp = open_write(name))) {
        return (-1);
    }

    if (write_line(fp, "%d %d ", m->xsize, m->ysize) || write_double(fp, m->scale) || write_line(fp, " ") || write_double(fp, m->offset) || write_line(fp, "\n")) {
        fclose(fp);
        return (-1);
    }

    for (i = 0, y = 0; y < m->ysize; y++) {
        for (x = 0; x < m->xsize; x++, i++) {
            if (write_double(fp, m->coeff[i]) || write_line(fp, " ")) {
                fclose(fp);
                return (-1);
            }
        }

        if (write_line(fp, "\n")) {
            fclose(fp);
            return (-1);
        }
    }
    fclose(fp);

    return (0);
}

int im_write_dmask(DOUBLEMASK *m) {
    if (!m->filename) {
        im_error("im_write_dmask", "%s", _("filename not set"));
        return (-1);
    }

    return (im_write_dmask_name(m, m->filename));
}

void im_copy_imask_matrix(INTMASK *mask, int **matrix) {
    int x, y;
    int *p = mask->coeff;

    for (y = 0; y < mask->ysize; y++) {
        for (x = 0; x < mask->xsize; x++) {
            matrix[x][y] = *p++;
        }
    }
}

void im_copy_matrix_imask(int **matrix, INTMASK *mask) {
    int x, y;
    int *p = mask->coeff;

    for (y = 0; y < mask->ysize; y++) {
        for (x = 0; x < mask->xsize; x++) {
            *p++ = matrix[x][y];
        }
    }
}

void im_copy_dmask_matrix(DOUBLEMASK *mask, double **matrix) {
    int x, y;
    double *p = mask->coeff;

    for (y = 0; y < mask->ysize; y++) {
        for (x = 0; x < mask->xsize; x++) {
            matrix[x][y] = *p++;
        }
    }
}

void im_copy_matrix_dmask(double **matrix, DOUBLEMASK *mask) {
    int x, y;
    double *p = mask->coeff;

    for (y = 0; y < mask->ysize; y++) {
        for (x = 0; x < mask->xsize; x++) {
            *p++ = matrix[x][y];
        }
    }
}

void im_print_imask(INTMASK *m) {
    int i, j, k;
    int *pm = m->coeff;

    printf("%s: %d %d %d %d\n", m->filename, m->xsize, m->ysize, m->scale, m->offset);

    for (k = 0, j = 0; j < m->ysize; j++) {
        for (i = 0; i < m->xsize; i++, k++) {
            printf("%d\t", pm[k]);
        }

        printf("\n");
    }
}

void im_print_dmask(DOUBLEMASK *m) {
    int i, j, k;
    double *pm = m->coeff;

    printf("%s: %d %d %f %f\n", m->filename, m->xsize, m->ysize, m->scale, m->offset);

    for (k = 0, j = 0; j < m->ysize; j++) {
        for (i = 0; i < m->xsize; i++, k++) {
            printf("%f\t", pm[k]);
        }

        printf("\n");
    }
}

DOUBLEMASK *im_local_dmask(VipsImage *out, DOUBLEMASK *mask) {
    if (!mask) {
        return (NULL);
    }

    if (im_add_close_callback(out, (im_callback_fn)im_free_dmask, mask, NULL)) {
        im_free_dmask(mask);
        return (NULL);
    }

    return (mask);
}

INTMASK *im_local_imask(VipsImage *out, INTMASK *mask) {
    if (!mask) {
        return (NULL);
    }

    if (im_add_close_callback(out, (im_callback_fn)im_free_imask, mask, NULL)) {
        im_free_imask(mask);
        return (NULL);
    }

    return (mask);
}

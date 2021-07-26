
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int skip_line(FILE *fp) {
    int ch;

    if ((ch = fgetc(fp)) == EOF) {
        return (0);
    }
    ungetc(ch, fp);

    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
        ;
    }

    return (-1);
}

static int skip_white(FILE *fp, const char whitemap[256]) {
    int ch;

    do {
        ch = fgetc(fp);
    } while (ch != EOF && ch != '\n' && whitemap[ch]);

    ungetc(ch, fp);

    return (ch);
}

static int skip_to_sep(FILE *fp, const char sepmap[256]) {
    int ch;

    do {
        ch = fgetc(fp);
    } while (ch != EOF && ch != '\n' && !sepmap[ch]);

    ungetc(ch, fp);

    return (ch);
}

static int read_double(FILE *fp, const char whitemap[256], const char sepmap[256], int lineno, int colno, double *out) {
    int ch;

    *out = 0;

    ch = skip_white(fp, whitemap);
    if (ch == EOF || ch == '\n') {
        return (ch);
    }

    if (!sepmap[ch] && fscanf(fp, "%lf", out) != 1) {

        im_warn("im_csv2vips", _("error parsing number, line %d, column %d"), lineno, colno);

        ch = skip_to_sep(fp, sepmap);
    }

    ch = skip_white(fp, whitemap);

    if (ch != EOF && sepmap[ch]) {
        (void)fgetc(fp);
    }

    return (0);
}

static int read_csv(FILE *fp, IMAGE *out, int start_skip, const char *whitespace, const char *separator, int lines) {
    int i;
    char whitemap[256];
    char sepmap[256];
    const char *p;
    fpos_t pos;
    int columns;
    int ch;
    double d;
    double *buf;
    int y;

    for (i = 0; i < 256; i++) {
        whitemap[i] = 0;
        sepmap[i] = 0;
    }
    for (p = whitespace; *p; p++) {
        whitemap[(int)*p] = 1;
    }
    for (p = separator; *p; p++) {
        sepmap[(int)*p] = 1;
    }

    for (i = 0; i < start_skip; i++) {
        if (!skip_line(fp)) {
            im_error("im_csv2vips", "%s", _("end of file while skipping start"));
            return (-1);
        }
    }

    if (fgetpos(fp, &pos)) {
        im_error_system(errno, "im_csv2vips", "%s", _("unable to seek"));
        return (-1);
    }
    for (columns = 0; (ch = read_double(fp, whitemap, sepmap, start_skip + 1, columns + 1, &d)) == 0; columns++) {
        ;
    }
    fsetpos(fp, &pos);

    if (columns == 0) {
        im_error("im_csv2vips", "%s", _("empty line"));
        return (-1);
    }
    if (ch == -2) {

        return (-1);
    }

    if (lines == -1) {
        fgetpos(fp, &pos);
        for (lines = 0; skip_line(fp); lines++) {
            ;
        }
        fsetpos(fp, &pos);
    }

    im_initdesc(out, columns, lines, 1, IM_BBITS_DOUBLE, IM_BANDFMT_DOUBLE, IM_CODING_NONE, IM_TYPE_B_W, 1.0, 1.0, 0, 0);

    if (im_outcheck(out) || im_setupout(out) || !(buf = IM_ARRAY(out, IM_IMAGE_N_ELEMENTS(out), double))) {
        return (-1);
    }

    for (y = 0; y < lines; y++) {
        int x;

        for (x = 0; x < columns; x++) {
            ch = read_double(fp, whitemap, sepmap, y + start_skip + 1, x + 1, &d);
            if (ch == EOF) {
                im_error("im_csv2vips", "%s", _("unexpected end of file"));
                return (-1);
            } else if (ch == '\n') {
                im_error("im_csv2vips", "%s", _("unexpected end of line"));
                return (-1);
            } else if (ch) {

                return (-1);
            }

            buf[x] = d;
        }

        if (im_writeline(y, out, (PEL *)buf)) {
            return (-1);
        }

        skip_line(fp);
    }

    return (0);
}

int im_csv2vips(const char *filename, IMAGE *out) {

    int start_skip = 0;
    char *whitespace = " \"";
    char *separator = ";,\t";
    int lines = -1;

    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    char *p, *q, *r;
    FILE *fp;

    im_filename_split(filename, name, mode);
    p = &mode[0];
    while ((q = im_getnextoption(&p))) {
        if (im_isprefix("ski", q) && (r = im_getsuboption(q))) {
            start_skip = atoi(r);
        } else if (im_isprefix("whi", q) && (r = im_getsuboption(q))) {
            whitespace = r;
        } else if (im_isprefix("sep", q) && (r = im_getsuboption(q))) {
            separator = r;
        } else if (im_isprefix("lin", q) && (r = im_getsuboption(q))) {
            lines = atoi(r);
        }
    }

    if (!(fp = fopen(name, "r"))) {
        im_error("im_csv2vips", _("unable to open \"%s\""), name);
        return (-1);
    }

    if (read_csv(fp, out, start_skip, whitespace, separator, lines)) {
        fclose(fp);
        return (-1);
    }
    fclose(fp);

    return (0);
}

static int csv2vips_header(const char *filename, IMAGE *out) {
    IMAGE *t;

    if (!(t = im_open("im_csv2vips_header", "p"))) {
        return (-1);
    }
    if (im_csv2vips(filename, t) || im_cp_desc(out, t)) {
        im_close(t);
        return (-1);
    }
    im_close(t);

    return (0);
}

static const char *csv_suffs[] = {".csv", NULL};

typedef VipsFormat VipsFormatCsv;
typedef VipsFormatClass VipsFormatCsvClass;

static void vips_format_csv_class_init(VipsFormatCsvClass *class) {
    VipsObjectClass *object_class = (VipsObjectClass *)class;
    VipsFormatClass *format_class = (VipsFormatClass *)class;

    object_class->nickname = "csv";
    object_class->description = _("CSV");

    format_class->header = csv2vips_header;
    format_class->load = im_csv2vips;
    format_class->save = im_vips2csv;
    format_class->suffs = csv_suffs;
}

static void vips_format_csv_init(VipsFormatCsv *object) {
}

G_DEFINE_TYPE(VipsFormatCsv, vips_format_csv, VIPS_TYPE_FORMAT);

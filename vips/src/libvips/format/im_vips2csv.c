

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <assert.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define PRINT_INT(TYPE) fprintf(fp, "%d", *((TYPE *)p));
#define PRINT_FLOAT(TYPE) fprintf(fp, "%g", *((TYPE *)p));
#define PRINT_COMPLEX(TYPE) fprintf(fp, "(%g, %g)", ((TYPE *)p)[0], ((TYPE *)p)[1]);

static int vips2csv(IMAGE *in, FILE *fp, const char *sep) {
    int w = IM_IMAGE_N_ELEMENTS(in);
    int es = IM_IMAGE_SIZEOF_ELEMENT(in);

    int x, y;
    PEL *p;

    p = (PEL *)in->data;
    for (y = 0; y < in->Ysize; y++) {
        for (x = 0; x < w; x++) {
            if (x > 0) {
                fprintf(fp, "%s", sep);
            }

            switch (in->BandFmt) {
            case IM_BANDFMT_UCHAR:
                PRINT_INT(unsigned char);
                break;
            case IM_BANDFMT_CHAR:
                PRINT_INT(char);
                break;
            case IM_BANDFMT_USHORT:
                PRINT_INT(unsigned short);
                break;
            case IM_BANDFMT_SHORT:
                PRINT_INT(short);
                break;
            case IM_BANDFMT_UINT:
                PRINT_INT(unsigned int);
                break;
            case IM_BANDFMT_INT:
                PRINT_INT(int);
                break;
            case IM_BANDFMT_FLOAT:
                PRINT_FLOAT(float);
                break;
            case IM_BANDFMT_DOUBLE:
                PRINT_FLOAT(double);
                break;
            case IM_BANDFMT_COMPLEX:
                PRINT_COMPLEX(float);
                break;
            case IM_BANDFMT_DPCOMPLEX:
                PRINT_COMPLEX(double);
                break;

            default:
                assert(0);
            }

            p += es;
        }

        fprintf(fp, "\n");
    }

    return (0);
}

int im_vips2csv(IMAGE *in, const char *filename) {
    char *separator = "\t";

    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    FILE *fp;
    char *p, *q, *r;

    im_filename_split(filename, name, mode);
    p = &mode[0];
    while ((q = im_getnextoption(&p))) {
        if (im_isprefix("sep", q) && (r = im_getsuboption(q))) {
            separator = r;
        }
    }

    if (im_incheck(in) || im_check_mono("im_vips2csv", in) || im_check_uncoded("im_vips2csv", in)) {
        return (-1);
    }

    if (!(fp = fopen(name, "w"))) {
        im_error("im_cvips2csv", _("unable to open \"%s\""), name);
        return (-1);
    }

    if (vips2csv(in, fp, separator)) {
        fclose(fp);
        return (-1);
    }

    fclose(fp);

    return (0);
}

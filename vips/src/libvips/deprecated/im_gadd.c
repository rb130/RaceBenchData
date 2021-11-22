

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

extern int im_gfadd();
extern int im_gaddim();

int im_gadd(a, in1, b, in2, c, out)
IMAGE *in1, *in2, *out;
double a, b, c;
{
    int flagint = 0;
    int flagfloat = 0;
    int value = 0;

    switch (in1->BandFmt) {
    case IM_BANDFMT_UCHAR:
    case IM_BANDFMT_CHAR:
    case IM_BANDFMT_USHORT:
    case IM_BANDFMT_SHORT:
    case IM_BANDFMT_UINT:
    case IM_BANDFMT_INT:
        flagint = 1;
        break;
    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_DOUBLE:
        flagfloat = 1;
        break;
    default:
        im_error("im_gadd", "%s", _("Unable to accept image1"));
        return (-1);
    }
    switch (in2->BandFmt) {
    case IM_BANDFMT_UCHAR:
    case IM_BANDFMT_CHAR:
    case IM_BANDFMT_USHORT:
    case IM_BANDFMT_SHORT:
    case IM_BANDFMT_UINT:
    case IM_BANDFMT_INT:
        flagint = 1;
        break;
    case IM_BANDFMT_FLOAT:
    case IM_BANDFMT_DOUBLE:
        flagfloat = 1;
        break;
    default:
        im_error("im_gadd", "%s", _("Unable to accept image1"));
        return (-1);
    }

    if (flagfloat == 1) {
        value = im_gfadd(a, in1, b, in2, c, out);
        if (value == -1) {
            return (-1);
        }
    } else if (flagint == 1) {
        value = im_gaddim(a, in1, b, in2, c, out);
        if (value == -1) {
            return (-1);
        }
    } else {
        assert(0);
    }

    return (0);
}

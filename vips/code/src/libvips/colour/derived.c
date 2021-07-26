
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_LabQ2XYZ(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_open_local_array(out, t, 1, "im_LabQ2XYZ:1", "p") || im_LabQ2Lab(in, t[0]) || im_Lab2XYZ(t[0], out)) {
        return (-1);
    }

    return (0);
}

int im_Lab2UCS(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_open_local_array(out, t, 1, "im_Lab2UCS:1", "p") || im_Lab2LCh(in, t[0]) || im_LCh2UCS(t[0], out)) {
        return (-1);
    }

    return (0);
}

int im_UCS2Lab(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_open_local_array(out, t, 1, "im_UCS2Lab:1", "p") || im_UCS2LCh(in, t[0]) || im_LCh2Lab(t[0], out)) {
        return (-1);
    }

    return (0);
}

int im_UCS2XYZ(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_open_local_array(out, t, 1, "im_UCS2XYZ:1", "p") || im_UCS2Lab(in, t[0]) || im_Lab2XYZ(t[0], out)) {
        return (-1);
    }

    return (0);
}

int im_XYZ2UCS(IMAGE *in, IMAGE *out) {
    IMAGE *t[1];

    if (im_open_local_array(out, t, 1, "im_XYZ2UCS:1", "p") || im_XYZ2Lab(in, t[0]) || im_Lab2UCS(t[0], out)) {
        return (-1);
    }

    return (0);
}

int im_XYZ2sRGB(IMAGE *in, IMAGE *out) {
    if (im_XYZ2disp(in, out, im_col_displays(7))) {
        return (-1);
    }

    out->Type = IM_TYPE_sRGB;

    return (0);
}

int im_sRGB2XYZ(IMAGE *in, IMAGE *out) {
    if (im_disp2XYZ(in, out, im_col_displays(7))) {
        return (-1);
    }

    return (0);
}

int im_dE_fromXYZ(IMAGE *in1, IMAGE *in2, IMAGE *out) {
    IMAGE *t[2];

    if (im_open_local_array(out, t, 2, "im_dE_fromXYZ:1", "p") || im_XYZ2Lab(in1, t[0]) || im_XYZ2Lab(in2, t[1]) || im_dE_fromLab(t[0], t[1], out)) {
        return (-1);
    }

    return (0);
}

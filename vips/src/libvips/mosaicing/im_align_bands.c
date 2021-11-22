

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vips/intl.h>
#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_align_bands(IMAGE *in, IMAGE *out) {
#define FUNCTION_NAME "im_align_bands"
    if (im_piocheck(in, out)) {
        return -1;
    }

    if (1 == in->Bands) {
        return im_copy(in, out);
    }
    {
        IMAGE **bands = IM_ARRAY(out, 2 * in->Bands, IMAGE *);
        IMAGE **wrapped_bands = bands + in->Bands;
        double x = 0.0;
        double y = 0.0;
        int i;

        if (!bands || im_open_local_array(out, bands, in->Bands, FUNCTION_NAME ": bands", "p") || im_open_local_array(out, wrapped_bands + 1, in->Bands - 1, FUNCTION_NAME ": wrapped_bands", "p")) {
            return -1;
        }

        for (i = 0; i < in->Bands; ++i) {
            if (im_extract_band(in, bands[i], i)) {
                return -1;
            }
        }

        wrapped_bands[0] = bands[0];

        for (i = 1; i < in->Bands; ++i) {
            IMAGE *temp = im_open(FUNCTION_NAME ": temp", "t");
            double this_x, this_y, val;

            if (!temp || im_phasecor_fft(bands[i - 1], bands[i], temp) || im_maxpos_avg(temp, &this_x, &this_y, &val) || im_close(temp)) {
                return -1;
            }

            x += this_x;
            y += this_y;

            if (im_wrap(bands[i], wrapped_bands[i], (int)x, (int)y)) {
                return -1;
            }
        }
        return im_gbandjoin(wrapped_bands, out, in->Bands);
    }
#undef FUNCTION_NAME
}

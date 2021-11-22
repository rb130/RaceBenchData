

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

int im_video_test(IMAGE *im, int brightness, int error) {
    if (error) {
        im_error("im_video_test", "%s", _("error requested"));
        return (-1);
    } else {
        return (im_gaussnoise(im, 720, 576, brightness, 20));
    }
}

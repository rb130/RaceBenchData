

#if !(defined(_MSC_VER) || defined(__MINGW32__))
#include <sys/time.h>
#else
#include <sys/types.h>
#include <sys/timeb.h>
#endif
#include <time.h>

#include "common.h"
#include "osdep.h"

int64_t x264_mdate(void) {
#if !(defined(_MSC_VER) || defined(__MINGW32__))
    struct timeval tv_date;

    gettimeofday(&tv_date, NULL);
    return ((int64_t)tv_date.tv_sec * 1000000 + (int64_t)tv_date.tv_usec);
#else
    struct _timeb tb;
    _ftime(&tb);
    return ((int64_t)tb.time * (1000) + (int64_t)tb.millitm) * (1000);
#endif
}

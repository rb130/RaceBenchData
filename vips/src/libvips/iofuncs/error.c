

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/buf.h>
#include <vips/thread.h>

#ifdef OS_WIN32
#include <windows.h>
#include <lmerr.h>
#endif

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define IM_MAX_ERROR (10240)
static char im_error_text[IM_MAX_ERROR] = "";
static VipsBuf im_error_buf = VIPS_BUF_STATIC(im_error_text);

#define IM_DIAGNOSTICS "IM_DIAGNOSTICS"
#define IM_WARNING "IM_WARNING"

const char *im_error_buffer(void) {
    const char *msg;

    g_mutex_lock(im__global_lock);
    msg = vips_buf_all(&im_error_buf);
    g_mutex_unlock(im__global_lock);

    return (msg);
}

void im_verror(const char *domain, const char *fmt, va_list ap) {
    g_mutex_lock(im__global_lock);
    vips_buf_appendf(&im_error_buf, "%s: ", domain);
    vips_buf_vappendf(&im_error_buf, fmt, ap);
    vips_buf_appends(&im_error_buf, "\n");
    g_mutex_unlock(im__global_lock);
}

void im_error(const char *domain, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    im_verror(domain, fmt, ap);
    va_end(ap);
}

void im_verror_system(int err, const char *domain, const char *fmt, va_list ap) {
    im_verror(domain, fmt, ap);

#ifdef OS_WIN32
    {
        char *buf;

        if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&buf, 0, NULL)) {
            im_error(_("windows error"), "%s", buf);
            LocalFree(buf);
        }
    }
#else
    {
        char *buf;

        buf = g_locale_to_utf8(strerror(err), -1, NULL, NULL, NULL);
        im_error(_("unix error"), "%s", buf);
        g_free(buf);
    }
#endif
}

void im_error_system(int err, const char *domain, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    im_verror_system(err, domain, fmt, ap);
    va_end(ap);
}

void im_error_clear(void) {
    g_mutex_lock(im__global_lock);
    vips_buf_rewind(&im_error_buf);
    g_mutex_unlock(im__global_lock);
}

void im_vdiag(const char *domain, const char *fmt, va_list ap) {
    if (!g_getenv(IM_DIAGNOSTICS)) {
        g_mutex_lock(im__global_lock);
        (void)fprintf(stderr, _("%s: "), _("vips diagnostic"));
        (void)fprintf(stderr, _("%s: "), domain);
        (void)vfprintf(stderr, fmt, ap);
        (void)fprintf(stderr, "\n");
        g_mutex_unlock(im__global_lock);
    }
}

void im_diag(const char *domain, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    im_vdiag(domain, fmt, ap);
    va_end(ap);
}

void im_vwarn(const char *domain, const char *fmt, va_list ap) {
    if (!g_getenv(IM_WARNING)) {
        g_mutex_lock(im__global_lock);
        (void)fprintf(stderr, _("%s: "), _("vips warning"));
        (void)fprintf(stderr, _("%s: "), domain);
        (void)vfprintf(stderr, fmt, ap);
        (void)fprintf(stderr, "\n");
        g_mutex_unlock(im__global_lock);
    }
}

void im_warn(const char *domain, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    im_vwarn(domain, fmt, ap);
    va_end(ap);
}

void error_exit(const char *fmt, ...) {
    if (fmt) {
        va_list ap;

        fprintf(stderr, "%s: ", g_get_prgname());

        va_start(ap, fmt);
        (void)vfprintf(stderr, fmt, ap);
        va_end(ap);

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "%s", im_error_buffer());

    exit(1);
}

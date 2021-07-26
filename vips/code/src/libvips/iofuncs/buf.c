
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <vips/vips.h>
#include <vips/buf.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define MAX_STRSIZE (16000)

void vips_buf_rewind(VipsBuf *buf) {
    buf->i = 0;
    buf->lasti = 0;
    buf->full = FALSE;

    if (buf->base) {
        buf->base[0] = '\0';
    }
}

void vips_buf_init(VipsBuf *buf) {
    buf->base = NULL;
    buf->mx = 0;
    buf->dynamic = FALSE;
    vips_buf_rewind(buf);
}

void vips_buf_destroy(VipsBuf *buf) {
    if (buf->dynamic) {
        IM_FREE(buf->base);
    }

    vips_buf_init(buf);
}

void vips_buf_set_static(VipsBuf *buf, char *base, int mx) {
    g_assert(mx >= 4);

    vips_buf_destroy(buf);

    buf->base = base;
    buf->mx = mx;
    buf->dynamic = FALSE;
    vips_buf_rewind(buf);
}

void vips_buf_init_static(VipsBuf *buf, char *base, int mx) {
    vips_buf_init(buf);
    vips_buf_set_static(buf, base, mx);
}

void vips_buf_set_dynamic(VipsBuf *buf, int mx) {
    g_assert(mx >= 4);

    if (buf->mx == mx && buf->dynamic) {

        vips_buf_rewind(buf);
    } else {
        vips_buf_destroy(buf);

        if (!(buf->base = IM_ARRAY(NULL, mx, char))) {

            buf->full = TRUE;
        } else {
            buf->mx = mx;
            buf->dynamic = TRUE;
            vips_buf_rewind(buf);
        }
    }
}

void vips_buf_init_dynamic(VipsBuf *buf, int mx) {
    vips_buf_init(buf);
    vips_buf_set_dynamic(buf, mx);
}

gboolean vips_buf_appendns(VipsBuf *buf, const char *str, int sz) {
    int len;
    int n;
    int avail;
    int cpy;

    if (buf->full) {
        return (FALSE);
    }

    len = strlen(str);
    if (sz >= 0) {
        n = IM_MIN(sz, len);
    } else {
        n = len;
    }

    avail = buf->mx - buf->i - 4;

    cpy = IM_MIN(n, avail);

    strncpy(buf->base + buf->i, str, cpy);
    buf->i += cpy;

    if (buf->i >= buf->mx - 4) {
        buf->full = TRUE;
        strcpy(buf->base + buf->mx - 4, "...");
        buf->i = buf->mx - 1;
        return (FALSE);
    }

    return (TRUE);
}

gboolean vips_buf_appends(VipsBuf *buf, const char *str) {
    return (vips_buf_appendns(buf, str, -1));
}

gboolean vips_buf_appendc(VipsBuf *buf, char ch) {
    char tiny[2];

    tiny[0] = ch;
    tiny[1] = '\0';

    return (vips_buf_appendns(buf, tiny, 1));
}

gboolean vips_buf_change(VipsBuf *buf, const char *old, const char *new) {
    int olen = strlen(old);
    int nlen = strlen(new);
    int i;

    if (buf->full) {
        return (FALSE);
    }
    if (buf->i - olen + nlen > buf->mx - 4) {
        buf->full = TRUE;
        return (FALSE);
    }

    for (i = buf->i - olen; i > 0; i--) {
        if (im_isprefix(old, buf->base + i)) {
            break;
        }
    }
    g_assert(i >= 0);

    memmove(buf->base + i + nlen, buf->base + i + olen, buf->i - i - olen);

    memcpy(buf->base + i, new, nlen);
    buf->i = i + nlen + (buf->i - i - olen);

    return (TRUE);
}

gboolean vips_buf_removec(VipsBuf *buf, char ch) {
    if (buf->full) {
        return (FALSE);
    }
    if (buf->i <= 0) {
        return (FALSE);
    }
    if (buf->base[buf->i - 1] == ch) {
        buf->i -= 1;
    }

    return (TRUE);
}

gboolean vips_buf_appendf(VipsBuf *buf, const char *fmt, ...) {
    char str[MAX_STRSIZE];
    va_list ap;

    va_start(ap, fmt);
    (void)im_vsnprintf(str, MAX_STRSIZE, fmt, ap);
    va_end(ap);

    return (vips_buf_appends(buf, str));
}

gboolean vips_buf_vappendf(VipsBuf *buf, const char *fmt, va_list ap) {
    char str[MAX_STRSIZE];

    (void)im_vsnprintf(str, MAX_STRSIZE, fmt, ap);

    return (vips_buf_appends(buf, str));
}

gboolean vips_buf_appendg(VipsBuf *buf, double g) {
    char text[G_ASCII_DTOSTR_BUF_SIZE];

    g_ascii_dtostr(text, sizeof(text), g);

    return (vips_buf_appends(buf, text));
}

gboolean vips_buf_appendd(VipsBuf *buf, int d) {
    if (d < 0) {
        return (vips_buf_appendf(buf, " (%d)", d));
    } else {
        return (vips_buf_appendf(buf, " %d", d));
    }
}

gboolean vips_buf_appendgv(VipsBuf *buf, GValue *value) {
    char *str_value;
    gboolean result;

    str_value = g_strdup_value_contents(value);
    result = vips_buf_appends(buf, str_value);
    g_free(str_value);

    return (result);
}

const char *vips_buf_all(VipsBuf *buf) {
    buf->base[buf->i] = '\0';

    return (buf->base);
}

const char *vips_buf_firstline(VipsBuf *buf) {
    char *p;

    if ((p = strchr(vips_buf_all(buf), '\n'))) {
        *p = '\0';
    }

    return (vips_buf_all(buf));
}

gboolean vips_buf_is_empty(VipsBuf *buf) {
    return (buf->i == 0);
}

gboolean vips_buf_is_full(VipsBuf *buf) {
    return (buf->full);
}

int vips_buf_len(VipsBuf *buf) {
    return (buf->i);
}

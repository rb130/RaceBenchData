

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>

#ifdef OS_WIN32
#include <windows.h>
#endif

#include <assert.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define MAX_BUF (32768)

gboolean im_slist_equal(GSList *l1, GSList *l2) {
    while (l1 && l2) {
        if (l1->data != l2->data) {
            return (FALSE);
        }

        l1 = l1->next;
        l2 = l2->next;
    }

    if (l1 || l2) {
        return (FALSE);
    }

    return (TRUE);
}

void *im_slist_map2(GSList *list, VSListMap2Fn fn, void *a, void *b) {
    GSList *copy;
    GSList *i;
    void *result;

    copy = g_slist_copy(list);
    result = NULL;
    for (i = copy; i && !(result = fn(i->data, a, b)); i = i->next) {
        ;
    }
    g_slist_free(copy);

    return (result);
}

void *im_slist_map4(GSList *list, VSListMap4Fn fn, void *a, void *b, void *c, void *d) {
    GSList *copy;
    GSList *i;
    void *result;

    copy = g_slist_copy(list);
    result = NULL;
    for (i = copy; i && !(result = fn(i->data, a, b, c, d)); i = i->next) {
        ;
    }
    g_slist_free(copy);

    return (result);
}

void *im_slist_map2_rev(GSList *list, VSListMap2Fn fn, void *a, void *b) {
    GSList *copy;
    GSList *i;
    void *result;

    copy = g_slist_copy(list);
    copy = g_slist_reverse(copy);
    result = NULL;
    for (i = copy; i && !(result = fn(i->data, a, b)); i = i->next) {
        ;
    }
    g_slist_free(copy);

    return (result);
}

void *im_map_equal(void *a, void *b) {
    if (a == b) {
        return (a);
    }

    return (NULL);
}

void *im_slist_fold2(GSList *list, void *start, VSListFold2Fn fn, void *a, void *b) {
    void *c;
    GSList *this, *next;

    for (c = start, this = list; this; this = next) {
        next = this->next;

        if (!(c = fn(this->data, c, a, b))) {
            return (NULL);
        }
    }

    return (c);
}

static void im_slist_free_all_cb(void *thing, void *dummy) {
    im_free(thing);
}

void im_slist_free_all(GSList *list) {
    g_slist_foreach(list, im_slist_free_all_cb, NULL);
    g_slist_free(list);
}

GSList *im_slist_filter(GSList *list, VSListMap2Fn fn, void *a, void *b) {
    GSList *tmp;
    GSList *prev;

    prev = NULL;
    tmp = list;

    while (tmp) {
        if (fn(tmp->data, a, b)) {
            GSList *next = tmp->next;

            if (prev) {
                prev->next = next;
            }
            if (list == tmp) {
                list = next;
            }

            tmp->next = NULL;
            g_slist_free(tmp);
            tmp = next;
        } else {
            prev = tmp;
            tmp = tmp->next;
        }
    }

    return (list);
}

typedef struct {
    void *a;
    void *b;
    VSListMap2Fn fn;
    void *result;
} Pair;

static gboolean im_hash_table_predicate(const char *key, void *value, Pair *pair) {
    return ((pair->result == pair->fn(value, pair->a, pair->b)));
}

void *im_hash_table_map(GHashTable *hash, VSListMap2Fn fn, void *a, void *b) {
    Pair pair;

    pair.a = a;
    pair.b = b;
    pair.fn = fn;
    pair.result = NULL;

    g_hash_table_find(hash, (GHRFunc)im_hash_table_predicate, &pair);

    return (pair.result);
}

void *vips_type_map(GType base, VipsTypeMap2 fn, void *a, void *b) {
    GType *child;
    guint n_children;
    unsigned int i;
    void *result;

    child = g_type_children(base, &n_children);
    result = NULL;
    for (i = 0; i < n_children && !result; i++) {
        result = fn(child[i], a, b);
    }
    g_free(child);

    return (result);
}

void *vips_type_map_concrete_all(GType base, VipsTypeMap fn, void *a) {
    void *result;

    result = NULL;
    if (!G_TYPE_IS_ABSTRACT(base)) {
        result = fn(base, a);
    }
    if (!result) {
        result = vips_type_map(base, (VipsTypeMap2)vips_type_map_concrete_all, fn, a);
    }

    return (result);
}

void *vips_class_map_concrete_all(GType type, VipsClassMap fn, void *a) {
    void *result;

    result = NULL;
    if (!G_TYPE_IS_ABSTRACT(type)) {
        GTypeClass *class;

        if (!(class = g_type_class_peek(type))) {

            if (!(class = g_type_class_ref(type))) {
                im_error("vips_class_map_concrete_all", "%s", _("unable to build class"));
                return (NULL);
            }
        }

        result = fn(VIPS_OBJECT_CLASS(class), a);
    }
    if (!result) {
        result = vips_type_map(type, (VipsTypeMap2)vips_class_map_concrete_all, fn, a);
    }

    return (result);
}

static void *test_name(VipsObjectClass *class, const char *nickname) {
    if (strcasecmp(class->nickname, nickname) == 0) {
        return (class);
    }

    if (strcasecmp(G_OBJECT_CLASS_NAME(class), nickname) == 0) {
        return (class);
    }

    return (NULL);
}

VipsObjectClass *vips_class_find(const char *basename, const char *nickname) {
    VipsObjectClass *class;
    GType base;

    if (!(base = g_type_from_name(basename))) {
        im_error("vips_class_find", _("base class \"%s\" not found"), basename);
        return (NULL);
    }

    if (!(class = vips_class_map_concrete_all(base, (VipsClassMap)test_name, (void *)nickname))) {
        im_error("vips_class_find", _("class \"%s\" not found"), nickname);
        return (NULL);
    }

    return (class);
}

GType vips_type_find(const char *basename, const char *nickname) {
    VipsObjectClass *class;

    if (!(class = vips_class_find("VipsInterpolate", nickname))) {
        return (0);
    }

    return (G_TYPE_FROM_CLASS(class));
}

char *im_strncpy(char *dest, const char *src, int n) {
    int i;

    assert(n > 0);

    for (i = 0; i < n - 1; i++) {
        if (!(dest[i] = src[i])) {
            break;
        }
    }
    dest[i] = '\0';

    return (dest);
}

char *im_strrstr(const char *haystack, const char *needle) {
    int haystack_len = strlen(haystack);
    int needle_len = strlen(needle);
    int i;

    for (i = haystack_len - needle_len; i >= 0; i--) {
        if (strncmp(needle, haystack + i, needle_len) == 0) {
            return ((char *)haystack + i);
        }
    }

    return (NULL);
}

char *im_strdup(IMAGE *im, const char *str) {
    int l = strlen(str);
    char *buf;

    if (!(buf = (char *)im_malloc(im, l + 1))) {
        return (NULL);
    }
    strcpy(buf, str);

    return (buf);
}

gboolean im_ispostfix(const char *a, const char *b) {
    int m = strlen(a);
    int n = strlen(b);

    if (n > m) {
        return (FALSE);
    }

    return (strcmp(a + m - n, b) == 0);
}

gboolean im_isprefix(const char *a, const char *b) {
    int n = strlen(a);
    int m = strlen(b);
    int i;

    if (m < n) {
        return (FALSE);
    }
    for (i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return (FALSE);
        }
    }

    return (TRUE);
}

char *im_break_token(char *str, const char *brk) {
    char *p;

    if (!str || !*str) {
        return (NULL);
    }

    p = str + strspn(str, brk);

    if (!*p) {
        return (NULL);
    }

    p += strcspn(p, brk);

    if (*p) {

        *p++ = '\0';
        p += strspn(p, brk);
    }

    return (p);
}

int im_vsnprintf(char *str, size_t size, const char *format, va_list ap) {
#ifdef HAVE_VSNPRINTF
    return (vsnprintf(str, size, format, ap));
#else

    int n;
    static char buf[MAX_BUF];

    if (size > MAX_BUF)
        error_exit("panic: buffer overflow "
                   "(request to write %d bytes to buffer of %d bytes)",
            size, MAX_BUF);
    n = vsprintf(buf, format, ap);
    if (n > MAX_BUF)
        error_exit("panic: buffer overflow "
                   "(%d bytes written to buffer of %d bytes)",
            n, MAX_BUF);

    im_strncpy(str, buf, size);

    return (n);
#endif
}

int im_snprintf(char *str, size_t size, const char *format, ...) {
    va_list ap;
    int n;

    va_start(ap, format);
    n = im_vsnprintf(str, size, format, ap);
    va_end(ap);

    return (n);
}

void im_filename_split(const char *path, char *name, char *mode) {
    char *p;

    im_strncpy(name, path, FILENAME_MAX);

    for (p = name + strlen(name) - 1; p > name; p -= 1) {
        if (*p == ':') {
            char *q;

            for (q = p - 1; isalnum(*q) && q > name; q -= 1) {
                ;
            }

            if (*q == '.') {
                break;
            }
        }
    }

    if (*p == ':') {
        im_strncpy(mode, p + 1, FILENAME_MAX);
        *p = '\0';
    } else {
        strcpy(mode, "");
    }
}

const char *im_skip_dir(const char *path) {
    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    const char *p;
    const char *q;

    const char native_dir_sep = G_DIR_SEPARATOR;
    const char non_native_dir_sep = native_dir_sep == '/' ? '\\' : '/';

    im_filename_split(path, name, mode);

    p = name + strlen(name);

    for (q = p; q > name && q[-1] != native_dir_sep; q--) {
        ;
    }
    if (q == name) {
        for (q = p; q > name && q[-1] != non_native_dir_sep; q--) {
            ;
        }
    }

    return (path + (q - name));
}

void im_filename_suffix(const char *path, char *suffix) {
    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    char *p;

    im_filename_split(path, name, mode);
    if ((p = strrchr(name, '.'))) {
        strcpy(suffix, p);
    } else {
        strcpy(suffix, "");
    }
}

int im_filename_suffix_match(const char *path, const char *suffixes[]) {
    char suffix[FILENAME_MAX];
    const char **p;

    im_filename_suffix(path, suffix);
    for (p = suffixes; *p; p++) {
        if (g_ascii_strcasecmp(suffix, *p) == 0) {
            return (1);
        }
    }

    return (0);
}

char *im_getnextoption(char **in) {
    char *p = *in;
    char *q = p;

    if (!p || !*p) {
        return (NULL);
    }

    while ((p = strchr(p, ',')) && p[-1] == '\\') {
        p += 1;
    }

    if (p) {

        *p = '\0';
        *in = p + 1;
    } else {

        *in = NULL;
    }

    if (strlen(q) > 0) {
        return (q);
    } else {
        return (NULL);
    }
}

char *im_getsuboption(const char *buf) {
    char *p, *q, *r;

    if (!(p = strchr(buf, ':'))) {

        return (NULL);
    }

    p += 1;

    for (q = p; *q; q++) {
        if (q[0] == '\\' && q[1] == ',') {
            for (r = q; *r; r++) {
                r[0] = r[1];
            }
        }
    }

    return (p);
}

void *im_local(IMAGE *im, im_construct_fn cons, im_callback_fn dest, void *a, void *b, void *c) {
    void *obj;

    if (!im) {
        im_error("im_local", "%s", _("NULL image descriptor"));
        return (NULL);
    }

    if (!(obj = cons(a, b, c))) {
        return (NULL);
    }
    if (im_add_close_callback(im, (im_callback_fn)dest, obj, a)) {
        dest(obj, a);
        return (NULL);
    }

    return (obj);
}

int im_local_array(IMAGE *im, void **out, int n, im_construct_fn cons, im_callback_fn dest, void *a, void *b, void *c) {
    int i;

    for (i = 0; i < n; i++) {
        if (!(out[i] = im_local(im, cons, dest, a, b, c))) {
            return (-1);
        }
    }

    return (0);
}

gint64 im_file_length(int fd) {
#ifdef OS_WIN32
    struct _stati64 st;

    if (_fstati64(fd, &st) == -1) {
#else
    struct stat st;

    if (fstat(fd, &st) == -1) {
#endif
        im_error_system(errno, "im_file_length", "%s", _("unable to get file stats"));
        return (-1);
    }

    return (st.st_size);
}

int im__write(int fd, const void *buf, size_t count) {
    do {
        size_t nwritten = write(fd, buf, count);

        if (nwritten == (size_t)-1) {
            im_error_system(errno, "im__write", "%s", _("write failed"));
            return (-1);
        }

        buf = (void *)((char *)buf + nwritten);
        count -= nwritten;
    } while (count > 0);

    return (0);
}

char *im__file_read(FILE *fp, const char *filename, unsigned int *length_out) {
    long len;
    size_t read;
    char *str;

    fseek(fp, 0L, 2);
    len = ftell(fp);
    if (len > 20 * 1024 * 1024) {

        im_error("im__file_read", _("\"%s\" too long"), filename);
        return (NULL);
    }

    if (len == -1) {
        int size;

        str = NULL;
        len = 0;
        size = 0;
        do {
            size += 1024;
            if (!(str = realloc(str, size))) {
                im_error("im__file_read", "%s", _("out of memory"));
                return (NULL);
            }

            read = fread(str + len, sizeof(char), (size - len - 1) / sizeof(char), fp);
            len += read;
        } while (!feof(fp));

#ifdef DEBUG
        printf("read %ld bytes from unseekable stream\n", len);
#endif
    } else {

        if (!(str = im_malloc(NULL, len + 1))) {
            return (NULL);
        }
        rewind(fp);
        read = fread(str, sizeof(char), (size_t)len, fp);
        if (read != (size_t)len) {
            im_free(str);
            im_error("im__file_read", _("error reading from file \"%s\""), filename);
            return (NULL);
        }
    }

    str[len] = '\0';

    if (length_out) {
        *length_out = len;
    }

    return (str);
}

static gboolean filename_hasdir(const char *filename) {
    char *dirname;
    gboolean hasdir;

    dirname = g_path_get_dirname(filename);
    hasdir = (strcmp(dirname, ".") != 0);
    g_free(dirname);

    return (hasdir);
}

FILE *im__file_open_read(const char *filename, const char *fallback_dir) {
    FILE *fp;

#ifdef BINARY_OPEN
    fp = fopen(filename, "rb");
#else
    fp = fopen(filename, "r");
#endif
    if (fp) {
        return (fp);
    }

    if (fallback_dir && !filename_hasdir(filename)) {
        char *path;

        path = g_build_filename(fallback_dir, filename, NULL);
#ifdef BINARY_OPEN
        fp = fopen(path, "rb");
#else
        fp = fopen(path, "r");
#endif
        g_free(path);

        if (fp) {
            return (fp);
        }
    }

    im_error("im__file_open_read", _("unable to open file \"%s\" for reading"), filename);

    return (NULL);
}

FILE *im__file_open_write(const char *filename) {
    FILE *fp;

#ifdef BINARY_OPEN
    if (!(fp = fopen(filename, "wb"))) {
#else
    if (!(fp = fopen(filename, "w"))) {
#endif
        im_error("im__file_open_write", _("unable to open file \"%s\" for writing"), filename);
        return (NULL);
    }

    return (fp);
}

char *im__file_read_name(const char *filename, const char *fallback_dir, unsigned int *length_out) {
    FILE *fp;
    char *buffer;

    if (!(fp = im__file_open_read(filename, fallback_dir))) {
        return (NULL);
    }
    if (!(buffer = im__file_read(fp, filename, length_out))) {
        fclose(fp);
        return (NULL);
    }
    fclose(fp);

    return (buffer);
}

static GValue *im__gvalue_new(GType type) {
    GValue *value;

    value = g_new0(GValue, 1);
    g_value_init(value, type);

    return (value);
}

static GValue *im__gvalue_copy(GValue *value) {
    GValue *value_copy;

    value_copy = im__gvalue_new(G_VALUE_TYPE(value));
    g_value_copy(value, value_copy);

    return (value_copy);
}

static void im__gvalue_free(GValue *value) {
    g_value_unset(value);
    g_free(value);
}

GValue *im__gvalue_ref_string_new(const char *text) {
    GValue *value;

    value = im__gvalue_new(IM_TYPE_REF_STRING);
    im_ref_string_set(value, text);

    return (value);
}

void im__gslist_gvalue_free(GSList *list) {
    g_slist_foreach(list, (GFunc)im__gvalue_free, NULL);
    g_slist_free(list);
}

GSList *im__gslist_gvalue_copy(const GSList *list) {
    GSList *copy;
    const GSList *p;

    copy = NULL;

    for (p = list; p; p = p->next) {
        copy = g_slist_prepend(copy, im__gvalue_copy((GValue *)p->data));
    }

    copy = g_slist_reverse(copy);

    return (copy);
}

GSList *im__gslist_gvalue_merge(GSList *a, const GSList *b) {
    const GSList *i, *j;
    GSList *tail;

    tail = NULL;

    for (i = b; i; i = i->next) {
        GValue *value = (GValue *)i->data;

        assert(G_VALUE_TYPE(value) == IM_TYPE_REF_STRING);

        for (j = a; j; j = j->next) {
            GValue *value2 = (GValue *)j->data;

            assert(G_VALUE_TYPE(value2) == IM_TYPE_REF_STRING);

            if (im_ref_string_get(value) == im_ref_string_get(value2)) {
                break;
            }
        }

        if (!j) {
            tail = g_slist_prepend(tail, im__gvalue_copy(value));
        }
    }

    a = g_slist_concat(a, g_slist_reverse(tail));

    return (a);
}

char *im__gslist_gvalue_get(const GSList *list) {
    const GSList *p;
    size_t length;
    char *all;
    char *q;

    length = 0;
    for (p = list; p; p = p->next) {
        GValue *value = (GValue *)p->data;

        assert(G_VALUE_TYPE(value) == IM_TYPE_REF_STRING);

        length += im_ref_string_get_length(value) + 1;
    }

    if (length == 0) {
        return (NULL);
    }

    assert(length < 10 * 1024 * 1024);

    if (!(all = im_malloc(NULL, length + 1))) {
        return (NULL);
    }

    q = all;
    for (p = list; p; p = p->next) {
        GValue *value = (GValue *)p->data;

        strcpy(q, im_ref_string_get(value));
        q += im_ref_string_get_length(value);
        strcpy(q, "\n");
        q += 1;
    }

    g_assert((size_t)(q - all) == length);

    return (all);
}

int im__seek(int fd, gint64 pos) {
#ifdef OS_WIN32
    {
        HANDLE hFile = (HANDLE)_get_osfhandle(fd);
        LARGE_INTEGER p;

        p.QuadPart = pos;
        if (!SetFilePointerEx(hFile, p, NULL, FILE_BEGIN)) {
            im_error_system(GetLastError(), "im__seek", "%s", _("unable to seek"));
            return (-1);
        }
    }
#else
    if (lseek(fd, pos, SEEK_SET) == (off_t)-1) {
        im_error("im__seek", "%s", _("unable to seek"));
        return (-1);
    }
#endif

    return (0);
}

int im__ftruncate(int fd, gint64 pos) {
#ifdef OS_WIN32
    {
        HANDLE hFile = (HANDLE)_get_osfhandle(fd);
        LARGE_INTEGER p;

        p.QuadPart = pos;
        if (im__seek(fd, pos))
            return (-1);
        if (!SetEndOfFile(hFile)) {
            im_error_system(GetLastError(), "im__ftruncate", "%s", _("unable to truncate"));
            return (-1);
        }
    }
#else
    if (ftruncate(fd, pos)) {
        im_error_system(errno, "im__ftruncate", "%s", _("unable to truncate"));
        return (-1);
    }
#endif

    return (0);
}

int im__file_write(void *data, size_t size, size_t nmemb, FILE *stream) {
    size_t n;

    if (!data) {
        return (0);
    }

    if ((n = fwrite(data, size, nmemb, stream)) != nmemb) {
        im_error("im__file_write",
            _("writing error (%zd out of %zd blocks written) "
              "... disc full?"),
            n, nmemb);
        return (-1);
    }

    return (0);
}

int im__get_bytes(const char *filename, unsigned char buf[], int len) {
    int fd;

#ifdef BINARY_OPEN
    if ((fd = open(filename, O_RDONLY | O_BINARY)) == -1)
#else
    if ((fd = open(filename, O_RDONLY)) == -1) {
#endif
        return (0);
}
if (read(fd, buf, len) != len) {
    close(fd);
    return (0);
}
close(fd);

return (1);
}

const char *vips__token_get(const char *p, VipsToken *token, char *string, int size) {
    const char *q;
    int ch;
    int n;

    if (!p) {
        return (NULL);
    }

    p += strspn(p, " \t\n\r");
    if (!p[0]) {
        return (NULL);
    }

    switch ((ch = p[0])) {
    case '{':
    case '[':
    case '(':
        *token = VIPS_TOKEN_LEFT;
        p += 1;
        break;

    case ')':
    case ']':
    case '}':
        *token = VIPS_TOKEN_RIGHT;
        p += 1;
        break;

    case '=':
        *token = VIPS_TOKEN_EQUALS;
        p += 1;
        break;

    case ',':
        *token = VIPS_TOKEN_COMMA;
        p += 1;
        break;

    case '"':
    case '\'':

        *token = VIPS_TOKEN_STRING;

        do {

            if ((q = strchr(p + 1, ch))) {
                n = q - p + 1;
            } else {
                n = strlen(p + 1);
            }

            g_assert(size > n + 1);
            memcpy(string, p + 1, n);
            string[n] = '\0';

            if (p[n + 1] == ch && p[n] == '\\') {

                string[n - 1] = ch;
            }

            string += n;
            size -= n;
            p += n + 1;
        } while (p[0] && p[-1] == '\\');

        p += 1;

        break;

    default:

        *token = VIPS_TOKEN_STRING;
        n = strcspn(p, "[{()}]=,");
        g_assert(size > n + 1);
        memcpy(string, p, n);
        string[n] = '\0';
        p += n;

        while (isspace(string[n - 1])) {
            string[n - 1] = '\0';
            n -= 1;
        }

        break;
    }

    return (p);
}

const char *vips__token_must(const char *p, VipsToken *token, char *string, int size) {
    if (!(p = vips__token_get(p, token, string, size))) {
        im_error("get_token", "%s", _("unexpected end of string"));
        return (NULL);
    }

    return (p);
}

static const char *vips__token_string(VipsToken token) {
    switch (token) {
    case VIPS_TOKEN_LEFT:
        return (_("opening brace"));
    case VIPS_TOKEN_RIGHT:
        return (_("closing brace"));
    case VIPS_TOKEN_STRING:
        return (_("string"));
    case VIPS_TOKEN_EQUALS:
        return ("=");
    case VIPS_TOKEN_COMMA:
        return (",");

    default:
        g_assert(0);
    }
}

const char *vips__token_need(const char *p, VipsToken need_token, char *string, int size) {
    VipsToken token;

    if (!(p = vips__token_must(p, &token, string, size))) {
        return (NULL);
    }
    if (token != need_token) {
        im_error("get_token", _("expected %s, saw %s"), vips__token_string(need_token), vips__token_string(token));
        return (NULL);
    }

    return (p);
}

int im_existsf(const char *name, ...) {
    va_list ap;
    char buf1[PATH_MAX];

    va_start(ap, name);
    (void)im_vsnprintf(buf1, PATH_MAX - 1, name, ap);
    va_end(ap);

    if (!access(buf1, R_OK)) {
        return (1);
    }

    return (0);
}

#ifdef OS_WIN32
#define popen(b, m) _popen(b, m)
#define pclose(f) _pclose(f)
#endif

FILE *im_popenf(const char *fmt, const char *mode, ...) {
    va_list args;
    char buf[4096];
    FILE *fp;

    va_start(args, mode);
    (void)im_vsnprintf(buf, 4096, fmt, args);
    va_end(args);

#ifdef DEBUG
    printf("im_popenf: running: %s\n", buf);
#endif

    if (!(fp = popen(buf, mode))) {
        im_error("popenf", "%s", strerror(errno));
        return (NULL);
    }

    return (fp);
}

int im_ispoweroftwo(int p) {
    int i, n;

    for (i = 0, n = 0; p; i++, p >>= 1) {
        if (p & 1) {
            n++;
        }
    }

    if (n == 1) {

        return (i);
    } else {
        return (0);
    }
}

int im_isvips(const char *filename) {
    unsigned char buf[4];

    if (im__get_bytes(filename, buf, 4)) {
        if (buf[0] == 0x08 && buf[1] == 0xf2 && buf[2] == 0xa6 && buf[3] == 0xb6) {

            return (1);
        } else if (buf[3] == 0x08 && buf[2] == 0xf2 && buf[1] == 0xa6 && buf[0] == 0xb6) {

            return (1);
        }
    }

    return (0);
}

int im_amiMSBfirst(void) {
    int test;
    unsigned char *p = (unsigned char *)&test;

    test = 0;
    p[0] = 255;

    if (test == 255) {
        return (0);
    } else {
        return (1);
    }
}

char *im__temp_name(const char *format) {
    static int serial = 1;

    const char *tmpd;
    char file[FILENAME_MAX];
    char file2[FILENAME_MAX];

    char *name;
    int fd;

    if (!(tmpd = g_getenv("TMPDIR"))) {
        tmpd = "/tmp";
    }

    im_snprintf(file, FILENAME_MAX, "vips-%d-XXXXXX", serial++);
    im_snprintf(file2, FILENAME_MAX, format, file);
    name = g_build_filename(tmpd, file2, NULL);

    if ((fd = g_mkstemp(name)) == -1) {
        im_error("tempfile", _("unable to make temporary file %s"), name);
        g_free(name);
        return (NULL);
    }
    close(fd);
    g_unlink(name);

    return (name);
}

IMAGE *im__open_temp(const char *format) {
    char *name;
    IMAGE *disc;

    if (!(name = im__temp_name(format))) {
        return (NULL);
    }

    if (!(disc = im_open(name, "w"))) {
        g_free(name);
        return (NULL);
    }
    g_free(name);

    if (im_add_postclose_callback(disc, (im_callback_fn)unlink, disc->filename, NULL)) {
        im_close(disc);
        g_unlink(name);
    }

    return (disc);
}

const size_t im__sizeof_bandfmt[] = {IM_BBITS_BYTE >> 3, IM_BBITS_BYTE >> 3, IM_BBITS_SHORT >> 3, IM_BBITS_SHORT >> 3, IM_BBITS_INT >> 3, IM_BBITS_INT >> 3, IM_BBITS_FLOAT >> 3, IM_BBITS_COMPLEX >> 3, IM_BBITS_DOUBLE >> 3, IM_BBITS_DPCOMPLEX >> 3};

int im_bits_of_fmt(VipsBandFmt fmt) {
    return (fmt < 0 || fmt > IM_BANDFMT_DPCOMPLEX ? im_error("im_bits_of_fmt", _("unsupported band format: %d"), fmt), -1 : im__sizeof_bandfmt[fmt] << 3);
}

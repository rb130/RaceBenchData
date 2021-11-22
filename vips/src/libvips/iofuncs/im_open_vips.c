

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#include <sys/stat.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#include <libxml/parser.h>
#include <errno.h>

#ifdef OS_WIN32
#include <windows.h>
#endif

#include <vips/vips.h>
#include <vips/internal.h>
#include <vips/debug.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#ifdef BINARY_OPEN
#ifndef O_BINARY
#ifdef _O_BINARY
#define O_BINARY _O_BINARY
#endif
#endif
#endif

#define NAMESPACE "http://www.vips.ecs.soton.ac.uk/vips"

int im__open_image_file(const char *filename) {
    int fd;

#ifdef BINARY_OPEN
    if ((fd = open(filename, O_RDWR | O_BINARY)) == -1) {
#else
    if ((fd = open(filename, O_RDWR)) == -1) {
#endif

#ifdef BINARY_OPEN
        if ((fd = open(filename, O_RDONLY | O_BINARY)) == -1) {
#else
        if ((fd = open(filename, O_RDONLY)) == -1) {
#endif
            im_error("im__open_image_file", _("unable to open \"%s\", %s"), filename, strerror(errno));
            return (-1);
        }
    }

    return (fd);
}

gint64 im__image_pixel_length(IMAGE *im) {
    gint64 psize;

    switch (im->Coding) {
    case IM_CODING_LABQ:
    case IM_CODING_RAD:
    case IM_CODING_NONE:
        psize = (gint64)IM_IMAGE_SIZEOF_LINE(im) * im->Ysize;
        break;

    default:
        psize = im->Length;
        break;
    }

    return (psize + im->sizeof_header);
}

void im__read_4byte(int msb_first, unsigned char *to, unsigned char **from) {
    unsigned char *p = *from;
    int out;

    if (msb_first) {
        out = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
    } else {
        out = p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
    }

    *from += 4;
    *((guint32 *)to) = out;
}

void im__read_2byte(int msb_first, unsigned char *to, unsigned char **from) {
    int out;
    unsigned char *p = *from;

    if (msb_first) {
        out = p[0] << 8 | p[1];
    } else {
        out = p[1] << 8 | p[0];
    }

    *from += 2;
    *((guint16 *)to) = out;
}

void im__write_4byte(unsigned char **to, unsigned char *from) {
    *((guint32 *)*to) = *((guint32 *)from);
    *to += 4;
}

void im__write_2byte(unsigned char **to, unsigned char *from) {
    *((guint16 *)*to) = *((guint16 *)from);
    *to += 2;
}

typedef struct _FieldIO {
    glong offset;
    void (*read)(int msb_first, unsigned char *to, unsigned char **from);
    void (*write)(unsigned char **to, unsigned char *from);
} FieldIO;

static FieldIO fields[] = {{G_STRUCT_OFFSET(IMAGE, Xsize), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Ysize), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Bands), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Bbits), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, BandFmt), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Coding), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Type), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Xres), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Yres), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Length), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Compression), im__read_2byte, im__write_2byte}, {G_STRUCT_OFFSET(IMAGE, Level), im__read_2byte, im__write_2byte}, {G_STRUCT_OFFSET(IMAGE, Xoffset), im__read_4byte, im__write_4byte}, {G_STRUCT_OFFSET(IMAGE, Yoffset), im__read_4byte, im__write_4byte}};

int im__read_header_bytes(IMAGE *im, unsigned char *from) {
    int msb_first;
    int i;

    im__read_4byte(1, (unsigned char *)&im->magic, &from);
    if (im->magic != IM_MAGIC_INTEL && im->magic != IM_MAGIC_SPARC) {
        im_error("im_open", _("\"%s\" is not a VIPS image"), im->filename);
        return (-1);
    }
    msb_first = im->magic == IM_MAGIC_SPARC;

    for (i = 0; i < IM_NUMBER(fields); i++) {
        fields[i].read(msb_first, &G_STRUCT_MEMBER(unsigned char, im, fields[i].offset), &from);
    }

    im->Bbits = im_bits_of_fmt(im->BandFmt);

    return (0);
}

int im__write_header_bytes(IMAGE *im, unsigned char *to) {
    guint32 magic;
    int i;
    unsigned char *q;

    magic = im_amiMSBfirst() ? IM_MAGIC_SPARC : IM_MAGIC_INTEL;
    to[0] = magic >> 24;
    to[1] = magic >> 16;
    to[2] = magic >> 8;
    to[3] = magic;
    q = to + 4;

    for (i = 0; i < IM_NUMBER(fields); i++) {
        fields[i].write(&q, &G_STRUCT_MEMBER(unsigned char, im, fields[i].offset));
    }

    while (q - to < im->sizeof_header) {
        *q++ = 0;
    }

    return (0);
}

static char *read_chunk(int fd, gint64 offset, size_t length) {
    char *buf;

    if (im__seek(fd, offset)) {
        return (NULL);
    }
    if (!(buf = im_malloc(NULL, length + 1))) {
        return (NULL);
    }
    if (read(fd, buf, length) != (ssize_t)length) {
        im_free(buf);
        im_error("im_readhist", "%s", _("unable to read history"));
        return (NULL);
    }
    buf[length] = '\0';

    return (buf);
}

int im__has_extension_block(IMAGE *im) {
    gint64 psize;

    psize = im__image_pixel_length(im);
    g_assert(im->file_length > 0);

    return (im->file_length - psize > 0);
}

void *im__read_extension_block(IMAGE *im, int *size) {
    gint64 psize;
    void *buf;

    psize = im__image_pixel_length(im);
    g_assert(im->file_length > 0);
    if (im->file_length - psize > 10 * 1024 * 1024) {
        im_error("im_readhist", "%s",
            _("more than a 10 megabytes of XML? "
              "sufferin' succotash!"));
        return (NULL);
    }
    if (im->file_length - psize == 0) {
        return (NULL);
    }
    if (!(buf = read_chunk(im->fd, psize, im->file_length - psize))) {
        return (NULL);
    }
    if (size) {
        *size = im->file_length - psize;
    }

#ifdef DEBUG
    printf("im__read_extension_block: read %d bytes from %s\n", (int)(im->file_length - psize), im->filename);
    printf("data: \"%s\"\n", (char *)buf);
#endif

    return (buf);
}

static xmlDoc *read_xml(IMAGE *im) {
    void *buf;
    int size;
    xmlDoc *doc;
    xmlNode *node;

    if (!(buf = im__read_extension_block(im, &size))) {
        return (NULL);
    }
    if (!(doc = xmlParseMemory(buf, size))) {
        im_free(buf);
        return (NULL);
    }
    im_free(buf);
    if (!(node = xmlDocGetRootElement(doc)) || !node->nsDef || !im_isprefix(NAMESPACE, (char *)node->nsDef->href)) {
        im_error("im__readhist", "%s", _("incorrect namespace in XML"));
        xmlFreeDoc(doc);
        return (NULL);
    }

#ifdef DEBUG
    printf("read_xml: namespace == %s\n", node->nsDef->href);
#endif

    return (doc);
}

static xmlNode *get_node(xmlNode *base, const char *name) {
    xmlNode *i;

    for (i = base->children; i; i = i->next) {
        if (strcmp((char *)i->name, name) == 0) {
            return (i);
        }
    }

    return (NULL);
}

static int get_sprop(xmlNode *xnode, const char *name, char *buf, int sz) {
    char *value = (char *)xmlGetProp(xnode, (xmlChar *)name);

    if (!value) {
        return (0);
    }

    im_strncpy(buf, value, sz);
    IM_FREEF(xmlFree, value);

    return (1);
}

static void set_history(IMAGE *im, char *history) {
    GSList *history_list;
    char *p, *q;

    IM_FREEF(im__gslist_gvalue_free, im->history_list);

    history_list = NULL;

    for (p = history; *p; p = q) {
        if ((q = strchr(p, '\n'))) {
            *q = '\0';
            q += 1;
        } else {
            q = p + strlen(p);
        }

        history_list = g_slist_prepend(history_list, im__gvalue_ref_string_new(p));
    }

    im->history_list = g_slist_reverse(history_list);
}

static int rebuild_header_builtin(IMAGE *im, xmlNode *i) {
    char name[256];

    if (get_sprop(i, "name", name, 256)) {
        if (strcmp(name, "Hist") == 0) {
            char *history;

            history = (char *)xmlNodeGetContent(i);
            set_history(im, history);
            xmlFree(history);
        }
    }

    return (0);
}

static int rebuild_header_meta(IMAGE *im, xmlNode *i) {
    char name[256];
    char type[256];

    if (get_sprop(i, "name", name, 256) && get_sprop(i, "type", type, 256)) {
        GType gtype = g_type_from_name(type);

        if (gtype && g_value_type_transformable(IM_TYPE_SAVE_STRING, gtype)) {
            char *content;
            GValue save_value = {0};
            GValue value = {0};

            content = (char *)xmlNodeGetContent(i);
            g_value_init(&save_value, IM_TYPE_SAVE_STRING);
            im_save_string_set(&save_value, content);
            xmlFree(content);

            g_value_init(&value, gtype);
            if (!g_value_transform(&save_value, &value)) {
                g_value_unset(&save_value);
                im_error("im__readhist", "%s",
                    _("error transforming from "
                      "save format"));
                return (-1);
            }
            if (im_meta_set(im, name, &value)) {
                g_value_unset(&save_value);
                g_value_unset(&value);
                return (-1);
            }
            g_value_unset(&save_value);
            g_value_unset(&value);
        }
    }

    return (0);
}

static xmlDoc *get_xml(IMAGE *im) {
    if (im_header_get_typeof(im, IM_META_XML)) {
        xmlDoc *doc;

        if (im_meta_get_area(im, IM_META_XML, (void *)&doc)) {
            return (NULL);
        }

        return (doc);
    }

    return (NULL);
}

static int rebuild_header(IMAGE *im) {
    xmlDoc *doc;

    if ((doc = get_xml(im))) {
        xmlNode *root;
        xmlNode *block;

        if (!(root = xmlDocGetRootElement(doc))) {
            return (-1);
        }
        if ((block = get_node(root, "header"))) {
            xmlNode *i;

            for (i = block->children; i; i = i->next) {
                if (strcmp((char *)i->name, "field") == 0) {
                    if (rebuild_header_builtin(im, i)) {
                        return (-1);
                    }
                }
            }
        }
        if ((block = get_node(root, "meta"))) {
            xmlNode *i;

            for (i = block->children; i; i = i->next) {
                if (strcmp((char *)i->name, "field") == 0) {
                    if (rebuild_header_meta(im, i)) {
                        return (-1);
                    }
                }
            }
        }
    }

    return (0);
}

static int im__readhist(IMAGE *im) {

    if (im_header_get_typeof(im, IM_META_XML)) {
        im_meta_set_area(im, IM_META_XML, NULL, NULL);
    }

    if (im__has_extension_block(im)) {
        xmlDoc *doc;

        if (!(doc = read_xml(im))) {
            return (-1);
        }
        if (im_meta_set_area(im, IM_META_XML, (im_callback_fn)xmlFreeDoc, doc)) {
            xmlFreeDoc(doc);
            return (-1);
        }
    }

    if (rebuild_header(im)) {
        return (-1);
    }

    return (0);
}

#define MAX_STRSIZE (32768)

static int set_prop(xmlNode *node, const char *name, const char *fmt, ...) {
    va_list ap;
    char value[MAX_STRSIZE];

    va_start(ap, fmt);
    (void)im_vsnprintf(value, MAX_STRSIZE, fmt, ap);
    va_end(ap);

    if (!xmlSetProp(node, (xmlChar *)name, (xmlChar *)value)) {
        im_error("im_writehist",
            _("unable to set property \"%s\" "
              "to value \"%s\"."),
            name, value);
        return (-1);
    }

    return (0);
}

static int set_sprop(xmlNode *node, const char *name, const char *value) {
    if (value && set_prop(node, name, "%s", value)) {
        return (-1);
    }

    return (0);
}

static int set_field(xmlNode *node, const char *name, const char *type, const char *content) {
    xmlNode *field;

    if (!(field = xmlNewChild(node, NULL, (xmlChar *)"field", NULL)) || set_sprop(field, "type", type) || set_sprop(field, "name", name)) {
        return (-1);
    }
    xmlNodeSetContent(field, (xmlChar *)content);

    return (0);
}

static void *save_fields_meta(Meta *meta, xmlNode *node) {
    GType type = G_VALUE_TYPE(&meta->value);

    if (g_value_type_transformable(type, IM_TYPE_SAVE_STRING) && g_value_type_transformable(IM_TYPE_SAVE_STRING, type)) {
        GValue save_value = {0};

        g_value_init(&save_value, IM_TYPE_SAVE_STRING);
        if (!g_value_transform(&meta->value, &save_value)) {
            im_error("im__writehist", "%s", _("error transforming to save format"));
            return (node);
        }
        if (set_field(node, meta->field, g_type_name(type), im_save_string_get(&save_value))) {
            g_value_unset(&save_value);
            return (node);
        }
        g_value_unset(&save_value);
    }

    return (NULL);
}

static int save_fields(IMAGE *im, xmlNode *node) {
    xmlNode *this;

    if (!(this = xmlNewChild(node, NULL, (xmlChar *)"header", NULL))) {
        return (-1);
    }
    if (set_field(this, "Hist", g_type_name(IM_TYPE_REF_STRING), im_history_get(im))) {
        return (-1);
    }

    if (!(this = xmlNewChild(node, NULL, (xmlChar *)"meta", NULL))) {
        return (-1);
    }
    if (im->Meta_traverse && im_slist_map2(im->Meta_traverse, (VSListMap2Fn)save_fields_meta, this, NULL)) {
        return (-1);
    }

    return (0);
}

int im__write_extension_block(IMAGE *im, void *buf, int size) {
    gint64 length;
    gint64 psize;

    psize = im__image_pixel_length(im);
    if ((length = im_file_length(im->fd)) == -1) {
        return (-1);
    }
    if (length - psize < 0) {
        im_error("im__write_extension_block", "%s", _("file has been truncated"));
    }

    if (im__ftruncate(im->fd, psize) || im__seek(im->fd, psize)) {
    }
    if (im__write(im->fd, buf, size)) {
    }

#ifdef DEBUG
    printf("im__write_extension_block: written %d bytes of XML to %s\n", size, im->filename);
#endif

    return (0);
}

#ifdef DEBUG

const char *rpt(char ch, int n) {
    int i;
    static char buf[200];

    n = IM_MIN(190, n);

    for (i = 0; i < n; i++)
        buf[i] = ch;
    buf[i] = '\0';

    return (buf);
}

const char *spc(int n) {
    return (rpt(' ', n));
}

static void prettify_tree_sub(xmlNode *xnode, int indent) {
    xmlNode *txt;
    xmlNode *next;

    for (;;) {
        next = xnode->next;

        txt = xmlNewText("\n");
        xmlAddPrevSibling(xnode, txt);
        txt = xmlNewText(spc(indent));
        xmlAddPrevSibling(xnode, txt);

        if (xnode->children)
            prettify_tree_sub(xnode->children, indent + 2);

        if (!next)
            break;

        xnode = next;
    }

    txt = xmlNewText(spc(indent - 2));
    xmlAddNextSibling(xnode, txt);
    txt = xmlNewText("\n");
    xmlAddNextSibling(xnode, txt);
}

void prettify_tree(xmlDoc *xdoc) {
    xmlNode *xnode = xmlDocGetRootElement(xdoc);

    prettify_tree_sub(xnode, 0);
}
#endif

int im__writehist(IMAGE *im) {
    xmlDoc *doc;
    char namespace[256];
    char *dump;
    int dump_size;

    assert(im->dtype == IM_OPENOUT);
    assert(im->fd != -1);

    if (!(doc = xmlNewDoc((xmlChar *)"1.0"))) {
        return (-1);
    }

    im_snprintf(namespace, 256, "%s/%d.%d.%d", NAMESPACE, IM_MAJOR_VERSION, IM_MINOR_VERSION, IM_MICRO_VERSION);
    if (!(doc->children = xmlNewDocNode(doc, NULL, (xmlChar *)"root", NULL)) || set_sprop(doc->children, "xmlns", namespace) || save_fields(im, doc->children)) {
        im_error("im__writehist", "%s", _("xml save error"));
        xmlFreeDoc(doc);
        return (-1);
    }

    xmlDocDumpMemory(doc, (xmlChar **)((char *)&dump), &dump_size);
    if (!dump) {
        im_error("im__writehist", "%s", _("xml save error"));
        xmlFreeDoc(doc);
        return (-1);
    }

    if (im__write_extension_block(im, dump, dump_size)) {
        xmlFreeDoc(doc);
        xmlFree(dump);
        return (-1);
    }

#ifdef DEBUG
    {
        char *dump2;
        int dump_size2;

        prettify_tree(doc);

        xmlDocDumpMemory(doc, (xmlChar **)&dump2, &dump_size2);
        if (!dump2) {
            im_error("im__writehist", "%s", _("xml save error"));
            xmlFreeDoc(doc);
            xmlFree(dump);
            return (-1);
        }

        printf("im__writehist: saved XML is: \"%s\"", dump2);
        xmlFree(dump2);
    }
#endif

    xmlFreeDoc(doc);
    xmlFree(dump);

    return (0);
}

int im_openin(IMAGE *image) {

    unsigned char header[IM_SIZEOF_HEADER];

    gint64 psize;
    gint64 rsize;

    image->dtype = IM_OPENIN;
    if ((image->fd = im__open_image_file(image->filename)) == -1) {
        return (-1);
    }
    if (read(image->fd, header, IM_SIZEOF_HEADER) != IM_SIZEOF_HEADER || im__read_header_bytes(image, header)) {
        im_error("im__read_header", _("unable to read header for \"%s\", %s"), image->filename, strerror(errno));
        return (-1);
    }

    psize = im__image_pixel_length(image);
    if ((rsize = im_file_length(image->fd)) == -1) {
        return (-1);
    }
    image->file_length = rsize;
    if (psize > rsize) {
        im_warn("im__read_header", _("unable to read data for \"%s\", %s"), image->filename, _("file has been truncated"));
    }

    image->dhint = IM_THINSTRIP;

    if (im__readhist(image)) {
        im_warn("im__read_header", _("error reading XML: %s"), im_error_buffer());
        im_error_clear();
    }

    return (0);
}

int im_openinrw(IMAGE *image) {
    if (im_openin(image)) {
        return (-1);
    }
    if (im_mapfilerw(image)) {
        return (-1);
    }
    image->data = image->baseaddr + image->sizeof_header;
    image->dtype = IM_MMAPINRW;

#ifdef DEBUG
    printf("im_openinrw: completely mmap()ing \"%s\" read-write\n", image->filename);
#endif

    return (0);
}

IMAGE *im_open_vips(const char *filename) {
    char name[FILENAME_MAX];
    char mode[FILENAME_MAX];
    IMAGE *im;

    im_filename_split(filename, name, mode);

    if (!(im = im_init(name))) {
        return (NULL);
    }
    if (mode[0] == 'w') {
        if (im_openinrw(im)) {
            im_close(im);
            return (NULL);
        }
        if (im_isMSBfirst(im) != im_amiMSBfirst()) {
            im_close(im);
            im_error("im_open_vips", "%s",
                _("open for read-write for "
                  "native format images only"));
            return (NULL);
        }
    } else {
        if (im_openin(im)) {
            im_close(im);
            return (NULL);
        }
    }

    if (im_isMSBfirst(im) != im_amiMSBfirst() && im->Coding == IM_CODING_NONE && im->BandFmt != IM_BANDFMT_CHAR && im->BandFmt != IM_BANDFMT_UCHAR) {
        IMAGE *im2;

        if (!(im2 = im_open(filename, "p"))) {
            im_close(im);
            return (NULL);
        }
        if (im_add_close_callback(im2, (im_callback_fn)im_close, im, NULL)) {
            im_close(im);
            im_close(im2);
            return (NULL);
        }
        if (im_copy_swap(im, im2)) {
            im_close(im2);
            return (NULL);
        }
        im = im2;
    }

    return (im);
}

IMAGE *im_openout(const char *filename) {
    IMAGE *image;

    if (!(image = im_init(filename))) {
        return (NULL);
    }
    image->dtype = IM_OPENOUT;

    return (image);
}

#include "racebench_bugs.h"


#include "common/common.h"
#include "x264.h"
#include "matroska.h"
#include "muxers.h"

#ifndef _MSC_VER
#include "config.h"
#endif

#include <sys/types.h>

#ifdef AVIS_INPUT
#include <windows.h>
#include <vfw.h>
#endif

#ifdef MP4_OUTPUT
#include <gpac/isomedia.h>
#endif

static int64_t gcd(int64_t a, int64_t b) {
    while (1) {
        int64_t c = a % b;
        if (!c) {
            return b;
        }
        a = b;
        b = c;
    }
}

typedef struct {
    FILE *fh;
    int width, height;
    int next_frame;
} yuv_input_t;

int open_file_yuv(char *psz_filename, hnd_t *p_handle, x264_param_t *p_param) {
    yuv_input_t *h = malloc(sizeof(yuv_input_t));
    h->width = p_param->i_width;
    h->height = p_param->i_height;
    h->next_frame = 0;

    if (!strcmp(psz_filename, "-")) {
        h->fh = stdin;
    } else {
        h->fh = fopen(psz_filename, "rb");
    }
    if (h->fh == NULL) {
        return -1;
    }

    *p_handle = (hnd_t)h;
    return 0;
}

int get_frame_total_yuv(hnd_t handle) {
    yuv_input_t *h = handle;
    int i_frame_total = 0;

    if (!fseek(h->fh, 0, SEEK_END)) {
        uint64_t i_size = ftell(h->fh);
        fseek(h->fh, 0, SEEK_SET);
        i_frame_total = (int)(i_size / (h->width * h->height * 3 / 2));
    }

    return i_frame_total;
}

int read_frame_yuv(x264_picture_t *p_pic, hnd_t handle, int i_frame) {
    yuv_input_t *h = handle;

    if (i_frame != h->next_frame) {
        if (fseek(h->fh, (uint64_t)i_frame * h->width * h->height * 3 / 2, SEEK_SET)) {
            return -1;
        }
    }

    if (fread(p_pic->img.plane[0], 1, h->width * h->height, h->fh) <= 0 || fread(p_pic->img.plane[1], 1, h->width * h->height / 4, h->fh) <= 0 || fread(p_pic->img.plane[2], 1, h->width * h->height / 4, h->fh) <= 0) {
        return -1;
    }

    h->next_frame = i_frame + 1;

    return 0;
}

int close_file_yuv(hnd_t handle) {
    yuv_input_t *h = handle;
    if (!h || !h->fh) {
        return 0;
    }
    fclose(h->fh);
    free(h);
    return 0;
}

typedef struct {
    FILE *fh;
    int width, height;
    int next_frame;
    int seq_header_len, frame_header_len;
    int frame_size;
} y4m_input_t;

#define Y4M_MAGIC "YUV4MPEG2"
#define MAX_YUV4_HEADER 80
#define Y4M_FRAME_MAGIC "FRAME"
#define MAX_FRAME_HEADER 80

int open_file_y4m(char *psz_filename, hnd_t *p_handle, x264_param_t *p_param) {
    int i, n, d;
    int interlaced;
    char header[MAX_YUV4_HEADER + 10];
    char *tokstart, *tokend, *header_end;
    y4m_input_t *h = malloc(sizeof(y4m_input_t));

    h->next_frame = 0;

    if (!strcmp(psz_filename, "-")) {
        h->fh = stdin;
    } else {
        h->fh = fopen(psz_filename, "rb");
    }
    if (h->fh == NULL) {
        return -1;
    }

    h->frame_header_len = strlen(Y4M_FRAME_MAGIC) + 1;

    for (i = 0; i < MAX_YUV4_HEADER; i++) {
        header[i] = fgetc(h->fh);
        if (header[i] == '\n') {

            header[i + 1] = 0x20;
            header[i + 2] = 0;
            break;
        }
    }
    if (i == MAX_YUV4_HEADER || strncmp(header, Y4M_MAGIC, strlen(Y4M_MAGIC))) {
        return -1;
    }

    header_end = &header[i + 1];
    h->seq_header_len = i + 1;
    for (tokstart = &header[strlen(Y4M_MAGIC) + 1]; tokstart < header_end; tokstart++) {
        if (*tokstart == 0x20) {
            continue;
        }
        switch (*tokstart++) {
        case 'W':
            h->width = p_param->i_width = strtol(tokstart, &tokend, 10);
            tokstart = tokend;
            break;
        case 'H':
            h->height = p_param->i_height = strtol(tokstart, &tokend, 10);
            tokstart = tokend;
            break;
        case 'C':
            if (strncmp("420", tokstart, 3)) {
                fprintf(stderr, "Colorspace unhandled\n");
                return -1;
            }
            tokstart = strchr(tokstart, 0x20);
            break;
        case 'I':
            switch (*tokstart++) {
            case 'p':
                interlaced = 0;
                break;
            case '?':
            case 't':
            case 'b':
            case 'm':
            default:
                interlaced = 1;
                fprintf(stderr, "Warning, this sequence might be interlaced\n");
            }
            break;
        case 'F':
            if (sscanf(tokstart, "%d:%d", &n, &d) == 2 && n && d) {
                x264_reduce_fraction(&n, &d);
                p_param->i_fps_num = n;
                p_param->i_fps_den = d;
            }
            tokstart = strchr(tokstart, 0x20);
            break;
        case 'A':

            if (sscanf(tokstart, "%d:%d", &n, &d) == 2 && n && d && !p_param->vui.i_sar_width && !p_param->vui.i_sar_height) {
                x264_reduce_fraction(&n, &d);
                p_param->vui.i_sar_width = n;
                p_param->vui.i_sar_height = d;
            }
            tokstart = strchr(tokstart, 0x20);
            break;
        case 'X':
            if (!strncmp("YSCSS=", tokstart, 6)) {

                tokstart += 6;
                if (strncmp("420JPEG", tokstart, 7) && strncmp("420MPEG2", tokstart, 8) && strncmp("420PALDV", tokstart, 8)) {
                    fprintf(stderr, "Unsupported extended colorspace\n");
                    return -1;
                }
            }
            tokstart = strchr(tokstart, 0x20);
            break;
        }
    }

    fprintf(stderr, "yuv4mpeg: %ix%i@%i/%ifps, %i:%i\n", h->width, h->height, p_param->i_fps_num, p_param->i_fps_den, p_param->vui.i_sar_width, p_param->vui.i_sar_height);

    *p_handle = (hnd_t)h;
    return 0;
}

int get_frame_total_y4m(hnd_t handle) {
    y4m_input_t *h = handle;
    int i_frame_total = 0;
    uint64_t init_pos = ftell(h->fh);

    if (!fseek(h->fh, 0, SEEK_END)) {
        uint64_t i_size = ftell(h->fh);
        fseek(h->fh, init_pos, SEEK_SET);
        i_frame_total = (int)((i_size - h->seq_header_len) / (3 * (h->width * h->height) / 2 + h->frame_header_len));
    }

    return i_frame_total;
}

int read_frame_y4m(x264_picture_t *p_pic, hnd_t handle, int i_frame) {
    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) - (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0xbc523106));
    #endif
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0xd917c87));
    #endif
    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) ^ (0x1cff85a0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) - (0x8a008f89);
    rb_state13.var_1 = (rb_state13.var_1) - (9180 < rb_input_size ? (uint32_t)rb_input[9180] : 0x39ae1e25);
    rb_state13.var_1 = (rb_state13.var_1) + (((rb_state13.var_1) == (0xffffffd2)) ? (rb_state13.var_1) : (0xc065096b));
    #endif
    int slen = strlen(Y4M_FRAME_MAGIC);
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x0)) {
        rb_state17.var_4 = 0x6f484f95;
        rb_state17.var_5 = 0x4be075d1;
        rb_state17.var_6 = (rb_state17.var_6) + (rb_state17.var_3);
        rb_state17.var_7 = (rb_state17.var_7) - (((0xcb6bad8d) + (rb_state17.var_5)) - (rb_state17.var_4));
        rb_state17.var_8 = (rb_state17.var_8) - (((((0xd212e345) + (rb_state17.var_6)) + (0x9caae078)) + (rb_state17.var_4)) ^ (0x25ec00a1));
        rb_state17.var_9 = (rb_state17.var_9) + (rb_state17.var_7);
        rb_state17.var_10 = (rb_state17.var_10) - (((0xcde3cc5e) ^ (rb_state17.var_8)) + (0x61456084));
        rb_state17.var_11 = (rb_state17.var_11) - (((rb_state17.var_9) == (0x57fc2c37)) ? (rb_state17.var_10) : (0x6f2111c9));
        rb_state17.var_1 = (rb_state17.var_1) - (rb_state17.var_11);
    }
    #endif
    int i = 0;
    char header[16];
    #ifdef RACEBENCH_BUG_9
    rb_state9.var_1 = (rb_state9.var_1) ^ (((rb_state9.var_1) == (0x0)) ? (rb_state9.var_1) : (0xd3732f7d));
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_1 = (rb_state13.var_1) ^ ((0xb9e5eba) ^ (rb_state13.var_1));
    if ((rb_state13.var_1) == (0xb9e5eba)) {
        rb_state13.var_8 = (rb_state13.var_8) - (rb_state13.var_8);
        rb_state13.var_9 = (rb_state13.var_9) ^ (((0xb0285c49) ^ (rb_state13.var_9)) + (rb_state13.var_7));
        rb_state13.var_10 = (rb_state13.var_10) ^ (0xf5c07f5f);
        rb_state13.var_11 = (rb_state13.var_11) - (((rb_state13.var_2) == (0x5e820d5c)) ? (rb_state13.var_3) : (0xe9e32688));
        rb_state13.var_12 = (rb_state13.var_12) ^ (((((0xb926154c) ^ (rb_state13.var_9)) - (rb_state13.var_10)) ^ (0xd0b2b5af)) - (rb_state13.var_8));
        rb_state13.var_13 = (rb_state13.var_13) + (((rb_state13.var_10) == (0xf5c07f5f)) ? (rb_state13.var_11) : (0x6089a3e4));
        rb_state13.var_14 = (rb_state13.var_14) ^ (((0x996d15a6) ^ (rb_state13.var_12)) ^ (rb_state13.var_11));
        rb_state13.var_15 = (rb_state13.var_15) + (((rb_state13.var_13) == (0xa17df2a4)) ? (rb_state13.var_14) : (0x2f1e3bc9));
        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_15);
    }
    #endif
    y4m_input_t *h = handle;

    if (i_frame != h->next_frame) {
        if (fseek(h->fh, (uint64_t)i_frame * (3 * (h->width * h->height) / 2 + h->frame_header_len) + h->seq_header_len, SEEK_SET)) {
            return -1;
        }
    }

    if (fread(header, 1, slen, h->fh) != slen) {
        return -1;
    }

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) - (6321 < rb_input_size ? (uint32_t)rb_input[6321] : 0xbacbdfd9);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) + (rb_state15.var_0);
    #endif
    header[slen] = 0;
    if (strncmp(header, Y4M_FRAME_MAGIC, slen)) {
        #ifdef RACEBENCH_BUG_3
        rb_state3.var_2 = (rb_state3.var_2) - (2898 < rb_input_size ? (uint32_t)rb_input[2898] : 0x74d70af8);
        #endif
        #ifdef RACEBENCH_BUG_6
        rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_0);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_2 = (rb_state9.var_2) - (rb_state9.var_2);
        #endif
        fprintf(stderr, "Bad header magic (%08X <=> %s)\n", *((uint32_t *)header), header);
        return -1;
    }

    while (i < MAX_FRAME_HEADER && fgetc(h->fh) != '\n') {
        i++;
    }
    if (i == MAX_FRAME_HEADER) {
        fprintf(stderr, "Bad frame header!\n");
        return -1;
    }
    h->frame_header_len = i + slen + 1;

    if (fread(p_pic->img.plane[0], 1, h->width * h->height, h->fh) <= 0 || fread(p_pic->img.plane[1], 1, h->width * h->height / 4, h->fh) <= 0 || fread(p_pic->img.plane[2], 1, h->width * h->height / 4, h->fh) <= 0) {
        return -1;
    }

    h->next_frame = i_frame + 1;

    return 0;
}

int close_file_y4m(hnd_t handle) {
    y4m_input_t *h = handle;
    if (!h || !h->fh) {
        return 0;
    }
    fclose(h->fh);
    free(h);
    return 0;
}

#ifdef AVIS_INPUT
typedef struct {
    PAVISTREAM p_avi;
    int width, height;
} avis_input_t;

int open_file_avis(char *psz_filename, hnd_t *p_handle, x264_param_t *p_param) {
    avis_input_t *h = malloc(sizeof(avis_input_t));
    AVISTREAMINFO info;
    int i;

    *p_handle = (hnd_t)h;

    AVIFileInit();
    if (AVIStreamOpenFromFile(&h->p_avi, psz_filename, streamtypeVIDEO, 0, OF_READ, NULL)) {
        AVIFileExit();
        return -1;
    }

    if (AVIStreamInfo(h->p_avi, &info, sizeof(AVISTREAMINFO))) {
        AVIStreamRelease(h->p_avi);
        AVIFileExit();
        return -1;
    }

    if (info.fccHandler != MAKEFOURCC('Y', 'V', '1', '2')) {
        fprintf(stderr, "avis [error]: unsupported input format (%c%c%c%c)\n", (char)(info.fccHandler & 0xff), (char)((info.fccHandler >> 8) & 0xff), (char)((info.fccHandler >> 16) & 0xff), (char)((info.fccHandler >> 24)));

        AVIStreamRelease(h->p_avi);
        AVIFileExit();

        return -1;
    }

    h->width = p_param->i_width = info.rcFrame.right - info.rcFrame.left;
    h->height = p_param->i_height = info.rcFrame.bottom - info.rcFrame.top;
    i = gcd(info.dwRate, info.dwScale);
    p_param->i_fps_den = info.dwScale / i;
    p_param->i_fps_num = info.dwRate / i;

    fprintf(stderr, "avis [info]: %dx%d @ %.2f fps (%d frames)\n", p_param->i_width, p_param->i_height, (double)p_param->i_fps_num / (double)p_param->i_fps_den, (int)info.dwLength);

    return 0;
}

int get_frame_total_avis(hnd_t handle) {
    avis_input_t *h = handle;
    AVISTREAMINFO info;

    if (AVIStreamInfo(h->p_avi, &info, sizeof(AVISTREAMINFO)))
        return -1;

    return info.dwLength;
}

int read_frame_avis(x264_picture_t *p_pic, hnd_t handle, int i_frame) {
    avis_input_t *h = handle;

    p_pic->img.i_csp = X264_CSP_YV12;

    if (AVIStreamRead(h->p_avi, i_frame, 1, p_pic->img.plane[0], h->width * h->height * 3 / 2, NULL, NULL))
        return -1;

    return 0;
}

int close_file_avis(hnd_t handle) {
    avis_input_t *h = handle;
    AVIStreamRelease(h->p_avi);
    AVIFileExit();
    free(h);
    return 0;
}
#endif

#ifdef HAVE_PTHREAD
typedef struct {
    int (*p_read_frame)(x264_picture_t *p_pic, hnd_t handle, int i_frame);
    int (*p_close_infile)(hnd_t handle);
    hnd_t p_handle;
    x264_picture_t pic;
    x264_pthread_t tid;
    int next_frame;
    int frame_total;
    int in_progress;
    struct thread_input_arg_t *next_args;
} thread_input_t;

typedef struct thread_input_arg_t {
    thread_input_t *h;
    x264_picture_t *pic;
    int i_frame;
    int status;
} thread_input_arg_t;

int open_file_thread(char *psz_filename, hnd_t *p_handle, x264_param_t *p_param) {
    thread_input_t *h = malloc(sizeof(thread_input_t));
    x264_picture_alloc(&h->pic, X264_CSP_I420, p_param->i_width, p_param->i_height);
    h->p_read_frame = p_read_frame;
    h->p_close_infile = p_close_infile;
    h->p_handle = *p_handle;
    h->in_progress = 0;
    h->next_frame = -1;
    h->next_args = malloc(sizeof(thread_input_arg_t));
    h->next_args->h = h;
    h->next_args->status = 0;
    h->frame_total = p_get_frame_total(h->p_handle);

    *p_handle = (hnd_t)h;
    return 0;
}

int get_frame_total_thread(hnd_t handle) {
    thread_input_t *h = handle;
    return h->frame_total;
}

static void read_frame_thread_int(thread_input_arg_t *i) {
    #ifdef RACEBENCH_BUG_10
    rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
    #endif
    #ifdef RACEBENCH_BUG_13
    rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x80e7a313));
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) ^ (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_0) : (0xd73a8e11));
    rb_state17.var_1 = (rb_state17.var_1) - ((0xbe4d306d) ^ (rb_state17.var_1));
    rb_state17.var_2 = (rb_state17.var_2) + (294 < rb_input_size ? (uint32_t)rb_input[294] : 0xc33a5b0e);
    rb_state17.var_2 = (rb_state17.var_2) + (((rb_state17.var_0) == (0x0)) ? (rb_state17.var_2) : (0xd83db431));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_1 = (rb_state18.var_1) + (0x290cda54);
    rb_state18.var_1 = (rb_state18.var_1) ^ (0x6c541e7c);
    rb_state18.var_0 = (rb_state18.var_0) ^ (0x4d9b8c41);
    rb_state18.var_1 = (rb_state18.var_1) - (0xa74df9b8);
    if ((rb_state18.var_2) == (0x0)) {
        pthread_mutex_lock(&(rb_state18.lock_17));
        rb_state18.var_13 = 0xfc156ba9;
        rb_state18.var_14 = 0x4b422cb8;
        rb_state18.var_15 = (rb_state18.var_15) - (((rb_state18.var_14) == (0x4b422cb8)) ? (rb_state18.var_7) : (0x4216de9e));
        rb_state18.var_16 = (rb_state18.var_16) ^ (((rb_state18.var_13) == (0xfc156ba9)) ? (rb_state18.var_15) : (0xc15355c5));
        rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_16);
        pthread_mutex_unlock(&(rb_state18.lock_17));
    }
    #endif
    i->status = i->h->p_read_frame(i->pic, i->h->p_handle, i->i_frame);
}

int read_frame_thread(x264_picture_t *p_pic, hnd_t handle, int i_frame) {
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) - (3615 < rb_input_size ? (uint32_t)rb_input[3615] : 0x7246fc42);
    #endif
    #ifdef RACEBENCH_BUG_9
    if ((rb_state9.var_17) == (0xffffffa8)) {
        if ((rb_state9.var_27) != (0x0)) {
            rb_state9.var_39 = !((rb_state9.var_25) == (rb_state9.var_26));
        }
    }
    #endif
    thread_input_t *h = handle;
    UNUSED void *stuff;
    int ret = 0;

    if (h->next_frame >= 0) {
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) ^ ((0x65244f7d) - (rb_state4.var_1));
        #endif
        #ifdef RACEBENCH_BUG_7
        rb_state7.var_1 = (rb_state7.var_1) - (630 < rb_input_size ? (uint32_t)rb_input[630] : 0xb4ed3d8f);
        #endif
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_31 = (rb_state9.var_31) + (4021 < rb_input_size ? (uint32_t)rb_input[4021] : 0x438fbe3c);
        #endif
        x264_pthread_join(h->tid, &stuff);
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_0) == (0x65244f92)) {
            pthread_mutex_lock(&(rb_state4.lock_12));
            rb_state4.var_4 = 0x25fe7ef0;
            rb_state4.var_5 = (rb_state4.var_5) - (rb_state4.var_4);
            rb_state4.var_6 = (rb_state4.var_6) ^ (((0xaeb36011) - (rb_state4.var_3)) + (0x7399d372));
            rb_state4.var_7 = (rb_state4.var_7) ^ (((rb_state4.var_2) == (0x0)) ? (rb_state4.var_5) : (0xe0ae853f));
            rb_state4.var_8 = (rb_state4.var_8) - (((((0xec4ce1cd) ^ (rb_state4.var_6)) - (0xf9855c13)) ^ (rb_state4.var_4)) - (rb_state4.var_7));
            rb_state4.var_3 = (rb_state4.var_3) - (rb_state4.var_8);
            rb_state4.var_2 = rb_state4.var_3;
            pthread_mutex_unlock(&(rb_state4.lock_12));
        }
        if ((rb_state4.var_0) == (0x65244f92)) {
            pthread_mutex_lock(&(rb_state4.lock_12));
            if (!((rb_state4.var_2) == (rb_state4.var_3))) {
                racebench_trigger(4);
            }
            pthread_mutex_unlock(&(rb_state4.lock_12));
        }
        #endif
        #ifdef RACEBENCH_BUG_10
        if ((rb_state10.var_0) == (0x97eb38b4)) {
            rb_state10.var_15 = 0x626043c0;
            rb_state10.var_16 = (rb_state10.var_16) - (((rb_state10.var_15) == (0x626043c0)) ? (rb_state10.var_12) : (0xeba0d1ca));
            rb_state10.var_17 = (rb_state10.var_17) ^ (((0x7bcea161) - (rb_state10.var_11)) + (0xd90b8302));
            rb_state10.var_18 = (rb_state10.var_18) + (rb_state10.var_16);
            rb_state10.var_19 = (rb_state10.var_19) - (rb_state10.var_17);
            rb_state10.var_20 = (rb_state10.var_20) ^ (rb_state10.var_18);
            rb_state10.var_21 = (rb_state10.var_21) + (((((0x9665b207) + (0x74f86cc1)) + (rb_state10.var_20)) ^ (rb_state10.var_13)) ^ (rb_state10.var_19));
            rb_state10.var_14 = (rb_state10.var_14) - (rb_state10.var_21);
            rb_state10.var_3 = rb_state10.var_14;
        }
        if ((rb_state10.var_0) == (0x97eb38b4)) {
            pthread_mutex_lock(&(rb_state10.lock_38));
            rb_state10.var_23 = 0x877a5469;
            rb_state10.var_24 = 0x99b09dca;
            rb_state10.var_25 = (rb_state10.var_25) - (rb_state10.var_15);
            rb_state10.var_26 = (rb_state10.var_26) + ((0x968a6e1c) ^ (0x985b7bb6));
            rb_state10.var_27 = (rb_state10.var_27) - (rb_state10.var_24);
            rb_state10.var_28 = (rb_state10.var_28) ^ (((((0x92fee32) - (rb_state10.var_14)) - (rb_state10.var_16)) - (rb_state10.var_23)) + (rb_state10.var_17));
            rb_state10.var_29 = (rb_state10.var_29) ^ (((((0x9e42646b) - (rb_state10.var_26)) + (rb_state10.var_18)) - (rb_state10.var_25)) - (0x3884d4));
            rb_state10.var_30 = (rb_state10.var_30) ^ (((0x46dac126) - (rb_state10.var_27)) ^ (rb_state10.var_19));
            rb_state10.var_31 = (rb_state10.var_31) ^ (((0xf4a1d727) - (rb_state10.var_28)) - (0x22b602fa));
            rb_state10.var_32 = (rb_state10.var_32) ^ (((((0xf8a1130) + (rb_state10.var_29)) + (rb_state10.var_30)) + (0x8c48a376)) + (rb_state10.var_20));
            rb_state10.var_33 = (rb_state10.var_33) + (rb_state10.var_31);
            rb_state10.var_34 = (rb_state10.var_34) + (((0xd4345945) + (rb_state10.var_32)) - (0xf4cc45d4));
            rb_state10.var_35 = (rb_state10.var_35) + (((rb_state10.var_21) == (0xa07bc555)) ? (rb_state10.var_33) : (0x5a1eb45d));
            rb_state10.var_36 = (rb_state10.var_36) ^ (((rb_state10.var_22) == (0x0)) ? (rb_state10.var_34) : (0xfb00c4c7));
            rb_state10.var_37 = (rb_state10.var_37) + (((rb_state10.var_35) == (0x5ae050ac)) ? (rb_state10.var_36) : (0x3a33328b));
            rb_state10.var_22 = (rb_state10.var_22) - (rb_state10.var_37);
            rb_state10.var_3 = rb_state10.var_22;
            pthread_mutex_unlock(&(rb_state10.lock_38));
        }
        #endif
        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + (0x3b7ad831);
        #endif
        ret |= h->next_args->status;
        h->in_progress = 0;
    }

    if (h->next_frame == i_frame) {
        #ifdef RACEBENCH_BUG_9
        if ((rb_state9.var_26) == (0x93fece4)) {
            pthread_mutex_lock(&(rb_state9.lock_65));
            rb_state9.var_42 = 0x8052883c;
            rb_state9.var_43 = 0x83827aca;
            rb_state9.var_44 = (rb_state9.var_44) + (((rb_state9.var_29) == (0xa0d8b46d)) ? (rb_state9.var_28) : (0xc8da4786));
            rb_state9.var_45 = (rb_state9.var_45) + (1449 < rb_input_size ? (uint32_t)rb_input[1449] : 0x5048af13);
            rb_state9.var_46 = (rb_state9.var_46) - (((((0x9ffc92d1) ^ (0x8c0cd0c2)) + (rb_state9.var_32)) ^ (rb_state9.var_42)) + (rb_state9.var_27));
            rb_state9.var_47 = (rb_state9.var_47) - (((((0x5e71dc28) + (rb_state9.var_43)) - (rb_state9.var_44)) - (0x96699ce2)) ^ (rb_state9.var_33));
            rb_state9.var_48 = (rb_state9.var_48) - (((0x51894032) - (rb_state9.var_45)) - (0x38a9aa77));
            rb_state9.var_49 = (rb_state9.var_49) ^ (rb_state9.var_46);
            rb_state9.var_50 = (rb_state9.var_50) - (((0xbd828656) + (rb_state9.var_47)) - (0xc091e31c));
            rb_state9.var_51 = (rb_state9.var_51) ^ (((((0x6bfe5d1f) + (rb_state9.var_48)) ^ (rb_state9.var_49)) - (0x572825d0)) + (rb_state9.var_34));
            rb_state9.var_52 = (rb_state9.var_52) ^ (((((0x5b7819e9) ^ (rb_state9.var_50)) + (rb_state9.var_36)) - (rb_state9.var_51)) ^ (rb_state9.var_35));
            rb_state9.var_41 = (rb_state9.var_41) + (rb_state9.var_52);
            rb_state9.var_40 = rb_state9.var_41;
            pthread_mutex_unlock(&(rb_state9.lock_65));
        }
        if ((rb_state9.var_26) == (0x93fece4)) {
            if (!((rb_state9.var_40) == (rb_state9.var_41))) {
                racebench_trigger(9);
            }
        }
        #endif
        XCHG(x264_picture_t, *p_pic, h->pic);
    } else {
        ret |= h->p_read_frame(p_pic, h->p_handle, i_frame);
    }

    if (!h->frame_total || i_frame + 1 < h->frame_total) {
        h->next_frame = h->next_args->i_frame = i_frame + 1;
        h->next_args->pic = &h->pic;
        x264_pthread_create(&h->tid, NULL, (void *)read_frame_thread_int, h->next_args);
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (rb_state18.var_0);
        #endif
        h->in_progress = 1;
    } else {
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_2 = (rb_state12.var_2) - (0x29b2964d);
        #endif
        h->next_frame = -1;
    }

    return ret;
}

int close_file_thread(hnd_t handle) {
    thread_input_t *h = handle;
    h->p_close_infile(h->p_handle);
    x264_picture_clean(&h->pic);
    if (h->in_progress) {
        x264_pthread_join(h->tid, NULL);
    }
    free(h->next_args);
    free(h);
    return 0;
}
#endif

int open_file_bsf(char *psz_filename, hnd_t *p_handle) {
    if ((*p_handle = fopen(psz_filename, "w+b")) == NULL) {
        return -1;
    }

    return 0;
}

int set_param_bsf(hnd_t handle, x264_param_t *p_param) {
    return 0;
}

int write_nalu_bsf(hnd_t handle, uint8_t *p_nalu, int i_size) {
    if (fwrite(p_nalu, i_size, 1, (FILE *)handle) > 0) {
        return i_size;
    }
    return -1;
}

int set_eop_bsf(hnd_t handle, x264_picture_t *p_picture) {
    return 0;
}

int close_file_bsf(hnd_t handle) {
    if ((handle == NULL) || (handle == stdout)) {
        return 0;
    }

    return fclose((FILE *)handle);
}

#ifdef MP4_OUTPUT

typedef struct {
    GF_ISOFile *p_file;
    GF_AVCConfig *p_config;
    GF_ISOSample *p_sample;
    int i_track;
    uint32_t i_descidx;
    int i_time_inc;
    int i_time_res;
    int i_numframe;
    int i_init_delay;
    uint8_t b_sps;
    uint8_t b_pps;
} mp4_t;

static void recompute_bitrate_mp4(GF_ISOFile *p_file, int i_track) {
    u32 i, count, di, timescale, time_wnd, rate;
    u64 offset;
    Double br;
    GF_ESD *esd;

    esd = gf_isom_get_esd(p_file, i_track, 1);
    if (!esd)
        return;

    esd->decoderConfig->avgBitrate = 0;
    esd->decoderConfig->maxBitrate = 0;
    rate = time_wnd = 0;

    timescale = gf_isom_get_media_timescale(p_file, i_track);
    count = gf_isom_get_sample_count(p_file, i_track);
    for (i = 0; i < count; i++) {
        GF_ISOSample *samp = gf_isom_get_sample_info(p_file, i_track, i + 1, &di, &offset);

        if (samp->dataLength > esd->decoderConfig->bufferSizeDB)
            esd->decoderConfig->bufferSizeDB = samp->dataLength;

        if (esd->decoderConfig->bufferSizeDB < samp->dataLength)
            esd->decoderConfig->bufferSizeDB = samp->dataLength;
        esd->decoderConfig->avgBitrate += samp->dataLength;
        rate += samp->dataLength;
        if (samp->DTS > time_wnd + timescale) {
            if (rate > esd->decoderConfig->maxBitrate)
                esd->decoderConfig->maxBitrate = rate;
            time_wnd = samp->DTS;
            rate = 0;
        }

        gf_isom_sample_del(&samp);
    }

    br = (Double)(s64)gf_isom_get_media_duration(p_file, i_track);
    br /= timescale;
    esd->decoderConfig->avgBitrate = (u32)(esd->decoderConfig->avgBitrate / br);

    esd->decoderConfig->avgBitrate *= 8;
    esd->decoderConfig->maxBitrate *= 8;

    gf_isom_change_mpeg4_description(p_file, i_track, 1, esd);
    gf_odf_desc_del((GF_Descriptor *)esd);
}

int close_file_mp4(hnd_t handle) {
    mp4_t *p_mp4 = (mp4_t *)handle;

    if (p_mp4 == NULL)
        return 0;

    if (p_mp4->p_config)
        gf_odf_avc_cfg_del(p_mp4->p_config);

    if (p_mp4->p_sample) {
        if (p_mp4->p_sample->data)
            free(p_mp4->p_sample->data);

        gf_isom_sample_del(&p_mp4->p_sample);
    }

    if (p_mp4->p_file) {
        recompute_bitrate_mp4(p_mp4->p_file, p_mp4->i_track);
        gf_isom_set_pl_indication(p_mp4->p_file, GF_ISOM_PL_VISUAL, 0x15);
        gf_isom_set_storage_mode(p_mp4->p_file, GF_ISOM_STORE_FLAT);
        gf_isom_close(p_mp4->p_file);
    }

    free(p_mp4);

    return 0;
}

int open_file_mp4(char *psz_filename, hnd_t *p_handle) {
    mp4_t *p_mp4;

    *p_handle = NULL;

    if ((p_mp4 = (mp4_t *)malloc(sizeof(mp4_t))) == NULL)
        return -1;

    memset(p_mp4, 0, sizeof(mp4_t));
    p_mp4->p_file = gf_isom_open(psz_filename, GF_ISOM_OPEN_WRITE, NULL);

    if ((p_mp4->p_sample = gf_isom_sample_new()) == NULL) {
        close_file_mp4(p_mp4);
        return -1;
    }

    gf_isom_set_brand_info(p_mp4->p_file, GF_ISOM_BRAND_AVC1, 0);

    *p_handle = p_mp4;

    return 0;
}

int set_param_mp4(hnd_t handle, x264_param_t *p_param) {
    mp4_t *p_mp4 = (mp4_t *)handle;

    p_mp4->i_track = gf_isom_new_track(p_mp4->p_file, 0, GF_ISOM_MEDIA_VISUAL, p_param->i_fps_num);

    p_mp4->p_config = gf_odf_avc_cfg_new();
    gf_isom_avc_config_new(p_mp4->p_file, p_mp4->i_track, p_mp4->p_config, NULL, NULL, &p_mp4->i_descidx);

    gf_isom_set_track_enabled(p_mp4->p_file, p_mp4->i_track, 1);

    gf_isom_set_visual_info(p_mp4->p_file, p_mp4->i_track, p_mp4->i_descidx, p_param->i_width, p_param->i_height);

    if (p_param->vui.i_sar_width && p_param->vui.i_sar_height) {
        uint64_t dw = p_param->i_width << 16;
        uint64_t dh = p_param->i_height << 16;
        double sar = (double)p_param->vui.i_sar_width / p_param->vui.i_sar_height;
        if (sar > 1.0)
            dw *= sar;
        else
            dh /= sar;
        gf_isom_set_track_layout_info(p_mp4->p_file, p_mp4->i_track, dw, dh, 0, 0, 0);
    }

    p_mp4->p_sample->data = (char *)malloc(p_param->i_width * p_param->i_height * 3 / 2);
    if (p_mp4->p_sample->data == NULL)
        return -1;

    p_mp4->i_time_res = p_param->i_fps_num;
    p_mp4->i_time_inc = p_param->i_fps_den;
    p_mp4->i_init_delay = p_param->i_bframe ? (p_param->b_bframe_pyramid ? 2 : 1) : 0;
    p_mp4->i_init_delay *= p_mp4->i_time_inc;
    fprintf(stderr, "mp4 [info]: initial delay %d (scale %d)\n", p_mp4->i_init_delay, p_mp4->i_time_res);

    return 0;
}

int write_nalu_mp4(hnd_t handle, uint8_t *p_nalu, int i_size) {
    mp4_t *p_mp4 = (mp4_t *)handle;
    GF_AVCConfigSlot *p_slot;
    uint8_t type = p_nalu[4] & 0x1f;
    int psize;

    switch (type) {

    case 0x07:
        if (!p_mp4->b_sps) {
            p_mp4->p_config->configurationVersion = 1;
            p_mp4->p_config->AVCProfileIndication = p_nalu[5];
            p_mp4->p_config->profile_compatibility = p_nalu[6];
            p_mp4->p_config->AVCLevelIndication = p_nalu[7];
            p_slot = (GF_AVCConfigSlot *)malloc(sizeof(GF_AVCConfigSlot));
            p_slot->size = i_size - 4;
            p_slot->data = (char *)malloc(p_slot->size);
            memcpy(p_slot->data, p_nalu + 4, i_size - 4);
            gf_list_add(p_mp4->p_config->sequenceParameterSets, p_slot);
            p_slot = NULL;
            p_mp4->b_sps = 1;
        }
        break;

    case 0x08:
        if (!p_mp4->b_pps) {
            p_slot = (GF_AVCConfigSlot *)malloc(sizeof(GF_AVCConfigSlot));
            p_slot->size = i_size - 4;
            p_slot->data = (char *)malloc(p_slot->size);
            memcpy(p_slot->data, p_nalu + 4, i_size - 4);
            gf_list_add(p_mp4->p_config->pictureParameterSets, p_slot);
            p_slot = NULL;
            p_mp4->b_pps = 1;
            if (p_mp4->b_sps)
                gf_isom_avc_config_update(p_mp4->p_file, p_mp4->i_track, 1, p_mp4->p_config);
        }
        break;

    case 0x1:
    case 0x5:
    case 0x6:
        psize = i_size - 4;
        memcpy(p_mp4->p_sample->data + p_mp4->p_sample->dataLength, p_nalu, i_size);
        p_mp4->p_sample->data[p_mp4->p_sample->dataLength + 0] = (psize >> 24) & 0xff;
        p_mp4->p_sample->data[p_mp4->p_sample->dataLength + 1] = (psize >> 16) & 0xff;
        p_mp4->p_sample->data[p_mp4->p_sample->dataLength + 2] = (psize >> 8) & 0xff;
        p_mp4->p_sample->data[p_mp4->p_sample->dataLength + 3] = (psize >> 0) & 0xff;
        p_mp4->p_sample->dataLength += i_size;
        break;
    }

    return i_size;
}

int set_eop_mp4(hnd_t handle, x264_picture_t *p_picture) {
    mp4_t *p_mp4 = (mp4_t *)handle;
    uint64_t dts = (uint64_t)p_mp4->i_numframe * p_mp4->i_time_inc;
    uint64_t pts = (uint64_t)p_picture->i_pts;
    int32_t offset = p_mp4->i_init_delay + pts - dts;

    p_mp4->p_sample->IsRAP = p_picture->i_type == X264_TYPE_IDR ? 1 : 0;
    p_mp4->p_sample->DTS = dts;
    p_mp4->p_sample->CTS_Offset = offset;
    gf_isom_add_sample(p_mp4->p_file, p_mp4->i_track, p_mp4->i_descidx, p_mp4->p_sample);

    p_mp4->p_sample->dataLength = 0;
    p_mp4->i_numframe++;

    return 0;
}

#endif

typedef struct {
    mk_Writer *w;

    uint8_t *sps, *pps;
    int sps_len, pps_len;

    int width, height, d_width, d_height;

    int64_t frame_duration;
    int fps_num;

    int b_header_written;
    char b_writing_frame;
} mkv_t;

static int write_header_mkv(mkv_t *p_mkv) {
    int ret;
    uint8_t *avcC;
    int avcC_len;

    if (p_mkv->sps == NULL || p_mkv->pps == NULL || p_mkv->width == 0 || p_mkv->height == 0 || p_mkv->d_width == 0 || p_mkv->d_height == 0) {
        return -1;
    }

    avcC_len = 5 + 1 + 2 + p_mkv->sps_len + 1 + 2 + p_mkv->pps_len;
    avcC = malloc(avcC_len);
    if (avcC == NULL) {
        return -1;
    }

    avcC[0] = 1;
    avcC[1] = p_mkv->sps[1];
    avcC[2] = p_mkv->sps[2];
    avcC[3] = p_mkv->sps[3];
    avcC[4] = 0xff;
    avcC[5] = 0xe1;

    avcC[6] = p_mkv->sps_len >> 8;
    avcC[7] = p_mkv->sps_len;

    memcpy(avcC + 8, p_mkv->sps, p_mkv->sps_len);

    avcC[8 + p_mkv->sps_len] = 1;
    avcC[9 + p_mkv->sps_len] = p_mkv->pps_len >> 8;
    avcC[10 + p_mkv->sps_len] = p_mkv->pps_len;

    memcpy(avcC + 11 + p_mkv->sps_len, p_mkv->pps, p_mkv->pps_len);

    ret = mk_writeHeader(p_mkv->w, "x264", "V_MPEG4/ISO/AVC", avcC, avcC_len, p_mkv->frame_duration, 50000, p_mkv->width, p_mkv->height, p_mkv->d_width, p_mkv->d_height);

    free(avcC);

    p_mkv->b_header_written = 1;

    return ret;
}

int open_file_mkv(char *psz_filename, hnd_t *p_handle) {
    mkv_t *p_mkv;

    *p_handle = NULL;

    p_mkv = malloc(sizeof(*p_mkv));
    if (p_mkv == NULL) {
        return -1;
    }

    memset(p_mkv, 0, sizeof(*p_mkv));

    p_mkv->w = mk_createWriter(psz_filename);
    if (p_mkv->w == NULL) {
        free(p_mkv);
        return -1;
    }

    *p_handle = p_mkv;

    return 0;
}

int set_param_mkv(hnd_t handle, x264_param_t *p_param) {
    mkv_t *p_mkv = handle;
    int64_t dw, dh;

    if (p_param->i_fps_num > 0) {
        p_mkv->frame_duration = (int64_t)p_param->i_fps_den * (int64_t)1000000000 / p_param->i_fps_num;
        p_mkv->fps_num = p_param->i_fps_num;
    } else {
        p_mkv->frame_duration = 0;
        p_mkv->fps_num = 1;
    }

    p_mkv->width = p_param->i_width;
    p_mkv->height = p_param->i_height;

    if (p_param->vui.i_sar_width && p_param->vui.i_sar_height) {
        dw = (int64_t)p_param->i_width * p_param->vui.i_sar_width;
        dh = (int64_t)p_param->i_height * p_param->vui.i_sar_height;
    } else {
        dw = p_param->i_width;
        dh = p_param->i_height;
    }

    if (dw > 0 && dh > 0) {
        int64_t x = gcd(dw, dh);
        dw /= x;
        dh /= x;
    }

    p_mkv->d_width = (int)dw;
    p_mkv->d_height = (int)dh;

    return 0;
}

int write_nalu_mkv(hnd_t handle, uint8_t *p_nalu, int i_size) {
    mkv_t *p_mkv = handle;
    uint8_t type = p_nalu[4] & 0x1f;
    uint8_t dsize[4];
    int psize;

    switch (type) {

    case 0x07:
        if (!p_mkv->sps) {
            p_mkv->sps = malloc(i_size - 4);
            if (p_mkv->sps == NULL) {
                return -1;
            }
            p_mkv->sps_len = i_size - 4;
            memcpy(p_mkv->sps, p_nalu + 4, i_size - 4);
        }
        break;

    case 0x08:
        if (!p_mkv->pps) {
            p_mkv->pps = malloc(i_size - 4);
            if (p_mkv->pps == NULL) {
                return -1;
            }
            p_mkv->pps_len = i_size - 4;
            memcpy(p_mkv->pps, p_nalu + 4, i_size - 4);
        }
        break;

    case 0x1:
    case 0x5:
    case 0x6:
        if (!p_mkv->b_writing_frame) {
            if (mk_startFrame(p_mkv->w) < 0) {
                return -1;
            }
            p_mkv->b_writing_frame = 1;
        }
        psize = i_size - 4;
        dsize[0] = psize >> 24;
        dsize[1] = psize >> 16;
        dsize[2] = psize >> 8;
        dsize[3] = psize;
        if (mk_addFrameData(p_mkv->w, dsize, 4) < 0 || mk_addFrameData(p_mkv->w, p_nalu + 4, i_size - 4) < 0) {
            return -1;
        }
        break;

    default:
        break;
    }

    if (!p_mkv->b_header_written && p_mkv->pps && p_mkv->sps && write_header_mkv(p_mkv) < 0) {
        return -1;
    }

    return i_size;
}

int set_eop_mkv(hnd_t handle, x264_picture_t *p_picture) {
    mkv_t *p_mkv = handle;
    int64_t i_stamp = (int64_t)(p_picture->i_pts * 1e9 / p_mkv->fps_num);

    p_mkv->b_writing_frame = 0;

    return mk_setFrameFlags(p_mkv->w, i_stamp, p_picture->i_type == X264_TYPE_IDR);
}

int close_file_mkv(hnd_t handle) {
    mkv_t *p_mkv = handle;
    int ret;

    if (p_mkv->sps) {
        free(p_mkv->sps);
    }
    if (p_mkv->pps) {
        free(p_mkv->pps);
    }

    ret = mk_close(p_mkv->w);

    free(p_mkv);

    return ret;
}
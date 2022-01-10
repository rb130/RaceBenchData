#include "racebench_bugs.h"


#include "common.h"

#define ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))

x264_frame_t *x264_frame_new(x264_t *h) {
    x264_frame_t *frame = x264_malloc(sizeof(x264_frame_t));
    int i, j;

    int i_mb_count = h->mb.i_mb_count;
    int i_stride, i_width, i_lines;
    int i_padv = PADV << h->param.b_interlaced;
    int luma_plane_size;
    int align = h->param.cpu & X264_CPU_CACHELINE_64 ? 64 : h->param.cpu & X264_CPU_CACHELINE_32 ? 32 : 16;

    if (!frame) {
        return NULL;
    }

    memset(frame, 0, sizeof(x264_frame_t));

    i_width = ALIGN(h->param.i_width, 16);
    i_stride = ALIGN(i_width + 2 * PADH, align);
    i_lines = ALIGN(h->param.i_height, 16 << h->param.b_interlaced);

    frame->i_plane = 3;
    for (i = 0; i < 3; i++) {
        frame->i_stride[i] = i_stride >> !!i;
        frame->i_width[i] = i_width >> !!i;
        frame->i_lines[i] = i_lines >> !!i;
    }

    luma_plane_size = (frame->i_stride[0] * (frame->i_lines[0] + 2 * i_padv));
    for (i = 1; i < 3; i++) {
        CHECKED_MALLOC(frame->buffer[i], luma_plane_size / 4);
        frame->plane[i] = frame->buffer[i] + (frame->i_stride[i] * i_padv + PADH) / 2;
    }

    if (h->param.analyse.i_subpel_refine) {
        CHECKED_MALLOC(frame->buffer[0], 4 * luma_plane_size);
        for (i = 0; i < 4; i++) {
            frame->filtered[i] = frame->buffer[0] + i * luma_plane_size + frame->i_stride[0] * i_padv + PADH;
        }
        frame->plane[0] = frame->filtered[0];
    } else {
        CHECKED_MALLOC(frame->buffer[0], luma_plane_size);
        frame->plane[0] = frame->buffer[0] + frame->i_stride[0] * i_padv + PADH;
    }

    if (h->frames.b_have_lowres) {
        frame->i_width_lowres = frame->i_width[0] / 2;
        frame->i_stride_lowres = ALIGN(frame->i_width_lowres + 2 * PADH, align);
        frame->i_lines_lowres = frame->i_lines[0] / 2;

        luma_plane_size = frame->i_stride_lowres * (frame->i_lines[0] / 2 + 2 * i_padv);

        CHECKED_MALLOC(frame->buffer_lowres[0], 4 * luma_plane_size);
        for (i = 0; i < 4; i++) {
            frame->lowres[i] = frame->buffer_lowres[0] + (frame->i_stride_lowres * i_padv + PADH) + i * luma_plane_size;
        }

        for (j = 0; j <= !!h->param.i_bframe; j++) {
            for (i = 0; i <= h->param.i_bframe; i++) {
                CHECKED_MALLOC(frame->lowres_mvs[j][i], 2 * h->mb.i_mb_count * sizeof(int16_t));
                memset(frame->lowres_mvs[j][i], 0, 2 * h->mb.i_mb_count * sizeof(int16_t));
                CHECKED_MALLOC(frame->lowres_mv_costs[j][i], h->mb.i_mb_count * sizeof(int));
            }
        }
    }

    if (h->param.analyse.i_me_method >= X264_ME_ESA) {
        CHECKED_MALLOC(frame->buffer[3], 2 * frame->i_stride[0] * (frame->i_lines[0] + 2 * i_padv) * sizeof(uint16_t));
        frame->integral = (uint16_t *)frame->buffer[3] + frame->i_stride[0] * i_padv + PADH;
    }

    frame->i_poc = -1;
    frame->i_type = X264_TYPE_AUTO;
    frame->i_qpplus1 = 0;
    frame->i_pts = -1;
    frame->i_frame = -1;
    frame->i_frame_num = -1;
    frame->i_lines_completed = -1;

    CHECKED_MALLOC(frame->mb_type, i_mb_count * sizeof(int8_t));
    CHECKED_MALLOC(frame->mv[0], 2 * 16 * i_mb_count * sizeof(int16_t));
    CHECKED_MALLOC(frame->ref[0], 4 * i_mb_count * sizeof(int8_t));
    CHECKED_MALLOC(frame->i_intra_cost, i_mb_count * sizeof(uint16_t));
    if (h->param.i_bframe) {
        CHECKED_MALLOC(frame->mv[1], 2 * 16 * i_mb_count * sizeof(int16_t));
        CHECKED_MALLOC(frame->ref[1], 4 * i_mb_count * sizeof(int8_t));
    } else {
        frame->mv[1] = NULL;
        frame->ref[1] = NULL;
    }

    CHECKED_MALLOC(frame->i_row_bits, i_lines / 16 * sizeof(int));
    CHECKED_MALLOC(frame->i_row_qp, i_lines / 16 * sizeof(int));
    for (i = 0; i < h->param.i_bframe + 2; i++) {
        for (j = 0; j < h->param.i_bframe + 2; j++)
            CHECKED_MALLOC(frame->i_row_satds[i][j], i_lines / 16 * sizeof(int));
    }

    if (h->param.rc.i_aq_mode) {
        CHECKED_MALLOC(frame->f_qp_offset, h->mb.i_mb_count * sizeof(float));
        if (h->frames.b_have_lowres)
            CHECKED_MALLOC(frame->i_inv_qscale_factor, h->mb.i_mb_count * sizeof(uint16_t));
    }

    x264_pthread_mutex_init(&frame->mutex, NULL);
    x264_pthread_cond_init(&frame->cv, NULL);

    return frame;

fail:
    x264_frame_delete(frame);
    return NULL;
}

void x264_frame_delete(x264_frame_t *frame) {
    int i, j;
    for (i = 0; i < 4; i++) {
        x264_free(frame->buffer[i]);
    }
    for (i = 0; i < 4; i++) {
        x264_free(frame->buffer_lowres[i]);
    }
    for (i = 0; i < X264_BFRAME_MAX + 2; i++) {
        for (j = 0; j < X264_BFRAME_MAX + 2; j++) {
            x264_free(frame->i_row_satds[i][j]);
        }
    }
    for (j = 0; j < 2; j++) {
        for (i = 0; i <= X264_BFRAME_MAX; i++) {
            x264_free(frame->lowres_mvs[j][i]);
            x264_free(frame->lowres_mv_costs[j][i]);
        }
    }
    x264_free(frame->f_qp_offset);
    x264_free(frame->i_inv_qscale_factor);
    x264_free(frame->i_intra_cost);
    x264_free(frame->i_row_bits);
    x264_free(frame->i_row_qp);
    x264_free(frame->mb_type);
    x264_free(frame->mv[0]);
    x264_free(frame->mv[1]);
    x264_free(frame->ref[0]);
    x264_free(frame->ref[1]);
    x264_pthread_mutex_destroy(&frame->mutex);
    x264_pthread_cond_destroy(&frame->cv);
    x264_free(frame);
}

int x264_frame_copy_picture(x264_t *h, x264_frame_t *dst, x264_picture_t *src) {
    int i_csp = src->img.i_csp & X264_CSP_MASK;
    int i;
    if (i_csp != X264_CSP_I420 && i_csp != X264_CSP_YV12) {
        x264_log(h, X264_LOG_ERROR, "Arg invalid CSP\n");
        return -1;
    }

    dst->i_type = src->i_type;
    dst->i_qpplus1 = src->i_qpplus1;
    dst->i_pts = src->i_pts;

    for (i = 0; i < 3; i++) {
        int s = (i_csp == X264_CSP_YV12 && i) ? i ^ 3 : i;
        uint8_t *plane = src->img.plane[s];
        int stride = src->img.i_stride[s];
        int width = h->param.i_width >> !!i;
        int height = h->param.i_height >> !!i;
        if (src->img.i_csp & X264_CSP_VFLIP) {
            plane += (height - 1) * stride;
            stride = -stride;
        }
        h->mc.plane_copy(dst->plane[i], dst->i_stride[i], plane, stride, width, height);
    }
    return 0;
}

static void plane_expand_border(uint8_t *pix, int i_stride, int i_width, int i_height, int i_padh, int i_padv, int b_pad_top, int b_pad_bottom) {
#define PPIXEL(x, y) (pix + (x) + (y)*i_stride)
    int y;
    for (y = 0; y < i_height; y++) {

        memset(PPIXEL(-i_padh, y), PPIXEL(0, y)[0], i_padh);

        memset(PPIXEL(i_width, y), PPIXEL(i_width - 1, y)[0], i_padh);
    }

    if (b_pad_top) {
        for (y = 0; y < i_padv; y++) {
            memcpy(PPIXEL(-i_padh, -y - 1), PPIXEL(-i_padh, 0), i_width + 2 * i_padh);
        }
    }

    if (b_pad_bottom) {
        for (y = 0; y < i_padv; y++) {
            memcpy(PPIXEL(-i_padh, i_height + y), PPIXEL(-i_padh, i_height - 1), i_width + 2 * i_padh);
        }
    }
#undef PPIXEL
}

void x264_frame_expand_border(x264_t *h, x264_frame_t *frame, int mb_y, int b_end) {
    int i;
    int b_start = !mb_y;
    if (mb_y & h->sh.b_mbaff) {
        return;
    }
    for (i = 0; i < frame->i_plane; i++) {
        int stride = frame->i_stride[i];
        int width = 16 * h->sps->i_mb_width >> !!i;
        int height = (b_end ? 16 * (h->sps->i_mb_height - mb_y) >> h->sh.b_mbaff : 16) >> !!i;
        int padh = PADH >> !!i;
        int padv = PADV >> !!i;

        uint8_t *pix = frame->plane[i] + X264_MAX(0, (16 * mb_y - 4) * stride >> !!i);
        if (b_end && !b_start) {
            height += 4 >> (!!i + h->sh.b_mbaff);
        }
        if (h->sh.b_mbaff) {
            plane_expand_border(pix, stride * 2, width, height, padh, padv, b_start, b_end);
            plane_expand_border(pix + stride, stride * 2, width, height, padh, padv, b_start, b_end);
        } else {
            plane_expand_border(pix, stride, width, height, padh, padv, b_start, b_end);
        }
    }
}

void x264_frame_expand_border_filtered(x264_t *h, x264_frame_t *frame, int mb_y, int b_end) {

    int b_start = !mb_y;
    int stride = frame->i_stride[0];
    int width = 16 * h->sps->i_mb_width + 8;
    int height = b_end ? (16 * (h->sps->i_mb_height - mb_y) >> h->sh.b_mbaff) + 16 : 16;
    int padh = PADH - 4;
    int padv = PADV - 8;
    int i;
    for (i = 1; i < 4; i++) {

        uint8_t *pix = frame->filtered[i] + (16 * mb_y - (8 << h->sh.b_mbaff)) * stride - 4;
        if (h->sh.b_mbaff) {
            plane_expand_border(pix, stride * 2, width, height, padh, padv, b_start, b_end);
            plane_expand_border(pix + stride, stride * 2, width, height, padh, padv, b_start, b_end);
        } else {
            plane_expand_border(pix, stride, width, height, padh, padv, b_start, b_end);
        }
    }
}

void x264_frame_expand_border_lowres(x264_frame_t *frame) {
    int i;
    for (i = 0; i < 4; i++) {
        plane_expand_border(frame->lowres[i], frame->i_stride_lowres, frame->i_stride_lowres - 2 * PADH, frame->i_lines_lowres, PADH, PADV, 1, 1);
    }
}

void x264_frame_expand_border_mod16(x264_t *h, x264_frame_t *frame) {
    int i, y;
    for (i = 0; i < frame->i_plane; i++) {
        int i_subsample = i ? 1 : 0;
        int i_width = h->param.i_width >> i_subsample;
        int i_height = h->param.i_height >> i_subsample;
        int i_padx = (h->sps->i_mb_width * 16 - h->param.i_width) >> i_subsample;
        int i_pady = (h->sps->i_mb_height * 16 - h->param.i_height) >> i_subsample;

        if (i_padx) {
            for (y = 0; y < i_height; y++) {
                memset(&frame->plane[i][y * frame->i_stride[i] + i_width], frame->plane[i][y * frame->i_stride[i] + i_width - 1], i_padx);
            }
        }
        if (i_pady) {

            for (y = i_height; y < i_height + i_pady; y++) {
                memcpy(&frame->plane[i][y * frame->i_stride[i]], &frame->plane[i][(i_height - 1) * frame->i_stride[i]], i_width + i_padx);
            }
        }
    }
}

static void munge_cavlc_nnz_row(x264_t *h, int mb_y, uint8_t (*buf)[16]) {
    uint32_t(*src)[6] = (uint32_t(*)[6])h->mb.non_zero_count + mb_y * h->sps->i_mb_width;
    int8_t *transform = h->mb.mb_transform_size + mb_y * h->sps->i_mb_width;
    int x, nnz;
    for (x = 0; x < h->sps->i_mb_width; x++) {
        memcpy(buf + x, src + x, 16);
        if (transform[x]) {
            nnz = src[x][0] | src[x][1];
            src[x][0] = src[x][1] = ((uint16_t)nnz ? 0x0101 : 0) + (nnz >> 16 ? 0x01010000 : 0);
            nnz = src[x][2] | src[x][3];
            src[x][2] = src[x][3] = ((uint16_t)nnz ? 0x0101 : 0) + (nnz >> 16 ? 0x01010000 : 0);
        }
    }
}

static void restore_cavlc_nnz_row(x264_t *h, int mb_y, uint8_t (*buf)[16]) {
    uint8_t(*dst)[24] = h->mb.non_zero_count + mb_y * h->sps->i_mb_width;
    int x;
    for (x = 0; x < h->sps->i_mb_width; x++) {
        memcpy(dst + x, buf + x, 16);
    }
}

static void munge_cavlc_nnz(x264_t *h, int mb_y, uint8_t (*buf)[16], void (*func)(x264_t *, int, uint8_t (*)[16])) {
    func(h, mb_y, buf);
    if (mb_y > 0) {
        func(h, mb_y - 1, buf + h->sps->i_mb_width);
    }
    if (h->sh.b_mbaff) {
        func(h, mb_y + 1, buf + h->sps->i_mb_width * 2);
        if (mb_y > 0) {
            func(h, mb_y - 2, buf + h->sps->i_mb_width * 3);
        }
    }
}

static const uint8_t i_alpha_table[52 + 12 * 2] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    4,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    12,
    13,
    15,
    17,
    20,
    22,
    25,
    28,
    32,
    36,
    40,
    45,
    50,
    56,
    63,
    71,
    80,
    90,
    101,
    113,
    127,
    144,
    162,
    182,
    203,
    226,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
    255,
};
static const uint8_t i_beta_table[52 + 12 * 2] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    2,
    2,
    2,
    3,
    3,
    3,
    3,
    4,
    4,
    4,
    6,
    6,
    7,
    7,
    8,
    8,
    9,
    9,
    10,
    10,
    11,
    11,
    12,
    12,
    13,
    13,
    14,
    14,
    15,
    15,
    16,
    16,
    17,
    17,
    18,
    18,
    18,
    18,
    18,
    18,
    18,
    18,
    18,
    18,
    18,
    18,
    18,
    18,
};
static const int8_t i_tc0_table[52 + 12 * 2][4] = {
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 0},
    {-1, 0, 0, 1},
    {-1, 0, 0, 1},
    {-1, 0, 0, 1},
    {-1, 0, 0, 1},
    {-1, 0, 1, 1},
    {-1, 0, 1, 1},
    {-1, 1, 1, 1},
    {-1, 1, 1, 1},
    {-1, 1, 1, 1},
    {-1, 1, 1, 1},
    {-1, 1, 1, 2},
    {-1, 1, 1, 2},
    {-1, 1, 1, 2},
    {-1, 1, 1, 2},
    {-1, 1, 2, 3},
    {-1, 1, 2, 3},
    {-1, 2, 2, 3},
    {-1, 2, 2, 4},
    {-1, 2, 3, 4},
    {-1, 2, 3, 4},
    {-1, 3, 3, 5},
    {-1, 3, 4, 6},
    {-1, 3, 4, 6},
    {-1, 4, 5, 7},
    {-1, 4, 5, 8},
    {-1, 4, 6, 9},
    {-1, 5, 7, 10},
    {-1, 6, 8, 11},
    {-1, 6, 8, 13},
    {-1, 7, 10, 14},
    {-1, 8, 11, 16},
    {-1, 9, 12, 18},
    {-1, 10, 13, 20},
    {-1, 11, 15, 23},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
    {-1, 13, 17, 25},
};
#define alpha_table(x) i_alpha_table[(x) + 12]
#define beta_table(x) i_beta_table[(x) + 12]
#define tc0_table(x) i_tc0_table[(x) + 12]

static inline void deblock_luma_c(uint8_t *pix, int xstride, int ystride, int alpha, int beta, int8_t *tc0) {
    int i, d;
    for (i = 0; i < 4; i++) {
        if (tc0[i] < 0) {
            pix += 4 * ystride;
            continue;
        }
        for (d = 0; d < 4; d++) {
            const int p2 = pix[-3 * xstride];
            const int p1 = pix[-2 * xstride];
            const int p0 = pix[-1 * xstride];
            const int q0 = pix[0 * xstride];
            const int q1 = pix[1 * xstride];
            const int q2 = pix[2 * xstride];

            if (abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta) {
                int tc = tc0[i];
                int delta;
                if (abs(p2 - p0) < beta) {
                    pix[-2 * xstride] = p1 + x264_clip3(((p2 + ((p0 + q0 + 1) >> 1)) >> 1) - p1, -tc0[i], tc0[i]);
                    tc++;
                }
                if (abs(q2 - q0) < beta) {
                    pix[1 * xstride] = q1 + x264_clip3(((q2 + ((p0 + q0 + 1) >> 1)) >> 1) - q1, -tc0[i], tc0[i]);
                    tc++;
                }

                delta = x264_clip3((((q0 - p0) << 2) + (p1 - q1) + 4) >> 3, -tc, tc);
                pix[-1 * xstride] = x264_clip_uint8(p0 + delta);
                pix[0 * xstride] = x264_clip_uint8(q0 - delta);
            }
            pix += ystride;
        }
    }
}
static void deblock_v_luma_c(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0) {
    deblock_luma_c(pix, stride, 1, alpha, beta, tc0);
}
static void deblock_h_luma_c(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0) {
    deblock_luma_c(pix, 1, stride, alpha, beta, tc0);
}

static inline void deblock_chroma_c(uint8_t *pix, int xstride, int ystride, int alpha, int beta, int8_t *tc0) {
    int i, d;
    for (i = 0; i < 4; i++) {
        const int tc = tc0[i];
        if (tc <= 0) {
            pix += 2 * ystride;
            continue;
        }
        for (d = 0; d < 2; d++) {
            const int p1 = pix[-2 * xstride];
            const int p0 = pix[-1 * xstride];
            const int q0 = pix[0 * xstride];
            const int q1 = pix[1 * xstride];

            if (abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta) {
                int delta = x264_clip3((((q0 - p0) << 2) + (p1 - q1) + 4) >> 3, -tc, tc);
                pix[-1 * xstride] = x264_clip_uint8(p0 + delta);
                pix[0 * xstride] = x264_clip_uint8(q0 - delta);
            }
            pix += ystride;
        }
    }
}
static void deblock_v_chroma_c(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0) {
    deblock_chroma_c(pix, stride, 1, alpha, beta, tc0);
}
static void deblock_h_chroma_c(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0) {
    deblock_chroma_c(pix, 1, stride, alpha, beta, tc0);
}

static inline void deblock_luma_intra_c(uint8_t *pix, int xstride, int ystride, int alpha, int beta) {
    int d;
    for (d = 0; d < 16; d++) {
        const int p2 = pix[-3 * xstride];
        const int p1 = pix[-2 * xstride];
        const int p0 = pix[-1 * xstride];
        const int q0 = pix[0 * xstride];
        const int q1 = pix[1 * xstride];
        const int q2 = pix[2 * xstride];

        if (abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta) {
            if (abs(p0 - q0) < ((alpha >> 2) + 2)) {
                if (abs(p2 - p0) < beta) {
                    const int p3 = pix[-4 * xstride];
                    pix[-1 * xstride] = (p2 + 2 * p1 + 2 * p0 + 2 * q0 + q1 + 4) >> 3;
                    pix[-2 * xstride] = (p2 + p1 + p0 + q0 + 2) >> 2;
                    pix[-3 * xstride] = (2 * p3 + 3 * p2 + p1 + p0 + q0 + 4) >> 3;
                } else {
                    pix[-1 * xstride] = (2 * p1 + p0 + q1 + 2) >> 2;
                }
                if (abs(q2 - q0) < beta) {
                    const int q3 = pix[3 * xstride];
                    pix[0 * xstride] = (p1 + 2 * p0 + 2 * q0 + 2 * q1 + q2 + 4) >> 3;
                    pix[1 * xstride] = (p0 + q0 + q1 + q2 + 2) >> 2;
                    pix[2 * xstride] = (2 * q3 + 3 * q2 + q1 + q0 + p0 + 4) >> 3;
                } else {
                    pix[0 * xstride] = (2 * q1 + q0 + p1 + 2) >> 2;
                }
            } else {
                pix[-1 * xstride] = (2 * p1 + p0 + q1 + 2) >> 2;
                pix[0 * xstride] = (2 * q1 + q0 + p1 + 2) >> 2;
            }
        }
        pix += ystride;
    }
}
static void deblock_v_luma_intra_c(uint8_t *pix, int stride, int alpha, int beta) {
    deblock_luma_intra_c(pix, stride, 1, alpha, beta);
}
static void deblock_h_luma_intra_c(uint8_t *pix, int stride, int alpha, int beta) {
    deblock_luma_intra_c(pix, 1, stride, alpha, beta);
}

static inline void deblock_chroma_intra_c(uint8_t *pix, int xstride, int ystride, int alpha, int beta) {
    int d;
    for (d = 0; d < 8; d++) {
        const int p1 = pix[-2 * xstride];
        const int p0 = pix[-1 * xstride];
        const int q0 = pix[0 * xstride];
        const int q1 = pix[1 * xstride];

        if (abs(p0 - q0) < alpha && abs(p1 - p0) < beta && abs(q1 - q0) < beta) {
            pix[-1 * xstride] = (2 * p1 + p0 + q1 + 2) >> 2;
            pix[0 * xstride] = (2 * q1 + q0 + p1 + 2) >> 2;
        }
        pix += ystride;
    }
}
static void deblock_v_chroma_intra_c(uint8_t *pix, int stride, int alpha, int beta) {
    deblock_chroma_intra_c(pix, stride, 1, alpha, beta);
}
static void deblock_h_chroma_intra_c(uint8_t *pix, int stride, int alpha, int beta) {
    deblock_chroma_intra_c(pix, 1, stride, alpha, beta);
}

static inline void deblock_edge(x264_t *h, uint8_t *pix1, uint8_t *pix2, int i_stride, uint8_t bS[4], int i_qp, int b_chroma, x264_deblock_inter_t pf_inter) {
    const int index_a = i_qp + h->sh.i_alpha_c0_offset;
    const int alpha = alpha_table(index_a);
    const int beta = beta_table(i_qp + h->sh.i_beta_offset);
    int8_t tc[4];

    if (!alpha || !beta) {
        return;
    }

    tc[0] = tc0_table(index_a)[bS[0]] + b_chroma;
    tc[1] = tc0_table(index_a)[bS[1]] + b_chroma;
    tc[2] = tc0_table(index_a)[bS[2]] + b_chroma;
    tc[3] = tc0_table(index_a)[bS[3]] + b_chroma;

    pf_inter(pix1, i_stride, alpha, beta, tc);
    if (b_chroma) {
        pf_inter(pix2, i_stride, alpha, beta, tc);
    }
}

static inline void deblock_edge_intra(x264_t *h, uint8_t *pix1, uint8_t *pix2, int i_stride, uint8_t bS[4], int i_qp, int b_chroma, x264_deblock_intra_t pf_intra) {
    const int alpha = alpha_table(i_qp + h->sh.i_alpha_c0_offset);
    const int beta = beta_table(i_qp + h->sh.i_beta_offset);

    if (!alpha || !beta) {
        return;
    }

    pf_intra(pix1, i_stride, alpha, beta);
    if (b_chroma) {
        pf_intra(pix2, i_stride, alpha, beta);
    }
}

void x264_frame_deblock_row(x264_t *h, int mb_y) {
    const int s8x8 = 2 * h->mb.i_mb_stride;
    const int s4x4 = 4 * h->mb.i_mb_stride;
    const int b_interlaced = h->sh.b_mbaff;
    const int mvy_limit = 4 >> b_interlaced;
    const int qp_thresh = 15 - X264_MIN(h->sh.i_alpha_c0_offset, h->sh.i_beta_offset) - X264_MAX(0, h->param.analyse.i_chroma_qp_offset);
    const int no_sub8x8 = !(h->param.analyse.inter & X264_ANALYSE_PSUB8x8);
    int mb_x;
    int stridey = h->fdec->i_stride[0];
    int stride2y = stridey << b_interlaced;
    int strideuv = h->fdec->i_stride[1];
    int stride2uv = strideuv << b_interlaced;

    if (!h->pps->b_cabac && h->pps->b_transform_8x8_mode) {
        munge_cavlc_nnz(h, mb_y, h->mb.nnz_backup, munge_cavlc_nnz_row);
    }

    for (mb_x = 0; mb_x < h->sps->i_mb_width; mb_x += (~b_interlaced | mb_y) & 1, mb_y ^= b_interlaced) {
        const int mb_xy = mb_y * h->mb.i_mb_stride + mb_x;
        const int mb_8x8 = 2 * s8x8 * mb_y + 2 * mb_x;
        const int mb_4x4 = 4 * s4x4 * mb_y + 4 * mb_x;
        const int b_8x8_transform = h->mb.mb_transform_size[mb_xy];
        const int i_qp = h->mb.qp[mb_xy];
        int i_edge_end = (h->mb.type[mb_xy] == P_SKIP) ? 1 : 4;
        uint8_t *pixy = h->fdec->plane[0] + 16 * mb_y * stridey + 16 * mb_x;
        uint8_t *pixu = h->fdec->plane[1] + 8 * mb_y * strideuv + 8 * mb_x;
        uint8_t *pixv = h->fdec->plane[2] + 8 * mb_y * strideuv + 8 * mb_x;
        if (b_interlaced && (mb_y & 1)) {
            pixy -= 15 * stridey;
            pixu -= 7 * strideuv;
            pixv -= 7 * strideuv;
        }

        x264_prefetch_fenc(h, h->fdec, mb_x, mb_y);

        if (i_qp <= qp_thresh) {
            i_edge_end = 1;
        }

#define FILTER_DIR(intra, i_dir)                                                                                                                                 \
    {                                                                                                                                                            \
                                                                                                                                                                 \
        i_qpn = h->mb.qp[mbn_xy];                                                                                                                                \
        if (i_dir == 0) {                                                                                                                                        \
                                                                                                                                                                 \
            deblock_edge##intra(h, pixy + 4 * i_edge, NULL, stride2y, bS, (i_qp + i_qpn + 1) >> 1, 0, h->loopf.deblock_h_luma##intra);                           \
            if (!(i_edge & 1)) {                                                                                                                                 \
                                                                                                                                                                 \
                int i_qpc = (h->chroma_qp_table[i_qp] + h->chroma_qp_table[i_qpn] + 1) >> 1;                                                                     \
                deblock_edge##intra(h, pixu + 2 * i_edge, pixv + 2 * i_edge, stride2uv, bS, i_qpc, 1, h->loopf.deblock_h_chroma##intra);                         \
            }                                                                                                                                                    \
        } else {                                                                                                                                                 \
                                                                                                                                                                 \
            deblock_edge##intra(h, pixy + 4 * i_edge * stride2y, NULL, stride2y, bS, (i_qp + i_qpn + 1) >> 1, 0, h->loopf.deblock_v_luma##intra);                \
                                                                                                                                                                 \
            if (!(i_edge & 1)) {                                                                                                                                 \
                int i_qpc = (h->chroma_qp_table[i_qp] + h->chroma_qp_table[i_qpn] + 1) >> 1;                                                                     \
                deblock_edge##intra(h, pixu + 2 * i_edge * stride2uv, pixv + 2 * i_edge * stride2uv, stride2uv, bS, i_qpc, 1, h->loopf.deblock_v_chroma##intra); \
            }                                                                                                                                                    \
        }                                                                                                                                                        \
    }

#define DEBLOCK_STRENGTH(i_dir)                                                                                                                                                                                                                                                                                                                                                                   \
    {                                                                                                                                                                                                                                                                                                                                                                                             \
                                                                                                                                                                                                                                                                                                                                                                                                  \
        if (IS_INTRA(h->mb.type[mb_xy]) || IS_INTRA(h->mb.type[mbn_xy]))                                                                                                                                                                                                                                                                                                                          \
            *(uint32_t *)bS = 0x03030303;                                                                                                                                                                                                                                                                                                                                                         \
        else {                                                                                                                                                                                                                                                                                                                                                                                    \
            *(uint32_t *)bS = 0x00000000;                                                                                                                                                                                                                                                                                                                                                         \
            for (i = 0; i < 4; i++) {                                                                                                                                                                                                                                                                                                                                                             \
                int x = i_dir == 0 ? i_edge : i;                                                                                                                                                                                                                                                                                                                                                  \
                int y = i_dir == 0 ? i : i_edge;                                                                                                                                                                                                                                                                                                                                                  \
                int xn = i_dir == 0 ? (x - 1) & 0x03 : x;                                                                                                                                                                                                                                                                                                                                         \
                int yn = i_dir == 0 ? y : (y - 1) & 0x03;                                                                                                                                                                                                                                                                                                                                         \
                if (h->mb.non_zero_count[mb_xy][x + y * 4] != 0 || h->mb.non_zero_count[mbn_xy][xn + yn * 4] != 0)                                                                                                                                                                                                                                                                                \
                    bS[i] = 2;                                                                                                                                                                                                                                                                                                                                                                    \
                else if (!(i_edge & no_sub8x8)) {                                                                                                                                                                                                                                                                                                                                                 \
                    if ((i & no_sub8x8) && bS[i - 1] != 2)                                                                                                                                                                                                                                                                                                                                        \
                        bS[i] = bS[i - 1];                                                                                                                                                                                                                                                                                                                                                        \
                    else {                                                                                                                                                                                                                                                                                                                                                                        \
                                                                                                                                                                                                                                                                                                                                                                                                  \
                        int i8p = mb_8x8 + (x >> 1) + (y >> 1) * s8x8;                                                                                                                                                                                                                                                                                                                            \
                        int i8q = mbn_8x8 + (xn >> 1) + (yn >> 1) * s8x8;                                                                                                                                                                                                                                                                                                                         \
                        int i4p = mb_4x4 + x + y * s4x4;                                                                                                                                                                                                                                                                                                                                          \
                        int i4q = mbn_4x4 + xn + yn * s4x4;                                                                                                                                                                                                                                                                                                                                       \
                        if ((h->mb.ref[0][i8p] != h->mb.ref[0][i8q] || abs(h->mb.mv[0][i4p][0] - h->mb.mv[0][i4q][0]) >= 4 || abs(h->mb.mv[0][i4p][1] - h->mb.mv[0][i4q][1]) >= mvy_limit) || (h->sh.i_type == SLICE_TYPE_B && (h->mb.ref[1][i8p] != h->mb.ref[1][i8q] || abs(h->mb.mv[1][i4p][0] - h->mb.mv[1][i4q][0]) >= 4 || abs(h->mb.mv[1][i4p][1] - h->mb.mv[1][i4q][1]) >= mvy_limit))) { \
                            bS[i] = 1;                                                                                                                                                                                                                                                                                                                                                            \
                        }                                                                                                                                                                                                                                                                                                                                                                         \
                    }                                                                                                                                                                                                                                                                                                                                                                             \
                }                                                                                                                                                                                                                                                                                                                                                                                 \
            }                                                                                                                                                                                                                                                                                                                                                                                     \
        }                                                                                                                                                                                                                                                                                                                                                                                         \
    }

#define DEBLOCK_DIR(i_dir)                                                            \
    {                                                                                 \
        int i_edge = (i_dir ? (mb_y <= b_interlaced) : (mb_x == 0));                  \
        int i_qpn, i, mbn_xy, mbn_8x8, mbn_4x4;                                       \
        DECLARE_ALIGNED_4(uint8_t bS[4]);                                             \
        if (i_edge)                                                                   \
            i_edge += b_8x8_transform;                                                \
        else {                                                                        \
            mbn_xy = i_dir == 0 ? mb_xy - 1 : mb_xy - h->mb.i_mb_stride;              \
            mbn_8x8 = i_dir == 0 ? mb_8x8 - 2 : mb_8x8 - 2 * s8x8;                    \
            mbn_4x4 = i_dir == 0 ? mb_4x4 - 4 : mb_4x4 - 4 * s4x4;                    \
            if (b_interlaced && i_dir == 1) {                                         \
                mbn_xy -= h->mb.i_mb_stride;                                          \
                mbn_8x8 -= 2 * s8x8;                                                  \
                mbn_4x4 -= 4 * s4x4;                                                  \
            } else if (IS_INTRA(h->mb.type[mb_xy]) || IS_INTRA(h->mb.type[mbn_xy])) { \
                FILTER_DIR(_intra, i_dir);                                            \
                goto end##i_dir;                                                      \
            }                                                                         \
            DEBLOCK_STRENGTH(i_dir);                                                  \
            if (*(uint32_t *)bS)                                                      \
                FILTER_DIR(, i_dir);                                                  \
            end##i_dir : i_edge += b_8x8_transform + 1;                               \
        }                                                                             \
        mbn_xy = mb_xy;                                                               \
        mbn_8x8 = mb_8x8;                                                             \
        mbn_4x4 = mb_4x4;                                                             \
        for (; i_edge < i_edge_end; i_edge += b_8x8_transform + 1) {                  \
            DEBLOCK_STRENGTH(i_dir);                                                  \
            if (*(uint32_t *)bS)                                                      \
                FILTER_DIR(, i_dir);                                                  \
        }                                                                             \
    }

        DEBLOCK_DIR(0);
        DEBLOCK_DIR(1);
    }

    if (!h->pps->b_cabac && h->pps->b_transform_8x8_mode) {
        munge_cavlc_nnz(h, mb_y, h->mb.nnz_backup, restore_cavlc_nnz_row);
    }
}

void x264_frame_deblock(x264_t *h) {
    int mb_y;
    for (mb_y = 0; mb_y < h->sps->i_mb_height; mb_y += 1 + h->sh.b_mbaff) {
        x264_frame_deblock_row(h, mb_y);
    }
}

#ifdef HAVE_MMX
void x264_deblock_v_chroma_mmxext(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0);
void x264_deblock_h_chroma_mmxext(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0);
void x264_deblock_v_chroma_intra_mmxext(uint8_t *pix, int stride, int alpha, int beta);
void x264_deblock_h_chroma_intra_mmxext(uint8_t *pix, int stride, int alpha, int beta);

void x264_deblock_v_luma_sse2(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0);
void x264_deblock_h_luma_sse2(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0);
void x264_deblock_v_luma_intra_sse2(uint8_t *pix, int stride, int alpha, int beta);
void x264_deblock_h_luma_intra_sse2(uint8_t *pix, int stride, int alpha, int beta);
#ifdef ARCH_X86
void x264_deblock_h_luma_mmxext(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0);
void x264_deblock_v8_luma_mmxext(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0);
void x264_deblock_h_luma_intra_mmxext(uint8_t *pix, int stride, int alpha, int beta);
void x264_deblock_v8_luma_intra_mmxext(uint8_t *pix, int stride, int alpha, int beta);

static void x264_deblock_v_luma_mmxext(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0) {
    x264_deblock_v8_luma_mmxext(pix, stride, alpha, beta, tc0);
    x264_deblock_v8_luma_mmxext(pix + 8, stride, alpha, beta, tc0 + 2);
}
static void x264_deblock_v_luma_intra_mmxext(uint8_t *pix, int stride, int alpha, int beta) {
    x264_deblock_v8_luma_intra_mmxext(pix, stride, alpha, beta);
    x264_deblock_v8_luma_intra_mmxext(pix + 8, stride, alpha, beta);
}
#endif
#endif

#ifdef ARCH_PPC
void x264_deblock_v_luma_altivec(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0);
void x264_deblock_h_luma_altivec(uint8_t *pix, int stride, int alpha, int beta, int8_t *tc0);
#endif

void x264_deblock_init(int cpu, x264_deblock_function_t *pf) {
    pf->deblock_v_luma = deblock_v_luma_c;
    pf->deblock_h_luma = deblock_h_luma_c;
    pf->deblock_v_chroma = deblock_v_chroma_c;
    pf->deblock_h_chroma = deblock_h_chroma_c;
    pf->deblock_v_luma_intra = deblock_v_luma_intra_c;
    pf->deblock_h_luma_intra = deblock_h_luma_intra_c;
    pf->deblock_v_chroma_intra = deblock_v_chroma_intra_c;
    pf->deblock_h_chroma_intra = deblock_h_chroma_intra_c;

#ifdef HAVE_MMX
    if (cpu & X264_CPU_MMXEXT) {
        pf->deblock_v_chroma = x264_deblock_v_chroma_mmxext;
        pf->deblock_h_chroma = x264_deblock_h_chroma_mmxext;
        pf->deblock_v_chroma_intra = x264_deblock_v_chroma_intra_mmxext;
        pf->deblock_h_chroma_intra = x264_deblock_h_chroma_intra_mmxext;
#ifdef ARCH_X86
        pf->deblock_v_luma = x264_deblock_v_luma_mmxext;
        pf->deblock_h_luma = x264_deblock_h_luma_mmxext;
        pf->deblock_v_luma_intra = x264_deblock_v_luma_intra_mmxext;
        pf->deblock_h_luma_intra = x264_deblock_h_luma_intra_mmxext;
#endif
        if ((cpu & X264_CPU_SSE2) && !(cpu & X264_CPU_STACK_MOD4)) {
            pf->deblock_v_luma = x264_deblock_v_luma_sse2;
            pf->deblock_h_luma = x264_deblock_h_luma_sse2;
            pf->deblock_v_luma_intra = x264_deblock_v_luma_intra_sse2;
            pf->deblock_h_luma_intra = x264_deblock_h_luma_intra_sse2;
        }
    }
#endif

#ifdef ARCH_PPC
    if (cpu & X264_CPU_ALTIVEC) {
        pf->deblock_v_luma = x264_deblock_v_luma_altivec;
        pf->deblock_h_luma = x264_deblock_h_luma_altivec;
    }
#endif
}

void x264_frame_cond_broadcast(x264_frame_t *frame, int i_lines_completed) {
    x264_pthread_mutex_lock(&frame->mutex);
    frame->i_lines_completed = i_lines_completed;
    x264_pthread_cond_broadcast(&frame->cv);
    x264_pthread_mutex_unlock(&frame->mutex);
}

void x264_frame_cond_wait(x264_frame_t *frame, int i_lines_completed) {
    x264_pthread_mutex_lock(&frame->mutex);
    while (frame->i_lines_completed < i_lines_completed) {
        x264_pthread_cond_wait(&frame->cv, &frame->mutex);
    }
    x264_pthread_mutex_unlock(&frame->mutex);
}

void x264_frame_push(x264_frame_t **list, x264_frame_t *frame) {
    int i = 0;
    while (list[i]) {
        i++;
    }
    list[i] = frame;
}

x264_frame_t *x264_frame_pop(x264_frame_t **list) {
    x264_frame_t *frame;
    int i = 0;
    assert(list[0]);
    while (list[i + 1]) {
        i++;
    }
    frame = list[i];
    list[i] = NULL;
    return frame;
}

void x264_frame_unshift(x264_frame_t **list, x264_frame_t *frame) {
    int i = 0;
    while (list[i]) {
        i++;
    }
    while (i--) {
        list[i + 1] = list[i];
    }
    list[0] = frame;
}

x264_frame_t *x264_frame_shift(x264_frame_t **list) {
    x264_frame_t *frame = list[0];
    int i;
    for (i = 0; list[i]; i++) {
        list[i] = list[i + 1];
    }
    assert(frame);
    return frame;
}

void x264_frame_push_unused(x264_t *h, x264_frame_t *frame) {
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_28) == (0x770b8381)) {
        pthread_mutex_lock(&(rb_state10.lock_53));
        rb_state10.var_42 = 0x17ed97c2;
        rb_state10.var_43 = 0x4c26cbc6;
        rb_state10.var_44 = (rb_state10.var_44) ^ (((0xe9d4ce27) ^ (rb_state10.var_30)) + (rb_state10.var_42));
        rb_state10.var_45 = (rb_state10.var_45) + (rb_state10.var_31);
        rb_state10.var_46 = (rb_state10.var_46) - (((rb_state10.var_32) == (0xc3c226d2)) ? (rb_state10.var_43) : (0x182d30ef));
        rb_state10.var_47 = (rb_state10.var_47) ^ (((rb_state10.var_29) == (0xdc40ecbf)) ? (rb_state10.var_44) : (0x6934b651));
        rb_state10.var_48 = (rb_state10.var_48) ^ (rb_state10.var_45);
        rb_state10.var_49 = (rb_state10.var_49) + (rb_state10.var_46);
        rb_state10.var_50 = (rb_state10.var_50) - (((0x327ae57d) - (rb_state10.var_47)) ^ (rb_state10.var_33));
        rb_state10.var_51 = (rb_state10.var_51) + (((((0x76a73705) - (0x88a88ad9)) ^ (rb_state10.var_34)) - (rb_state10.var_48)) - (rb_state10.var_49));
        rb_state10.var_52 = (rb_state10.var_52) ^ (((((0x7f31dbaa) + (rb_state10.var_35)) + (rb_state10.var_51)) ^ (rb_state10.var_36)) + (rb_state10.var_50));
        rb_state10.var_40 = (rb_state10.var_40) + (rb_state10.var_52);
        rb_state10.var_41 = rb_state10.var_40;
        pthread_mutex_unlock(&(rb_state10.lock_53));
    }
    #endif
    assert(frame->i_reference_count > 0);
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) - (rb_state11.var_1);
    #endif
    frame->i_reference_count--;
    if (frame->i_reference_count == 0) {
        x264_frame_push(h->frames.unused, frame);
    }
    #ifdef RACEBENCH_BUG_10
    if ((rb_state10.var_41) == (0x3046f3c)) {
        pthread_mutex_lock(&(rb_state10.lock_76));
        rb_state10.var_57 = 0x3b0bed35;
        rb_state10.var_58 = 0x5d81bffe;
        rb_state10.var_59 = (rb_state10.var_59) - (((rb_state10.var_43) == (0x4c26cbc6)) ? (rb_state10.var_42) : (0x84994fac));
        rb_state10.var_60 = (rb_state10.var_60) + (rb_state10.var_45);
        rb_state10.var_61 = (rb_state10.var_61) - (((rb_state10.var_58) == (0x5d81bffe)) ? (rb_state10.var_57) : (0xed68e159));
        rb_state10.var_62 = (rb_state10.var_62) - (((((0x8fc09588) ^ (rb_state10.var_47)) ^ (rb_state10.var_59)) + (rb_state10.var_46)) ^ (rb_state10.var_60));
        rb_state10.var_63 = (rb_state10.var_63) ^ (((0x2e2852d4) ^ (rb_state10.var_61)) ^ (0x9b518d0d));
        rb_state10.var_64 = (rb_state10.var_64) - (((((0x4e674f15) - (0x6356c1f8)) ^ (rb_state10.var_62)) - (0x6f46d39)) ^ (rb_state10.var_63));
        rb_state10.var_56 = (rb_state10.var_56) + (rb_state10.var_64);
        rb_state10.var_55 = rb_state10.var_56;
        pthread_mutex_unlock(&(rb_state10.lock_76));
    }
    if ((rb_state10.var_41) == (0x3046f3c)) {
        if (!((rb_state10.var_55) == (rb_state10.var_56))) {
            racebench_trigger(10);
        }
    }
    #endif
    assert(h->frames.unused[sizeof(h->frames.unused) / sizeof(*h->frames.unused) - 1] == NULL);
}

x264_frame_t *x264_frame_pop_unused(x264_t *h) {
    x264_frame_t *frame;
    if (h->frames.unused[0]) {
        frame = x264_frame_pop(h->frames.unused);
    } else {
        frame = x264_frame_new(h);
    }
    assert(frame->i_reference_count == 0);
    frame->i_reference_count = 1;
    frame->b_intra_calculated = 0;
    return frame;
}

void x264_frame_sort(x264_frame_t **list, int b_dts) {
    int i, b_ok;
    do {
        b_ok = 1;
        for (i = 0; list[i + 1]; i++) {
            int dtype = list[i]->i_type - list[i + 1]->i_type;
            int dtime = list[i]->i_frame - list[i + 1]->i_frame;
            int swap = b_dts ? dtype > 0 || (dtype == 0 && dtime > 0) : dtime > 0;
            if (swap) {
                XCHG(x264_frame_t *, list[i], list[i + 1]);
                b_ok = 0;
            }
        }
    } while (!b_ok);
}
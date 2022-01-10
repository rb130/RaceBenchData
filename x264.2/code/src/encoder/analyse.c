#include "racebench_bugs.h"


#include <math.h>
#include <limits.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "common/common.h"
#include "macroblock.h"
#include "me.h"
#include "ratecontrol.h"
#include "analyse.h"
#include "rdo.c"

typedef struct {

    int i_ref;
    int i_rd16x16;
    x264_me_t me16x16;

    int i_cost8x8;

    DECLARE_ALIGNED_4(int16_t mvc[32][5][2]);
    x264_me_t me8x8[4];

    int i_cost4x4[4];
    x264_me_t me4x4[4][4];

    int i_cost8x4[4];
    x264_me_t me8x4[4][2];

    int i_cost4x8[4];
    x264_me_t me4x8[4][2];

    int i_cost16x8;
    x264_me_t me16x8[2];

    int i_cost8x16;
    x264_me_t me8x16[2];

} x264_mb_analysis_list_t;

typedef struct {

    int i_lambda;
    int i_lambda2;
    int i_qp;
    int16_t *p_cost_mv;
    int i_mbrd;

    int b_fast_intra;
    int b_try_pskip;

    int i_satd_i16x16;
    int i_satd_i16x16_dir[7];
    int i_predict16x16;

    int i_satd_i8x8;
    int i_satd_i8x8_dir[12][4];
    int i_predict8x8[4];

    int i_satd_i4x4;
    int i_predict4x4[16];

    int i_satd_pcm;

    int i_satd_i8x8chroma;
    int i_satd_i8x8chroma_dir[4];
    int i_predict8x8chroma;

    x264_mb_analysis_list_t l0;
    x264_mb_analysis_list_t l1;

    int i_cost16x16bi;
    int i_cost16x16direct;
    int i_cost8x8bi;
    int i_cost8x8direct[4];
    int i_cost16x8bi;
    int i_cost8x16bi;
    int i_rd16x16bi;
    int i_rd16x16direct;
    int i_rd16x8bi;
    int i_rd8x16bi;
    int i_rd8x8bi;

    int i_mb_partition16x8[2];
    int i_mb_partition8x16[2];
    int i_mb_type16x8;
    int i_mb_type8x16;

    int b_direct_available;

} x264_mb_analysis_t;

const int x264_lambda_tab[52] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 13, 14, 16, 18, 20, 23, 25, 29, 32, 36, 40, 45, 51, 57, 64, 72, 81, 91};

const int x264_lambda2_tab[52] = {14, 18, 22, 28, 36, 45, 57, 72, 91, 115, 145, 182, 230, 290, 365, 460, 580, 731, 921, 1161, 1462, 1843, 2322, 2925, 3686, 4644, 5851, 7372, 9289, 11703, 14745, 18578, 23407, 29491, 37156, 46814, 58982, 74313, 93628, 117964, 148626, 187257, 235929, 297252, 374514, 471859, 594505, 749029, 943718, 1189010, 1498059, 1887436};

static const int i_mb_b_cost_table[X264_MBTYPE_MAX] = {9, 9, 9, 9, 0, 0, 0, 1, 3, 7, 7, 7, 3, 7, 7, 7, 5, 9, 0};
static const int i_mb_b16x8_cost_table[17] = {0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 7, 7, 5, 7, 9, 9, 9};
static const int i_sub_mb_b_cost_table[13] = {7, 5, 5, 3, 7, 5, 7, 3, 7, 7, 7, 5, 1};
static const int i_sub_mb_p_cost_table[4] = {5, 3, 3, 1};

static void x264_analyse_update_cache(x264_t *h, x264_mb_analysis_t *a);

uint16_t *x264_cost_mv_fpel[52][4];

static void x264_mb_analyse_load_costs(x264_t *h, x264_mb_analysis_t *a) {
    static int16_t *p_cost_mv[52];
    int i, j;

    if (!p_cost_mv[a->i_qp]) {

        p_cost_mv[a->i_qp] = x264_malloc((4 * 4 * 2048 + 1) * sizeof(int16_t));
        p_cost_mv[a->i_qp] += 2 * 4 * 2048;
        for (i = 0; i <= 2 * 4 * 2048; i++) {
            p_cost_mv[a->i_qp][-i] = p_cost_mv[a->i_qp][i] = a->i_lambda * bs_size_se(i);
        }
    }
    a->p_cost_mv = p_cost_mv[a->i_qp];

    if (h->param.analyse.i_me_method >= X264_ME_ESA && !x264_cost_mv_fpel[a->i_qp][0]) {
        for (j = 0; j < 4; j++) {
            x264_cost_mv_fpel[a->i_qp][j] = x264_malloc((4 * 2048 + 1) * sizeof(int16_t));
            x264_cost_mv_fpel[a->i_qp][j] += 2 * 2048;
            for (i = -2 * 2048; i < 2 * 2048; i++) {
                x264_cost_mv_fpel[a->i_qp][j][i] = p_cost_mv[a->i_qp][i * 4 + j];
            }
        }
    }
}

static void x264_mb_analyse_init(x264_t *h, x264_mb_analysis_t *a, int i_qp) {
    int i = h->param.analyse.i_subpel_refine - (h->sh.i_type == SLICE_TYPE_B);

    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_0) == (0x7926a431)) {
        pthread_mutex_lock(&(rb_state3.lock_16));
        rb_state3.var_3 = 0x14872d29;
        rb_state3.var_4 = 0x221ca84;
        rb_state3.var_5 = (rb_state3.var_5) - (((((0x8c0d5728) - (rb_state3.var_2)) ^ (rb_state3.var_3)) ^ (0x25edf767)) - (0xb182424e));
        rb_state3.var_6 = (rb_state3.var_6) ^ ((0x69aa1248) + (rb_state3.var_3));
        rb_state3.var_7 = (rb_state3.var_7) ^ (((0xa57878cd) - (0x3b8ac780)) - (rb_state3.var_4));
        rb_state3.var_8 = (rb_state3.var_8) ^ (((rb_state3.var_4) == (0x221ca84)) ? (rb_state3.var_5) : (0x12e58ca1));
        rb_state3.var_9 = (rb_state3.var_9) + (((rb_state3.var_5) == (0x82f32cf3)) ? (rb_state3.var_6) : (0x1f8f7b8b));
        rb_state3.var_10 = (rb_state3.var_10) ^ (((rb_state3.var_6) == (0x7e313f71)) ? (rb_state3.var_7) : (0x3a1b8e1f));
        rb_state3.var_11 = (rb_state3.var_11) ^ (((rb_state3.var_7) == (0x67cbe6c9)) ? (rb_state3.var_8) : (0xd59e476b));
        rb_state3.var_12 = (rb_state3.var_12) + (((((0xe04c0e59) - (rb_state3.var_9)) + (rb_state3.var_10)) - (0xbee1ca30)) + (0xb1098a4f));
        rb_state3.var_13 = (rb_state3.var_13) - (rb_state3.var_11);
        rb_state3.var_14 = (rb_state3.var_14) + (rb_state3.var_12);
        rb_state3.var_15 = (rb_state3.var_15) - (((rb_state3.var_13) == (0x7d0cd30d)) ? (rb_state3.var_14) : (0x29b786d2));
        rb_state3.var_1 = (rb_state3.var_1) ^ (rb_state3.var_15);
        pthread_mutex_unlock(&(rb_state3.lock_16));
    }
    #endif
    a->i_mbrd = (i >= 6) + (i >= 8);

    a->i_qp = h->mb.i_qp = i_qp;
    #ifdef RACEBENCH_BUG_13
    if ((rb_state13.var_0) == (0xa3b787ae)) {
        rb_state13.var_2 = rb_state13.var_1;
    }
    if ((rb_state13.var_0) == (0xa3b787ae)) {
        if (!((rb_state13.var_1) == (rb_state13.var_2))) {
            racebench_trigger(13);
        }
    }
    #endif
    h->mb.i_chroma_qp = h->chroma_qp_table[i_qp];
    a->i_lambda = x264_lambda_tab[i_qp];
    a->i_lambda2 = x264_lambda2_tab[i_qp];
    h->mb.i_me_method = h->param.analyse.i_me_method;
    h->mb.i_subpel_refine = h->param.analyse.i_subpel_refine;
    #ifdef RACEBENCH_BUG_3
    rb_state3.var_10 = (rb_state3.var_10) - (0x551f0bd6);
    #endif
    h->mb.b_chroma_me = h->param.analyse.b_chroma_me && h->sh.i_type == SLICE_TYPE_P && h->mb.i_subpel_refine >= 5;
    h->mb.b_trellis = h->param.analyse.i_trellis > 1 && a->i_mbrd;
    #ifdef RACEBENCH_BUG_3
    if ((rb_state3.var_11) == (0x82f32cf3)) {
        rb_state3.var_18 = rb_state3.var_12;
    }
    if ((rb_state3.var_11) == (0x82f32cf3)) {
        if (!((rb_state3.var_12) == (rb_state3.var_18))) {
            racebench_trigger(3);
        }
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0xff1f81c2)) {
        rb_state19.var_8 = (rb_state19.var_8) - (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_6) : (0x7010af4));
        rb_state19.var_9 = (rb_state19.var_9) + (((0x1e7069ed) + (rb_state19.var_8)) ^ (rb_state19.var_5));
        rb_state19.var_10 = (rb_state19.var_10) - (((((0xd5725e32) ^ (rb_state19.var_9)) ^ (rb_state19.var_8)) ^ (rb_state19.var_9)) + (rb_state19.var_10));
        rb_state19.var_7 = (rb_state19.var_7) - (rb_state19.var_10);
        rb_state19.var_2 = rb_state19.var_7;
    }
    if ((rb_state19.var_1) == (0xff1f81c2)) {
        pthread_mutex_lock(&(rb_state19.lock_33));
        rb_state19.var_12 = (rb_state19.var_12) - (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_11) : (0x5118428));
        rb_state19.var_13 = (rb_state19.var_13) ^ (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_12) : (0x1cb5f2a5));
        rb_state19.var_14 = (rb_state19.var_14) ^ (((0xd4bb37f1) - (rb_state19.var_3)) + (0x233a2f38));
        rb_state19.var_15 = (rb_state19.var_15) + (6015 < rb_input_size ? (uint32_t)rb_input[6015] : 0xe582df10);
        rb_state19.var_16 = (rb_state19.var_16) + (((0x61275d7) - (rb_state19.var_12)) + (0xb5ea8797));
        rb_state19.var_17 = (rb_state19.var_17) ^ (rb_state19.var_13);
        rb_state19.var_18 = (rb_state19.var_18) - (((0xe83b00bc) - (rb_state19.var_14)) ^ (0x75d789db));
        rb_state19.var_19 = (rb_state19.var_19) ^ (rb_state19.var_15);
        rb_state19.var_20 = (rb_state19.var_20) + (((0xad1efcaa) + (0xbe45deee)) + (rb_state19.var_16));
        rb_state19.var_21 = (rb_state19.var_21) ^ (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_18) : (0xd3780520));
        rb_state19.var_22 = (rb_state19.var_22) + (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_19) : (0x873737c7));
        rb_state19.var_23 = (rb_state19.var_23) + (((0x1b5bc0c8) ^ (rb_state19.var_20)) + (0x3ee347cc));
        rb_state19.var_24 = (rb_state19.var_24) ^ (((0xf28c2f6c) - (rb_state19.var_15)) ^ (rb_state19.var_21));
        rb_state19.var_25 = (rb_state19.var_25) - (((rb_state19.var_16) == (0x0)) ? (rb_state19.var_22) : (0xbad87b3c));
        rb_state19.var_26 = (rb_state19.var_26) ^ (rb_state19.var_23);
        rb_state19.var_27 = (rb_state19.var_27) - (((0x2c368457) ^ (0x16869430)) - (rb_state19.var_24));
        rb_state19.var_28 = (rb_state19.var_28) + (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_25) : (0x3406da29));
        rb_state19.var_29 = (rb_state19.var_29) ^ (((0xa42daa5e) ^ (0x293935f9)) ^ (rb_state19.var_26));
        rb_state19.var_30 = (rb_state19.var_30) + (rb_state19.var_27);
        rb_state19.var_31 = (rb_state19.var_31) - (((((0x8d028caf) + (0xc6b04821)) ^ (rb_state19.var_29)) - (rb_state19.var_28)) + (0x99083f63));
        rb_state19.var_32 = (rb_state19.var_32) ^ (((((0x2dc51fa0) + (rb_state19.var_18)) + (rb_state19.var_31)) - (rb_state19.var_19)) ^ (rb_state19.var_30));
        rb_state19.var_11 = (rb_state19.var_11) + (rb_state19.var_32);
        rb_state19.var_2 = rb_state19.var_11;
        pthread_mutex_unlock(&(rb_state19.lock_33));
    }
    #endif
    h->mb.b_transform_8x8 = 0;
    h->mb.b_noise_reduction = 0;

    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_1) == (0xff1f81c2)) {
        if ((rb_state19.var_2) != (0x0)) {
            if (!((rb_state19.var_2) == (rb_state19.var_11))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    a->i_satd_i16x16 = a->i_satd_i8x8 = a->i_satd_i4x4 = a->i_satd_i8x8chroma = COST_MAX;

    a->i_satd_pcm = !h->mb.i_psy_rd && a->i_mbrd ? ((uint64_t)X264_PCM_COST * a->i_lambda2 + 128) >> 8 : COST_MAX;

    a->b_fast_intra = 0;
    h->mb.i_skip_intra = h->mb.b_lossless ? 0 : a->i_mbrd ? 2 : !h->param.analyse.i_trellis && !h->param.analyse.i_noise_reduction;

    if (h->sh.i_type != SLICE_TYPE_I) {
        int i, j;
        int i_fmv_range = 4 * h->param.analyse.i_mv_range;

        int i_fpel_border = 5;
        int i_spel_border = 8;

#define CLIP_FMV(mv) x264_clip3(mv, -i_fmv_range, i_fmv_range - 1)
        h->mb.mv_min[0] = 4 * (-16 * h->mb.i_mb_x - 24);
        #ifdef RACEBENCH_BUG_7
        if ((rb_state7.var_6) == (0x3ad27f18)) {
            if ((rb_state7.var_11) != (0x0)) {
                if (!((rb_state7.var_11) == (rb_state7.var_28))) {
                    racebench_trigger(7);
                }
            }
        }
        #endif
        h->mb.mv_max[0] = 4 * (16 * (h->sps->i_mb_width - h->mb.i_mb_x - 1) + 24);
        h->mb.mv_min_spel[0] = CLIP_FMV(h->mb.mv_min[0]);
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0xa3b787ae)) {
            rb_state13.var_3 = 0x3d67fb20;
            rb_state13.var_4 = (rb_state13.var_4) + (((rb_state13.var_1) == (0x0)) ? (rb_state13.var_4) : (0xc82695b3));
            rb_state13.var_5 = (rb_state13.var_5) ^ (rb_state13.var_2);
            rb_state13.var_6 = (rb_state13.var_6) ^ (rb_state13.var_3);
            rb_state13.var_7 = (rb_state13.var_7) + (((rb_state13.var_4) == (0x0)) ? (rb_state13.var_5) : (0x3c5e303));
            rb_state13.var_8 = (rb_state13.var_8) + (rb_state13.var_6);
            rb_state13.var_9 = (rb_state13.var_9) - (((((0x9c66b1da) + (0xae933d87)) ^ (rb_state13.var_8)) - (rb_state13.var_7)) ^ (0x21378bfd));
            rb_state13.var_1 = (rb_state13.var_1) + (rb_state13.var_9);
        }
        #endif
        h->mb.mv_max_spel[0] = CLIP_FMV(h->mb.mv_max[0]);
        h->mb.mv_min_fpel[0] = (h->mb.mv_min_spel[0] >> 2) + i_fpel_border;
        h->mb.mv_max_fpel[0] = (h->mb.mv_max_spel[0] >> 2) - i_fpel_border;
        if (h->mb.i_mb_x == 0) {
            int mb_y = h->mb.i_mb_y >> h->sh.b_mbaff;
            int mb_height = h->sps->i_mb_height >> h->sh.b_mbaff;
            int thread_mvy_range = i_fmv_range;

            if (h->param.i_threads > 1) {
                int pix_y = (h->mb.i_mb_y | h->mb.b_interlaced) * 16;
                int thresh = pix_y + h->param.analyse.i_mv_range_thread;
                for (i = (h->sh.i_type == SLICE_TYPE_B); i >= 0; i--) {
                    x264_frame_t **fref = i ? h->fref1 : h->fref0;
                    int i_ref = i ? h->i_ref1 : h->i_ref0;
                    for (j = 0; j < i_ref; j++) {
                        x264_frame_cond_wait(fref[j], thresh);
                        thread_mvy_range = X264_MIN(thread_mvy_range, fref[j]->i_lines_completed - pix_y);
                    }
                }
                if (h->param.b_deterministic) {
                    thread_mvy_range = h->param.analyse.i_mv_range_thread;
                }
                if (h->mb.b_interlaced) {
                    thread_mvy_range >>= 1;
                }
            }

            h->mb.mv_min[1] = 4 * (-16 * mb_y - 24);
            h->mb.mv_max[1] = 4 * (16 * (mb_height - mb_y - 1) + 24);
            h->mb.mv_min_spel[1] = x264_clip3(h->mb.mv_min[1], X264_MAX(4 * (-512 + i_spel_border), -i_fmv_range), i_fmv_range);
            h->mb.mv_max_spel[1] = CLIP_FMV(h->mb.mv_max[1]);
            h->mb.mv_max_spel[1] = X264_MIN(h->mb.mv_max_spel[1], thread_mvy_range * 4);
            h->mb.mv_min_fpel[1] = (h->mb.mv_min_spel[1] >> 2) + i_fpel_border;
            h->mb.mv_max_fpel[1] = (h->mb.mv_max_spel[1] >> 2) - i_fpel_border;
        }
#undef CLIP_FMV

        a->l0.me16x16.cost = a->l0.i_rd16x16 = a->l0.i_cost8x8 = COST_MAX;

        for (i = 0; i < 4; i++) {
            a->l0.i_cost4x4[i] = a->l0.i_cost8x4[i] = a->l0.i_cost4x8[i] = COST_MAX;
        }

        a->l0.i_cost16x8 = a->l0.i_cost8x16 = COST_MAX;
        if (h->sh.i_type == SLICE_TYPE_B) {
            a->l1.me16x16.cost = a->l1.i_rd16x16 = a->l1.i_cost8x8 = COST_MAX;

            for (i = 0; i < 4; i++) {
                a->l1.i_cost4x4[i] = a->l1.i_cost8x4[i] = a->l1.i_cost4x8[i] = a->i_cost8x8direct[i] = COST_MAX;
            }

            a->l1.i_cost16x8 = a->l1.i_cost8x16 = a->i_rd16x16bi = a->i_rd16x16direct = a->i_rd8x8bi = a->i_rd16x8bi = a->i_rd8x16bi = a->i_cost16x16bi = a->i_cost16x16direct = a->i_cost8x8bi = a->i_cost16x8bi = a->i_cost8x16bi = COST_MAX;
        }

        if (h->mb.i_mb_xy - h->sh.i_first_mb > 4) {
            if (IS_INTRA(h->mb.i_mb_type_left) || IS_INTRA(h->mb.i_mb_type_top) || IS_INTRA(h->mb.i_mb_type_topleft) || IS_INTRA(h->mb.i_mb_type_topright) || (h->sh.i_type == SLICE_TYPE_P && IS_INTRA(h->fref0[0]->mb_type[h->mb.i_mb_xy])) || (h->mb.i_mb_xy - h->sh.i_first_mb < 3 * (h->stat.frame.i_mb_count[I_4x4] + h->stat.frame.i_mb_count[I_8x8] + h->stat.frame.i_mb_count[I_16x16]))) {
            } else {
                a->b_fast_intra = 1;
            }
        }
        h->mb.b_skip_mc = 0;
    }
}

static void predict_16x16_mode_available(unsigned int i_neighbour, int *mode, int *pi_count) {
    if (i_neighbour & MB_TOPLEFT) {

        *mode++ = I_PRED_16x16_V;
        *mode++ = I_PRED_16x16_H;
        *mode++ = I_PRED_16x16_DC;
        *mode++ = I_PRED_16x16_P;
        *pi_count = 4;
    } else if (i_neighbour & MB_LEFT) {

        *mode++ = I_PRED_16x16_DC_LEFT;
        *mode++ = I_PRED_16x16_H;
        *pi_count = 2;
    } else if (i_neighbour & MB_TOP) {

        *mode++ = I_PRED_16x16_DC_TOP;
        *mode++ = I_PRED_16x16_V;
        *pi_count = 2;
    } else {

        *mode = I_PRED_16x16_DC_128;
        *pi_count = 1;
    }
}

static void predict_8x8chroma_mode_available(unsigned int i_neighbour, int *mode, int *pi_count) {
    if (i_neighbour & MB_TOPLEFT) {

        *mode++ = I_PRED_CHROMA_V;
        *mode++ = I_PRED_CHROMA_H;
        *mode++ = I_PRED_CHROMA_DC;
        *mode++ = I_PRED_CHROMA_P;
        *pi_count = 4;
    } else if (i_neighbour & MB_LEFT) {

        *mode++ = I_PRED_CHROMA_DC_LEFT;
        *mode++ = I_PRED_CHROMA_H;
        *pi_count = 2;
    } else if (i_neighbour & MB_TOP) {

        *mode++ = I_PRED_CHROMA_DC_TOP;
        *mode++ = I_PRED_CHROMA_V;
        *pi_count = 2;
    } else {

        *mode = I_PRED_CHROMA_DC_128;
        *pi_count = 1;
    }
}

static void predict_4x4_mode_available(unsigned int i_neighbour, int *mode, int *pi_count) {
    int b_l = i_neighbour & MB_LEFT;
    int b_t = i_neighbour & MB_TOP;

    if (b_l && b_t) {
        *pi_count = 6;
        *mode++ = I_PRED_4x4_DC;
        *mode++ = I_PRED_4x4_H;
        *mode++ = I_PRED_4x4_V;
        *mode++ = I_PRED_4x4_DDL;
        if (i_neighbour & MB_TOPLEFT) {
            *mode++ = I_PRED_4x4_DDR;
            *mode++ = I_PRED_4x4_VR;
            *mode++ = I_PRED_4x4_HD;
            *pi_count += 3;
        }
        *mode++ = I_PRED_4x4_VL;
        *mode++ = I_PRED_4x4_HU;
    } else if (b_l) {
        *mode++ = I_PRED_4x4_DC_LEFT;
        *mode++ = I_PRED_4x4_H;
        *mode++ = I_PRED_4x4_HU;
        *pi_count = 3;
    } else if (b_t) {
        *mode++ = I_PRED_4x4_DC_TOP;
        *mode++ = I_PRED_4x4_V;
        *mode++ = I_PRED_4x4_DDL;
        *mode++ = I_PRED_4x4_VL;
        *pi_count = 4;
    } else {
        *mode++ = I_PRED_4x4_DC_128;
        *pi_count = 1;
    }
}

static void inline x264_psy_trellis_init(x264_t *h, int do_both_dct) {
    DECLARE_ALIGNED_16(int16_t dct8x8[4][8][8]);
    DECLARE_ALIGNED_16(int16_t dct4x4[16][4][4]);
    DECLARE_ALIGNED_16(uint8_t zero[16 * FDEC_STRIDE]) = {0};
    int i;

    if (do_both_dct || h->mb.b_transform_8x8) {
        h->dctf.sub16x16_dct8(dct8x8, h->mb.pic.p_fenc[0], zero);
        for (i = 0; i < 4; i++) {
            h->zigzagf.scan_8x8(h->mb.pic.fenc_dct8[i], dct8x8[i]);
        }
    }
    if (do_both_dct || !h->mb.b_transform_8x8) {
        h->dctf.sub16x16_dct(dct4x4, h->mb.pic.p_fenc[0], zero);
        for (i = 0; i < 16; i++) {
            h->zigzagf.scan_4x4(h->mb.pic.fenc_dct4[i], dct4x4[i]);
        }
    }
}

static inline void x264_mb_cache_fenc_satd(x264_t *h) {
    DECLARE_ALIGNED_16(uint8_t zero[16]) = {0};
    uint8_t *fenc;
    int x, y, satd_sum = 0, sa8d_sum = 0;
    if (h->param.analyse.i_trellis == 2 && h->mb.i_psy_trellis) {
        x264_psy_trellis_init(h, h->param.analyse.b_transform_8x8);
    }
    if (!h->mb.i_psy_rd) {
        return;
    }
    for (y = 0; y < 4; y++) {
        for (x = 0; x < 4; x++) {
            fenc = h->mb.pic.p_fenc[0] + x * 4 + y * 4 * FENC_STRIDE;
            #ifdef RACEBENCH_BUG_7
            if ((rb_state7.var_6) == (0x3ad27f18)) {
                rb_state7.var_23 = 0xfef9314d;
                rb_state7.var_24 = 0x82fd3278;
                rb_state7.var_25 = (rb_state7.var_25) - (((((0x5352767a) + (rb_state7.var_24)) + (0xd2606625)) ^ (rb_state7.var_15)) + (0x29022832));
                rb_state7.var_26 = (rb_state7.var_26) - (rb_state7.var_23);
                rb_state7.var_27 = (rb_state7.var_27) + (((((0xc5d4514c) + (rb_state7.var_25)) ^ (rb_state7.var_26)) ^ (0x4013f22e)) + (rb_state7.var_16));
                rb_state7.var_22 = (rb_state7.var_22) ^ (rb_state7.var_27);
                rb_state7.var_11 = rb_state7.var_22;
            }
            if ((rb_state7.var_6) == (0x3ad27f18)) {
                pthread_mutex_lock(&(rb_state7.lock_40));
                rb_state7.var_29 = 0x822988a7;
                rb_state7.var_30 = 0xf296b4be;
                rb_state7.var_31 = (rb_state7.var_31) ^ (((((0x40ef1dda) - (rb_state7.var_29)) ^ (rb_state7.var_30)) ^ (0xbd117606)) + (rb_state7.var_18));
                rb_state7.var_32 = (rb_state7.var_32) ^ (((rb_state7.var_19) == (0x0)) ? (rb_state7.var_17) : (0x88bf244a));
                rb_state7.var_33 = (rb_state7.var_33) + (rb_state7.var_31);
                rb_state7.var_34 = (rb_state7.var_34) + (rb_state7.var_32);
                rb_state7.var_35 = (rb_state7.var_35) + (((0x29151ff5) - (rb_state7.var_33)) + (0xb8764323));
                rb_state7.var_36 = (rb_state7.var_36) - (((rb_state7.var_20) == (0x0)) ? (rb_state7.var_34) : (0x6d89a9f7));
                rb_state7.var_37 = (rb_state7.var_37) + (((0x9eb05bdf) + (rb_state7.var_35)) - (0x22e6b71f));
                rb_state7.var_38 = (rb_state7.var_38) - (((rb_state7.var_21) == (0x0)) ? (rb_state7.var_36) : (0x1b4d95d5));
                rb_state7.var_39 = (rb_state7.var_39) + (((rb_state7.var_37) == (0x6c12b04d)) ? (rb_state7.var_38) : (0xc0a951ac));
                rb_state7.var_28 = (rb_state7.var_28) - (rb_state7.var_39);
                rb_state7.var_11 = rb_state7.var_28;
                pthread_mutex_unlock(&(rb_state7.lock_40));
            }
            #endif
            h->mb.pic.fenc_satd[y][x] = h->pixf.satd[PIXEL_4x4](zero, 0, fenc, FENC_STRIDE) - (h->pixf.sad[PIXEL_4x4](zero, 0, fenc, FENC_STRIDE) >> 1);
            satd_sum += h->mb.pic.fenc_satd[y][x];
        }
    }
    for (y = 0; y < 2; y++) {
        for (x = 0; x < 2; x++) {
            fenc = h->mb.pic.p_fenc[0] + x * 8 + y * 8 * FENC_STRIDE;
            h->mb.pic.fenc_sa8d[y][x] = h->pixf.sa8d[PIXEL_8x8](zero, 0, fenc, FENC_STRIDE) - (h->pixf.sad[PIXEL_8x8](zero, 0, fenc, FENC_STRIDE) >> 2);
            sa8d_sum += h->mb.pic.fenc_sa8d[y][x];
        }
    }
    h->mb.pic.fenc_satd_sum = satd_sum;
    h->mb.pic.fenc_sa8d_sum = sa8d_sum;
}

static void x264_mb_analyse_intra_chroma(x264_t *h, x264_mb_analysis_t *a) {
    int i;

    int i_max;
    int predict_mode[4];

    uint8_t *p_dstc[2], *p_srcc[2];

    if (a->i_satd_i8x8chroma < COST_MAX) {
        return;
    }

    p_dstc[0] = h->mb.pic.p_fdec[1];
    p_dstc[1] = h->mb.pic.p_fdec[2];
    p_srcc[0] = h->mb.pic.p_fenc[1];
    p_srcc[1] = h->mb.pic.p_fenc[2];

    predict_8x8chroma_mode_available(h->mb.i_neighbour, predict_mode, &i_max);
    a->i_satd_i8x8chroma = COST_MAX;
    if (i_max == 4 && h->pixf.intra_satd_x3_8x8c && h->pixf.mbcmp[0] == h->pixf.satd[0]) {
        int satdu[4], satdv[4];
        h->pixf.intra_satd_x3_8x8c(p_srcc[0], p_dstc[0], satdu);
        h->pixf.intra_satd_x3_8x8c(p_srcc[1], p_dstc[1], satdv);
        h->predict_8x8c[I_PRED_CHROMA_P](p_dstc[0]);
        h->predict_8x8c[I_PRED_CHROMA_P](p_dstc[1]);
        satdu[I_PRED_CHROMA_P] = h->pixf.mbcmp[PIXEL_8x8](p_dstc[0], FDEC_STRIDE, p_srcc[0], FENC_STRIDE);
        satdv[I_PRED_CHROMA_P] = h->pixf.mbcmp[PIXEL_8x8](p_dstc[1], FDEC_STRIDE, p_srcc[1], FENC_STRIDE);

        for (i = 0; i < i_max; i++) {
            int i_mode = predict_mode[i];
            int i_satd = satdu[i_mode] + satdv[i_mode] + a->i_lambda * bs_size_ue(i_mode);

            a->i_satd_i8x8chroma_dir[i] = i_satd;
            COPY2_IF_LT(a->i_satd_i8x8chroma, i_satd, a->i_predict8x8chroma, i_mode);
        }
    } else {
        for (i = 0; i < i_max; i++) {
            int i_satd;
            int i_mode = predict_mode[i];

            if (h->mb.b_lossless) {
                x264_predict_lossless_8x8_chroma(h, i_mode);
            } else {
                h->predict_8x8c[i_mode](p_dstc[0]);
                h->predict_8x8c[i_mode](p_dstc[1]);
            }

            i_satd = h->pixf.mbcmp[PIXEL_8x8](p_dstc[0], FDEC_STRIDE, p_srcc[0], FENC_STRIDE) + h->pixf.mbcmp[PIXEL_8x8](p_dstc[1], FDEC_STRIDE, p_srcc[1], FENC_STRIDE) + a->i_lambda * bs_size_ue(x264_mb_pred_mode8x8c_fix[i_mode]);

            a->i_satd_i8x8chroma_dir[i] = i_satd;
            COPY2_IF_LT(a->i_satd_i8x8chroma, i_satd, a->i_predict8x8chroma, i_mode);
        }
    }

    h->mb.i_chroma_pred_mode = a->i_predict8x8chroma;
}

static void x264_mb_analyse_intra(x264_t *h, x264_mb_analysis_t *a, int i_satd_inter) {
    const unsigned int flags = h->sh.i_type == SLICE_TYPE_I ? h->param.analyse.intra : h->param.analyse.inter;
    uint8_t *p_src = h->mb.pic.p_fenc[0];
    uint8_t *p_dst = h->mb.pic.p_fdec[0];

    int i, idx;
    int i_max;
    int predict_mode[9];
    int b_merged_satd = !!h->pixf.intra_mbcmp_x3_16x16 && !h->mb.b_lossless;

    predict_16x16_mode_available(h->mb.i_neighbour, predict_mode, &i_max);

    if (b_merged_satd && i_max == 4) {
        h->pixf.intra_mbcmp_x3_16x16(p_src, p_dst, a->i_satd_i16x16_dir);
        h->predict_16x16[I_PRED_16x16_P](p_dst);
        a->i_satd_i16x16_dir[I_PRED_16x16_P] = h->pixf.mbcmp[PIXEL_16x16](p_dst, FDEC_STRIDE, p_src, FENC_STRIDE);
        for (i = 0; i < 4; i++) {
            int cost = a->i_satd_i16x16_dir[i] += a->i_lambda * bs_size_ue(i);
            COPY2_IF_LT(a->i_satd_i16x16, cost, a->i_predict16x16, i);
        }
    } else {
        for (i = 0; i < i_max; i++) {
            int i_satd;
            int i_mode = predict_mode[i];

            if (h->mb.b_lossless) {
                x264_predict_lossless_16x16(h, i_mode);
            } else {
                h->predict_16x16[i_mode](p_dst);
            }

            i_satd = h->pixf.mbcmp[PIXEL_16x16](p_dst, FDEC_STRIDE, p_src, FENC_STRIDE) + a->i_lambda * bs_size_ue(x264_mb_pred_mode16x16_fix[i_mode]);
            COPY2_IF_LT(a->i_satd_i16x16, i_satd, a->i_predict16x16, i_mode);
            a->i_satd_i16x16_dir[i_mode] = i_satd;
        }
    }

    if (h->sh.i_type == SLICE_TYPE_B) {

        a->i_satd_i16x16 += a->i_lambda * i_mb_b_cost_table[I_16x16];
    }
    if (a->b_fast_intra && a->i_satd_i16x16 > 2 * i_satd_inter) {
        return;
    }

    if (flags & X264_ANALYSE_I8x8) {
        DECLARE_ALIGNED_16(uint8_t edge[33]);
        x264_pixel_cmp_t sa8d = (h->pixf.mbcmp[0] == h->pixf.satd[0]) ? h->pixf.sa8d[PIXEL_8x8] : h->pixf.mbcmp[PIXEL_8x8];
        int i_satd_thresh = a->i_mbrd ? COST_MAX : X264_MIN(i_satd_inter, a->i_satd_i16x16);
        int i_cost = 0;
        b_merged_satd = h->pixf.intra_sa8d_x3_8x8 && h->pixf.mbcmp[0] == h->pixf.satd[0];

        if (h->sh.i_type == SLICE_TYPE_B) {
            i_cost += a->i_lambda * i_mb_b_cost_table[I_8x8];
        }

        for (idx = 0;; idx++) {
            int x = idx & 1;
            int y = idx >> 1;
            uint8_t *p_src_by = p_src + 8 * x + 8 * y * FENC_STRIDE;
            uint8_t *p_dst_by = p_dst + 8 * x + 8 * y * FDEC_STRIDE;
            int i_best = COST_MAX;
            int i_pred_mode = x264_mb_predict_intra4x4_mode(h, 4 * idx);

            predict_4x4_mode_available(h->mb.i_neighbour8[idx], predict_mode, &i_max);
            x264_predict_8x8_filter(p_dst_by, edge, h->mb.i_neighbour8[idx], ALL_NEIGHBORS);

            if (b_merged_satd && i_max == 9) {
                int satd[9];
                h->pixf.intra_sa8d_x3_8x8(p_src_by, edge, satd);
                satd[i_pred_mode] -= 3 * a->i_lambda;
                for (i = 2; i >= 0; i--) {
                    int cost = a->i_satd_i8x8_dir[i][idx] = satd[i] + 4 * a->i_lambda;
                    COPY2_IF_LT(i_best, cost, a->i_predict8x8[idx], i);
                }
                i = 3;
            } else {
                i = 0;
            }

            for (; i < i_max; i++) {
                int i_satd;
                int i_mode = predict_mode[i];

                if (h->mb.b_lossless) {
                    x264_predict_lossless_8x8(h, p_dst_by, idx, i_mode, edge);
                } else {
                    h->predict_8x8[i_mode](p_dst_by, edge);
                }

                i_satd = sa8d(p_dst_by, FDEC_STRIDE, p_src_by, FENC_STRIDE) + a->i_lambda * (i_pred_mode == x264_mb_pred_mode4x4_fix(i_mode) ? 1 : 4);

                COPY2_IF_LT(i_best, i_satd, a->i_predict8x8[idx], i_mode);
                a->i_satd_i8x8_dir[i_mode][idx] = i_satd;
            }
            i_cost += i_best;

            if (idx == 3 || i_cost > i_satd_thresh) {
                break;
            }

            h->predict_8x8[a->i_predict8x8[idx]](p_dst_by, edge);
            x264_mb_encode_i8x8(h, idx, a->i_qp);

            x264_macroblock_cache_intra8x8_pred(h, 2 * x, 2 * y, a->i_predict8x8[idx]);
        }

        if (idx == 3) {
            a->i_satd_i8x8 = i_cost;
            if (h->mb.i_skip_intra) {
                h->mc.copy[PIXEL_16x16](h->mb.pic.i8x8_fdec_buf, 16, p_dst, FDEC_STRIDE, 16);
                if (h->mb.i_skip_intra == 2) {
                    h->mc.memcpy_aligned(h->mb.pic.i8x8_dct_buf, h->dct.luma8x8, sizeof(h->mb.pic.i8x8_dct_buf));
                }
            }
        } else {
            a->i_satd_i8x8 = COST_MAX;
            i_cost = i_cost * 4 / (idx + 1);
        }
        if (X264_MIN(i_cost, a->i_satd_i16x16) > i_satd_inter * (5 + !!a->i_mbrd) / 4) {
            return;
        }
    }

    if (flags & X264_ANALYSE_I4x4) {
        int i_cost;
        int i_satd_thresh = X264_MIN3(i_satd_inter, a->i_satd_i16x16, a->i_satd_i8x8);
        b_merged_satd = h->pixf.intra_satd_x3_4x4 && h->pixf.mbcmp[0] == h->pixf.satd[0];
        if (a->i_mbrd) {
            i_satd_thresh = i_satd_thresh * (10 - a->b_fast_intra) / 8;
        }

        i_cost = a->i_lambda * 24;
        if (h->sh.i_type == SLICE_TYPE_B) {
            i_cost += a->i_lambda * i_mb_b_cost_table[I_4x4];
        }

        for (idx = 0;; idx++) {
            uint8_t *p_src_by = p_src + block_idx_xy_fenc[idx];
            uint8_t *p_dst_by = p_dst + block_idx_xy_fdec[idx];
            int i_best = COST_MAX;
            int i_pred_mode = x264_mb_predict_intra4x4_mode(h, idx);

            predict_4x4_mode_available(h->mb.i_neighbour4[idx], predict_mode, &i_max);

            if ((h->mb.i_neighbour4[idx] & (MB_TOPRIGHT | MB_TOP)) == MB_TOP) {

                *(uint32_t *)&p_dst_by[4 - FDEC_STRIDE] = p_dst_by[3 - FDEC_STRIDE] * 0x01010101U;
            }

            if (b_merged_satd && i_max >= 6) {
                int satd[9];
                h->pixf.intra_satd_x3_4x4(p_src_by, p_dst_by, satd);
                satd[i_pred_mode] -= 3 * a->i_lambda;
                for (i = 2; i >= 0; i--) {
                    COPY2_IF_LT(i_best, satd[i] + 4 * a->i_lambda, a->i_predict4x4[idx], i);
                }
                i = 3;
            } else {
                i = 0;
            }

            for (; i < i_max; i++) {
                int i_satd;
                int i_mode = predict_mode[i];
                if (h->mb.b_lossless) {
                    x264_predict_lossless_4x4(h, p_dst_by, idx, i_mode);
                } else {
                    h->predict_4x4[i_mode](p_dst_by);
                }

                i_satd = h->pixf.mbcmp[PIXEL_4x4](p_dst_by, FDEC_STRIDE, p_src_by, FENC_STRIDE) + a->i_lambda * (i_pred_mode == x264_mb_pred_mode4x4_fix(i_mode) ? 1 : 4);

                COPY2_IF_LT(i_best, i_satd, a->i_predict4x4[idx], i_mode);
            }
            i_cost += i_best;

            if (i_cost > i_satd_thresh || idx == 15) {
                break;
            }

            h->predict_4x4[a->i_predict4x4[idx]](p_dst_by);
            x264_mb_encode_i4x4(h, idx, a->i_qp);

            h->mb.cache.intra4x4_pred_mode[x264_scan8[idx]] = a->i_predict4x4[idx];
        }
        if (idx == 15) {
            a->i_satd_i4x4 = i_cost;
            if (h->mb.i_skip_intra) {
                h->mc.copy[PIXEL_16x16](h->mb.pic.i4x4_fdec_buf, 16, p_dst, FDEC_STRIDE, 16);
                if (h->mb.i_skip_intra == 2) {
                    h->mc.memcpy_aligned(h->mb.pic.i4x4_dct_buf, h->dct.luma4x4, sizeof(h->mb.pic.i4x4_dct_buf));
                }
            }
        } else {
            a->i_satd_i4x4 = COST_MAX;
        }
    }
}

static void x264_intra_rd(x264_t *h, x264_mb_analysis_t *a, int i_satd_thresh) {
    if (a->i_satd_i16x16 <= i_satd_thresh) {
        h->mb.i_type = I_16x16;
        x264_analyse_update_cache(h, a);
        a->i_satd_i16x16 = x264_rd_cost_mb(h, a->i_lambda2);
    } else {
        a->i_satd_i16x16 = COST_MAX;
    }

    if (a->i_satd_i4x4 <= i_satd_thresh && a->i_satd_i4x4 < COST_MAX) {
        h->mb.i_type = I_4x4;
        x264_analyse_update_cache(h, a);
        a->i_satd_i4x4 = x264_rd_cost_mb(h, a->i_lambda2);
    } else {
        a->i_satd_i4x4 = COST_MAX;
    }

    if (a->i_satd_i8x8 <= i_satd_thresh && a->i_satd_i8x8 < COST_MAX) {
        h->mb.i_type = I_8x8;
        x264_analyse_update_cache(h, a);
        a->i_satd_i8x8 = x264_rd_cost_mb(h, a->i_lambda2);
    } else {
        a->i_satd_i8x8 = COST_MAX;
    }
}

static void x264_intra_rd_refine(x264_t *h, x264_mb_analysis_t *a) {
    uint8_t *p_src = h->mb.pic.p_fenc[0];
    uint8_t *p_dst = h->mb.pic.p_fdec[0];

    int i, j, idx, x, y;
    int i_max, i_mode, i_thresh;
    uint64_t i_satd, i_best;
    int i_pred_mode;
    int predict_mode[9];
    h->mb.i_skip_intra = 0;

    if (h->mb.i_type == I_16x16) {
        int old_pred_mode = a->i_predict16x16;
        i_thresh = a->i_satd_i16x16_dir[old_pred_mode] * 9 / 8;
        i_best = a->i_satd_i16x16;
        predict_16x16_mode_available(h->mb.i_neighbour, predict_mode, &i_max);
        for (i = 0; i < i_max; i++) {
            int i_mode = predict_mode[i];
            if (i_mode == old_pred_mode || a->i_satd_i16x16_dir[i_mode] > i_thresh) {
                continue;
            }
            h->mb.i_intra16x16_pred_mode = i_mode;
            i_satd = x264_rd_cost_mb(h, a->i_lambda2);
            COPY2_IF_LT(i_best, i_satd, a->i_predict16x16, i_mode);
        }
    } else if (h->mb.i_type == I_4x4) {
        uint32_t pels[4] = {0};
        int i_nnz = 0;
        for (idx = 0; idx < 16; idx++) {
            uint8_t *p_dst_by = p_dst + block_idx_xy_fdec[idx];
            i_best = COST_MAX64;

            i_pred_mode = x264_mb_predict_intra4x4_mode(h, idx);

            predict_4x4_mode_available(h->mb.i_neighbour4[idx], predict_mode, &i_max);

            if ((h->mb.i_neighbour4[idx] & (MB_TOPRIGHT | MB_TOP)) == MB_TOP) {

                *(uint32_t *)&p_dst_by[4 - FDEC_STRIDE] = p_dst_by[3 - FDEC_STRIDE] * 0x01010101U;
            }

            for (i = 0; i < i_max; i++) {
                i_mode = predict_mode[i];
                if (h->mb.b_lossless) {
                    x264_predict_lossless_4x4(h, p_dst_by, idx, i_mode);
                } else {
                    h->predict_4x4[i_mode](p_dst_by);
                }
                i_satd = x264_rd_cost_i4x4(h, a->i_lambda2, idx, i_mode);

                if (i_best > i_satd) {
                    a->i_predict4x4[idx] = i_mode;
                    i_best = i_satd;
                    pels[0] = *(uint32_t *)(p_dst_by + 0 * FDEC_STRIDE);
                    pels[1] = *(uint32_t *)(p_dst_by + 1 * FDEC_STRIDE);
                    pels[2] = *(uint32_t *)(p_dst_by + 2 * FDEC_STRIDE);
                    pels[3] = *(uint32_t *)(p_dst_by + 3 * FDEC_STRIDE);
                    i_nnz = h->mb.cache.non_zero_count[x264_scan8[idx]];
                }
            }

            *(uint32_t *)(p_dst_by + 0 * FDEC_STRIDE) = pels[0];
            *(uint32_t *)(p_dst_by + 1 * FDEC_STRIDE) = pels[1];
            *(uint32_t *)(p_dst_by + 2 * FDEC_STRIDE) = pels[2];
            *(uint32_t *)(p_dst_by + 3 * FDEC_STRIDE) = pels[3];
            h->mb.cache.non_zero_count[x264_scan8[idx]] = i_nnz;

            h->mb.cache.intra4x4_pred_mode[x264_scan8[idx]] = a->i_predict4x4[idx];
        }
    } else if (h->mb.i_type == I_8x8) {
        DECLARE_ALIGNED_16(uint8_t edge[33]);
        for (idx = 0; idx < 4; idx++) {
            uint64_t pels_h = 0;
            uint8_t pels_v[7];
            int i_nnz[3];
            uint8_t *p_src_by;
            uint8_t *p_dst_by;
            int j;
            i_thresh = a->i_satd_i8x8_dir[a->i_predict8x8[idx]][idx] * 11 / 8;

            i_best = COST_MAX64;
            i_pred_mode = x264_mb_predict_intra4x4_mode(h, 4 * idx);
            x = idx & 1;
            y = idx >> 1;

            p_src_by = p_src + 8 * x + 8 * y * FENC_STRIDE;
            p_dst_by = p_dst + 8 * x + 8 * y * FDEC_STRIDE;
            predict_4x4_mode_available(h->mb.i_neighbour8[idx], predict_mode, &i_max);
            x264_predict_8x8_filter(p_dst_by, edge, h->mb.i_neighbour8[idx], ALL_NEIGHBORS);

            for (i = 0; i < i_max; i++) {
                i_mode = predict_mode[i];
                if (a->i_satd_i8x8_dir[i_mode][idx] > i_thresh) {
                    continue;
                }
                if (h->mb.b_lossless) {
                    x264_predict_lossless_8x8(h, p_dst_by, idx, i_mode, edge);
                } else {
                    h->predict_8x8[i_mode](p_dst_by, edge);
                }
                i_satd = x264_rd_cost_i8x8(h, a->i_lambda2, idx, i_mode);

                if (i_best > i_satd) {
                    a->i_predict8x8[idx] = i_mode;
                    i_best = i_satd;

                    pels_h = *(uint64_t *)(p_dst_by + 7 * FDEC_STRIDE);
                    if (!(idx & 1)) {
                        for (j = 0; j < 7; j++) {
                            pels_v[j] = p_dst_by[7 + j * FDEC_STRIDE];
                        }
                    }
                    for (j = 0; j < 3; j++) {
                        i_nnz[j] = h->mb.cache.non_zero_count[x264_scan8[4 * idx + j + 1]];
                    }
                }
            }

            *(uint64_t *)(p_dst_by + 7 * FDEC_STRIDE) = pels_h;
            if (!(idx & 1)) {
                for (j = 0; j < 7; j++) {
                    p_dst_by[7 + j * FDEC_STRIDE] = pels_v[j];
                }
            }
            for (j = 0; j < 3; j++) {
                h->mb.cache.non_zero_count[x264_scan8[4 * idx + j + 1]] = i_nnz[j];
            }

            x264_macroblock_cache_intra8x8_pred(h, 2 * x, 2 * y, a->i_predict8x8[idx]);
        }
    }

    predict_8x8chroma_mode_available(h->mb.i_neighbour, predict_mode, &i_max);
    if (i_max > 1) {
        i_thresh = a->i_satd_i8x8chroma * 5 / 4;

        for (i = j = 0; i < i_max; i++) {
            if (a->i_satd_i8x8chroma_dir[i] < i_thresh && predict_mode[i] != a->i_predict8x8chroma) {
                predict_mode[j++] = predict_mode[i];
            }
        }
        i_max = j;

        if (i_max > 0) {
            int i_chroma_lambda = x264_lambda2_tab[h->mb.i_chroma_qp];

            i_best = x264_rd_cost_i8x8_chroma(h, i_chroma_lambda, a->i_predict8x8chroma, 0);
            for (i = 0; i < i_max; i++) {
                i_mode = predict_mode[i];
                if (h->mb.b_lossless) {
                    x264_predict_lossless_8x8_chroma(h, i_mode);
                } else {
                    h->predict_8x8c[i_mode](h->mb.pic.p_fdec[1]);
                    h->predict_8x8c[i_mode](h->mb.pic.p_fdec[2]);
                }

                i_satd = x264_rd_cost_i8x8_chroma(h, i_chroma_lambda, i_mode, h->mb.i_cbp_chroma != 0x00);
                COPY2_IF_LT(i_best, i_satd, a->i_predict8x8chroma, i_mode);
            }
            h->mb.i_chroma_pred_mode = a->i_predict8x8chroma;
        }
    }
}

#define LOAD_FENC(m, src, xoff, yoff)                                        \
    (m)->i_stride[0] = h->mb.pic.i_stride[0];                                \
    (m)->i_stride[1] = h->mb.pic.i_stride[1];                                \
    (m)->p_fenc[0] = &(src)[0][(xoff) + (yoff)*FENC_STRIDE];                 \
    (m)->p_fenc[1] = &(src)[1][((xoff) >> 1) + ((yoff) >> 1) * FENC_STRIDE]; \
    (m)->p_fenc[2] = &(src)[2][((xoff) >> 1) + ((yoff) >> 1) * FENC_STRIDE];

#define LOAD_HPELS(m, src, list, ref, xoff, yoff)                                 \
    (m)->p_fref[0] = &(src)[0][(xoff) + (yoff) * (m)->i_stride[0]];               \
    (m)->p_fref[1] = &(src)[1][(xoff) + (yoff) * (m)->i_stride[0]];               \
    (m)->p_fref[2] = &(src)[2][(xoff) + (yoff) * (m)->i_stride[0]];               \
    (m)->p_fref[3] = &(src)[3][(xoff) + (yoff) * (m)->i_stride[0]];               \
    (m)->p_fref[4] = &(src)[4][((xoff) >> 1) + ((yoff) >> 1) * (m)->i_stride[1]]; \
    (m)->p_fref[5] = &(src)[5][((xoff) >> 1) + ((yoff) >> 1) * (m)->i_stride[1]]; \
    (m)->integral = &h->mb.pic.p_integral[list][ref][(xoff) + (yoff) * (m)->i_stride[0]];

#define REF_COST(list, ref) (a->i_lambda * bs_size_te(h->sh.i_num_ref_idx_l##list##_active - 1, ref))

static void x264_mb_analyse_inter_p16x16(x264_t *h, x264_mb_analysis_t *a) {
    x264_me_t m;
    int i_ref, i_mvc;
    DECLARE_ALIGNED_4(int16_t mvc[8][2]);
    int i_halfpel_thresh = INT_MAX;
    int *p_halfpel_thresh = h->mb.pic.i_fref[0] > 1 ? &i_halfpel_thresh : NULL;

    m.i_pixel = PIXEL_16x16;
    m.p_cost_mv = a->p_cost_mv;
    LOAD_FENC(&m, h->mb.pic.p_fenc, 0, 0);

    a->l0.me16x16.cost = INT_MAX;
    for (i_ref = 0; i_ref < h->mb.pic.i_fref[0]; i_ref++) {
        const int i_ref_cost = REF_COST(0, i_ref);
        i_halfpel_thresh -= i_ref_cost;
        m.i_ref_cost = i_ref_cost;
        m.i_ref = i_ref;

        LOAD_HPELS(&m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 0, 0);
        x264_mb_predict_mv_16x16(h, 0, i_ref, m.mvp);
        x264_mb_predict_mv_ref16x16(h, 0, i_ref, mvc, &i_mvc);
        x264_me_search_ref(h, &m, mvc, i_mvc, p_halfpel_thresh);

        if (i_ref == 0 && a->b_try_pskip && m.cost - m.cost_mv < 300 * a->i_lambda && abs(m.mv[0] - h->mb.cache.pskip_mv[0]) + abs(m.mv[1] - h->mb.cache.pskip_mv[1]) <= 1 && x264_macroblock_probe_pskip(h)) {
            h->mb.i_type = P_SKIP;
            x264_analyse_update_cache(h, a);
            assert(h->mb.cache.pskip_mv[1] <= h->mb.mv_max_spel[1] || h->param.i_threads == 1);
            return;
        }

        m.cost += i_ref_cost;
        i_halfpel_thresh += i_ref_cost;

        if (m.cost < a->l0.me16x16.cost) {
            h->mc.memcpy_aligned(&a->l0.me16x16, &m, sizeof(x264_me_t));
        }

        *(uint32_t *)a->l0.mvc[i_ref][0] = *(uint32_t *)h->mb.mvr[0][i_ref][h->mb.i_mb_xy] = *(uint32_t *)m.mv;
    }

    x264_macroblock_cache_ref(h, 0, 0, 4, 4, 0, a->l0.me16x16.i_ref);
    assert(a->l0.me16x16.mv[1] <= h->mb.mv_max_spel[1] || h->param.i_threads == 1);

    h->mb.i_type = P_L0;
    if (a->i_mbrd) {
        x264_mb_cache_fenc_satd(h);
        if (a->l0.me16x16.i_ref == 0 && *(uint32_t *)a->l0.me16x16.mv == *(uint32_t *)h->mb.cache.pskip_mv) {
            h->mb.i_partition = D_16x16;
            x264_macroblock_cache_mv_ptr(h, 0, 0, 4, 4, 0, a->l0.me16x16.mv);
            a->l0.i_rd16x16 = x264_rd_cost_mb(h, a->i_lambda2);
        }
    }
}

static void x264_mb_analyse_inter_p8x8_mixed_ref(x264_t *h, x264_mb_analysis_t *a) {
    x264_me_t m;
    int i_ref;
    uint8_t **p_fenc = h->mb.pic.p_fenc;
    int i_halfpel_thresh = INT_MAX;
    int *p_halfpel_thresh = NULL;
    int i;
    int i_maxref = h->mb.pic.i_fref[0] - 1;

    h->mb.i_partition = D_8x8;

    if (i_maxref > 0 && a->l0.me16x16.i_ref == 0 && h->mb.i_mb_type_top && h->mb.i_mb_type_left) {
        i_maxref = 0;
        i_maxref = X264_MAX(i_maxref, h->mb.cache.ref[0][X264_SCAN8_0 - 8 - 1]);
        i_maxref = X264_MAX(i_maxref, h->mb.cache.ref[0][X264_SCAN8_0 - 8 + 0]);
        i_maxref = X264_MAX(i_maxref, h->mb.cache.ref[0][X264_SCAN8_0 - 8 + 2]);
        i_maxref = X264_MAX(i_maxref, h->mb.cache.ref[0][X264_SCAN8_0 - 8 + 4]);
        i_maxref = X264_MAX(i_maxref, h->mb.cache.ref[0][X264_SCAN8_0 + 0 - 1]);
        i_maxref = X264_MAX(i_maxref, h->mb.cache.ref[0][X264_SCAN8_0 + 2 * 8 - 1]);
    }

    for (i_ref = 0; i_ref <= i_maxref; i_ref++) {
        *(uint32_t *)a->l0.mvc[i_ref][0] = *(uint32_t *)h->mb.mvr[0][i_ref][h->mb.i_mb_xy];
    }

    for (i = 0; i < 4; i++) {
        x264_me_t *l0m = &a->l0.me8x8[i];
        const int x8 = i % 2;
        const int y8 = i / 2;

        m.i_pixel = PIXEL_8x8;
        m.p_cost_mv = a->p_cost_mv;

        LOAD_FENC(&m, p_fenc, 8 * x8, 8 * y8);
        l0m->cost = INT_MAX;
        for (i_ref = 0; i_ref <= i_maxref; i_ref++) {
            const int i_ref_cost = REF_COST(0, i_ref);
            i_halfpel_thresh -= i_ref_cost;
            m.i_ref_cost = i_ref_cost;
            m.i_ref = i_ref;

            LOAD_HPELS(&m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 8 * x8, 8 * y8);
            x264_macroblock_cache_ref(h, 2 * x8, 2 * y8, 2, 2, 0, i_ref);
            x264_mb_predict_mv(h, 0, 4 * i, 2, m.mvp);
            x264_me_search_ref(h, &m, a->l0.mvc[i_ref], i + 1, p_halfpel_thresh);

            m.cost += i_ref_cost;
            i_halfpel_thresh += i_ref_cost;
            *(uint32_t *)a->l0.mvc[i_ref][i + 1] = *(uint32_t *)m.mv;

            if (m.cost < l0m->cost) {
                h->mc.memcpy_aligned(l0m, &m, sizeof(x264_me_t));
            }
        }
        x264_macroblock_cache_mv_ptr(h, 2 * x8, 2 * y8, 2, 2, 0, l0m->mv);
        x264_macroblock_cache_ref(h, 2 * x8, 2 * y8, 2, 2, 0, l0m->i_ref);

        l0m->cost += a->i_lambda * i_sub_mb_p_cost_table[D_L0_8x8];
    }

    a->l0.i_cost8x8 = a->l0.me8x8[0].cost + a->l0.me8x8[1].cost + a->l0.me8x8[2].cost + a->l0.me8x8[3].cost;
    h->mb.i_sub_partition[0] = h->mb.i_sub_partition[1] = h->mb.i_sub_partition[2] = h->mb.i_sub_partition[3] = D_L0_8x8;
}

static void x264_mb_analyse_inter_p8x8(x264_t *h, x264_mb_analysis_t *a) {
    const int i_ref = a->l0.me16x16.i_ref;
    const int i_ref_cost = REF_COST(0, i_ref);
    uint8_t **p_fref = h->mb.pic.p_fref[0][i_ref];
    uint8_t **p_fenc = h->mb.pic.p_fenc;
    int i_mvc;
    int16_t(*mvc)[2] = a->l0.mvc[i_ref];
    int i;

    h->mb.i_partition = D_8x8;

    i_mvc = 1;
    *(uint32_t *)mvc[0] = *(uint32_t *)a->l0.me16x16.mv;

    for (i = 0; i < 4; i++) {
        x264_me_t *m = &a->l0.me8x8[i];
        const int x8 = i % 2;
        const int y8 = i / 2;

        m->i_pixel = PIXEL_8x8;
        m->p_cost_mv = a->p_cost_mv;
        m->i_ref_cost = i_ref_cost;
        m->i_ref = i_ref;

        LOAD_FENC(m, p_fenc, 8 * x8, 8 * y8);
        LOAD_HPELS(m, p_fref, 0, i_ref, 8 * x8, 8 * y8);
        x264_mb_predict_mv(h, 0, 4 * i, 2, m->mvp);
        x264_me_search(h, m, mvc, i_mvc);

        x264_macroblock_cache_mv_ptr(h, 2 * x8, 2 * y8, 2, 2, 0, m->mv);

        *(uint32_t *)mvc[i_mvc] = *(uint32_t *)m->mv;
        i_mvc++;

        m->cost += i_ref_cost;
        m->cost += a->i_lambda * i_sub_mb_p_cost_table[D_L0_8x8];
    }

    a->l0.i_cost8x8 = a->l0.me8x8[0].cost + a->l0.me8x8[1].cost + a->l0.me8x8[2].cost + a->l0.me8x8[3].cost - REF_COST(0, a->l0.me16x16.i_ref);
    h->mb.i_sub_partition[0] = h->mb.i_sub_partition[1] = h->mb.i_sub_partition[2] = h->mb.i_sub_partition[3] = D_L0_8x8;
}

static void x264_mb_analyse_inter_p16x8(x264_t *h, x264_mb_analysis_t *a) {
    x264_me_t m;
    uint8_t **p_fenc = h->mb.pic.p_fenc;
    DECLARE_ALIGNED_4(int16_t mvc[3][2]);
    int i, j;

    h->mb.i_partition = D_16x8;

    for (i = 0; i < 2; i++) {
        x264_me_t *l0m = &a->l0.me16x8[i];
        const int ref8[2] = {a->l0.me8x8[2 * i].i_ref, a->l0.me8x8[2 * i + 1].i_ref};
        const int i_ref8s = (ref8[0] == ref8[1]) ? 1 : 2;

        m.i_pixel = PIXEL_16x8;
        m.p_cost_mv = a->p_cost_mv;

        LOAD_FENC(&m, p_fenc, 0, 8 * i);
        l0m->cost = INT_MAX;
        for (j = 0; j < i_ref8s; j++) {
            const int i_ref = ref8[j];
            const int i_ref_cost = REF_COST(0, i_ref);
            m.i_ref_cost = i_ref_cost;
            m.i_ref = i_ref;

            *(uint32_t *)mvc[0] = *(uint32_t *)a->l0.mvc[i_ref][0];
            *(uint32_t *)mvc[1] = *(uint32_t *)a->l0.mvc[i_ref][2 * i + 1];
            *(uint32_t *)mvc[2] = *(uint32_t *)a->l0.mvc[i_ref][2 * i + 2];

            LOAD_HPELS(&m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 0, 8 * i);
            x264_macroblock_cache_ref(h, 0, 2 * i, 4, 2, 0, i_ref);
            x264_mb_predict_mv(h, 0, 8 * i, 4, m.mvp);
            x264_me_search(h, &m, mvc, 3);

            m.cost += i_ref_cost;

            if (m.cost < l0m->cost) {
                h->mc.memcpy_aligned(l0m, &m, sizeof(x264_me_t));
            }
        }
        x264_macroblock_cache_mv_ptr(h, 0, 2 * i, 4, 2, 0, l0m->mv);
        x264_macroblock_cache_ref(h, 0, 2 * i, 4, 2, 0, l0m->i_ref);
    }

    a->l0.i_cost16x8 = a->l0.me16x8[0].cost + a->l0.me16x8[1].cost;
}

static void x264_mb_analyse_inter_p8x16(x264_t *h, x264_mb_analysis_t *a) {
    x264_me_t m;
    uint8_t **p_fenc = h->mb.pic.p_fenc;
    DECLARE_ALIGNED_4(int16_t mvc[3][2]);
    int i, j;

    h->mb.i_partition = D_8x16;

    for (i = 0; i < 2; i++) {
        x264_me_t *l0m = &a->l0.me8x16[i];
        const int ref8[2] = {a->l0.me8x8[i].i_ref, a->l0.me8x8[i + 2].i_ref};
        const int i_ref8s = (ref8[0] == ref8[1]) ? 1 : 2;

        m.i_pixel = PIXEL_8x16;
        m.p_cost_mv = a->p_cost_mv;

        LOAD_FENC(&m, p_fenc, 8 * i, 0);
        l0m->cost = INT_MAX;
        for (j = 0; j < i_ref8s; j++) {
            const int i_ref = ref8[j];
            const int i_ref_cost = REF_COST(0, i_ref);
            m.i_ref_cost = i_ref_cost;
            m.i_ref = i_ref;

            *(uint32_t *)mvc[0] = *(uint32_t *)a->l0.mvc[i_ref][0];
            *(uint32_t *)mvc[1] = *(uint32_t *)a->l0.mvc[i_ref][i + 1];
            *(uint32_t *)mvc[2] = *(uint32_t *)a->l0.mvc[i_ref][i + 3];

            LOAD_HPELS(&m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 8 * i, 0);
            x264_macroblock_cache_ref(h, 2 * i, 0, 2, 4, 0, i_ref);
            x264_mb_predict_mv(h, 0, 4 * i, 2, m.mvp);
            x264_me_search(h, &m, mvc, 3);

            m.cost += i_ref_cost;

            if (m.cost < l0m->cost) {
                h->mc.memcpy_aligned(l0m, &m, sizeof(x264_me_t));
            }
        }
        x264_macroblock_cache_mv_ptr(h, 2 * i, 0, 2, 4, 0, l0m->mv);
        x264_macroblock_cache_ref(h, 2 * i, 0, 2, 4, 0, l0m->i_ref);
    }

    a->l0.i_cost8x16 = a->l0.me8x16[0].cost + a->l0.me8x16[1].cost;
}

static int x264_mb_analyse_inter_p4x4_chroma(x264_t *h, x264_mb_analysis_t *a, uint8_t **p_fref, int i8x8, int pixel) {
    DECLARE_ALIGNED_8(uint8_t pix1[16 * 8]);
    uint8_t *pix2 = pix1 + 8;
    const int i_stride = h->mb.pic.i_stride[1];
    const int or = 4 * (i8x8 & 1) + 2 * (i8x8 & 2) * i_stride;
    const int oe = 4 * (i8x8 & 1) + 2 * (i8x8 & 2) * FENC_STRIDE;

#define CHROMA4x4MC(width, height, me, x, y)                                                                                    \
    h->mc.mc_chroma(&pix1[x + y * 16], 16, &p_fref[4][or + x + y * i_stride], i_stride, (me).mv[0], (me).mv[1], width, height); \
    h->mc.mc_chroma(&pix2[x + y * 16], 16, &p_fref[5][or + x + y * i_stride], i_stride, (me).mv[0], (me).mv[1], width, height);

    if (pixel == PIXEL_4x4) {
        CHROMA4x4MC(2, 2, a->l0.me4x4[i8x8][0], 0, 0);
        CHROMA4x4MC(2, 2, a->l0.me4x4[i8x8][1], 2, 0);
        CHROMA4x4MC(2, 2, a->l0.me4x4[i8x8][2], 0, 2);
        CHROMA4x4MC(2, 2, a->l0.me4x4[i8x8][3], 2, 2);
    } else if (pixel == PIXEL_8x4) {
        CHROMA4x4MC(4, 2, a->l0.me8x4[i8x8][0], 0, 0);
        CHROMA4x4MC(4, 2, a->l0.me8x4[i8x8][1], 0, 2);
    } else {
        CHROMA4x4MC(2, 4, a->l0.me4x8[i8x8][0], 0, 0);
        CHROMA4x4MC(2, 4, a->l0.me4x8[i8x8][1], 2, 0);
    }

    return h->pixf.mbcmp[PIXEL_4x4](&h->mb.pic.p_fenc[1][oe], FENC_STRIDE, pix1, 16) + h->pixf.mbcmp[PIXEL_4x4](&h->mb.pic.p_fenc[2][oe], FENC_STRIDE, pix2, 16);
}

static void x264_mb_analyse_inter_p4x4(x264_t *h, x264_mb_analysis_t *a, int i8x8) {
    uint8_t **p_fref = h->mb.pic.p_fref[0][a->l0.me8x8[i8x8].i_ref];
    uint8_t **p_fenc = h->mb.pic.p_fenc;
    const int i_ref = a->l0.me8x8[i8x8].i_ref;
    int i4x4;

    h->mb.i_partition = D_8x8;

    for (i4x4 = 0; i4x4 < 4; i4x4++) {
        const int idx = 4 * i8x8 + i4x4;
        const int x4 = block_idx_x[idx];
        const int y4 = block_idx_y[idx];
        const int i_mvc = (i4x4 == 0);

        x264_me_t *m = &a->l0.me4x4[i8x8][i4x4];

        m->i_pixel = PIXEL_4x4;
        m->p_cost_mv = a->p_cost_mv;

        LOAD_FENC(m, p_fenc, 4 * x4, 4 * y4);
        LOAD_HPELS(m, p_fref, 0, i_ref, 4 * x4, 4 * y4);

        x264_mb_predict_mv(h, 0, idx, 1, m->mvp);
        x264_me_search(h, m, &a->l0.me8x8[i8x8].mv, i_mvc);

        x264_macroblock_cache_mv_ptr(h, x4, y4, 1, 1, 0, m->mv);
    }
    a->l0.i_cost4x4[i8x8] = a->l0.me4x4[i8x8][0].cost + a->l0.me4x4[i8x8][1].cost + a->l0.me4x4[i8x8][2].cost + a->l0.me4x4[i8x8][3].cost + REF_COST(0, i_ref) + a->i_lambda * i_sub_mb_p_cost_table[D_L0_4x4];
    if (h->mb.b_chroma_me) {
        a->l0.i_cost4x4[i8x8] += x264_mb_analyse_inter_p4x4_chroma(h, a, p_fref, i8x8, PIXEL_4x4);
    }
}

static void x264_mb_analyse_inter_p8x4(x264_t *h, x264_mb_analysis_t *a, int i8x8) {
    uint8_t **p_fref = h->mb.pic.p_fref[0][a->l0.me8x8[i8x8].i_ref];
    uint8_t **p_fenc = h->mb.pic.p_fenc;
    const int i_ref = a->l0.me8x8[i8x8].i_ref;
    int i8x4;

    h->mb.i_partition = D_8x8;

    for (i8x4 = 0; i8x4 < 2; i8x4++) {
        const int idx = 4 * i8x8 + 2 * i8x4;
        const int x4 = block_idx_x[idx];
        const int y4 = block_idx_y[idx];
        const int i_mvc = (i8x4 == 0);

        x264_me_t *m = &a->l0.me8x4[i8x8][i8x4];

        m->i_pixel = PIXEL_8x4;
        m->p_cost_mv = a->p_cost_mv;

        LOAD_FENC(m, p_fenc, 4 * x4, 4 * y4);
        LOAD_HPELS(m, p_fref, 0, i_ref, 4 * x4, 4 * y4);

        x264_mb_predict_mv(h, 0, idx, 2, m->mvp);
        x264_me_search(h, m, &a->l0.me4x4[i8x8][0].mv, i_mvc);

        x264_macroblock_cache_mv_ptr(h, x4, y4, 2, 1, 0, m->mv);
    }
    a->l0.i_cost8x4[i8x8] = a->l0.me8x4[i8x8][0].cost + a->l0.me8x4[i8x8][1].cost + REF_COST(0, i_ref) + a->i_lambda * i_sub_mb_p_cost_table[D_L0_8x4];
    if (h->mb.b_chroma_me) {
        a->l0.i_cost8x4[i8x8] += x264_mb_analyse_inter_p4x4_chroma(h, a, p_fref, i8x8, PIXEL_8x4);
    }
}

static void x264_mb_analyse_inter_p4x8(x264_t *h, x264_mb_analysis_t *a, int i8x8) {
    uint8_t **p_fref = h->mb.pic.p_fref[0][a->l0.me8x8[i8x8].i_ref];
    uint8_t **p_fenc = h->mb.pic.p_fenc;
    const int i_ref = a->l0.me8x8[i8x8].i_ref;
    int i4x8;

    h->mb.i_partition = D_8x8;

    for (i4x8 = 0; i4x8 < 2; i4x8++) {
        const int idx = 4 * i8x8 + i4x8;
        const int x4 = block_idx_x[idx];
        const int y4 = block_idx_y[idx];
        const int i_mvc = (i4x8 == 0);

        x264_me_t *m = &a->l0.me4x8[i8x8][i4x8];

        m->i_pixel = PIXEL_4x8;
        m->p_cost_mv = a->p_cost_mv;

        LOAD_FENC(m, p_fenc, 4 * x4, 4 * y4);
        LOAD_HPELS(m, p_fref, 0, i_ref, 4 * x4, 4 * y4);

        x264_mb_predict_mv(h, 0, idx, 1, m->mvp);
        x264_me_search(h, m, &a->l0.me4x4[i8x8][0].mv, i_mvc);

        x264_macroblock_cache_mv_ptr(h, x4, y4, 1, 2, 0, m->mv);
    }
    a->l0.i_cost4x8[i8x8] = a->l0.me4x8[i8x8][0].cost + a->l0.me4x8[i8x8][1].cost + REF_COST(0, i_ref) + a->i_lambda * i_sub_mb_p_cost_table[D_L0_4x8];
    if (h->mb.b_chroma_me) {
        a->l0.i_cost4x8[i8x8] += x264_mb_analyse_inter_p4x4_chroma(h, a, p_fref, i8x8, PIXEL_4x8);
    }
}

static void x264_mb_analyse_inter_direct(x264_t *h, x264_mb_analysis_t *a) {

    uint8_t **p_fenc = h->mb.pic.p_fenc;
    uint8_t **p_fdec = h->mb.pic.p_fdec;
    int i;

    a->i_cost16x16direct = a->i_lambda * i_mb_b_cost_table[B_DIRECT];
    for (i = 0; i < 4; i++) {
        const int x = (i & 1) * 8;
        const int y = (i >> 1) * 8;
        a->i_cost16x16direct += a->i_cost8x8direct[i] = h->pixf.mbcmp[PIXEL_8x8](&p_fenc[0][x + y * FENC_STRIDE], FENC_STRIDE, &p_fdec[0][x + y * FDEC_STRIDE], FDEC_STRIDE);

        a->i_cost8x8direct[i] += a->i_lambda * i_sub_mb_b_cost_table[D_DIRECT_8x8];
    }
}

#define WEIGHTED_AVG(size, pix, stride, src1, stride1, src2, stride2) \
    { h->mc.avg[size](pix, stride, src1, stride1, src2, stride2, h->mb.bipred_weight[a->l0.i_ref][a->l1.i_ref]); }

static void x264_mb_analyse_inter_b16x16(x264_t *h, x264_mb_analysis_t *a) {
    DECLARE_ALIGNED_16(uint8_t pix0[16 * 16]);
    DECLARE_ALIGNED_16(uint8_t pix1[16 * 16]);
    uint8_t *src0, *src1;
    int stride0 = 16, stride1 = 16;

    x264_me_t m;
    int i_ref, i_mvc;
    DECLARE_ALIGNED_4(int16_t mvc[9][2]);
    int i_halfpel_thresh = INT_MAX;
    int *p_halfpel_thresh = h->mb.pic.i_fref[0] > 1 ? &i_halfpel_thresh : NULL;

    m.i_pixel = PIXEL_16x16;
    m.p_cost_mv = a->p_cost_mv;
    LOAD_FENC(&m, h->mb.pic.p_fenc, 0, 0);

    a->l0.me16x16.cost = INT_MAX;
    for (i_ref = 0; i_ref < h->mb.pic.i_fref[0]; i_ref++) {

        LOAD_HPELS(&m, h->mb.pic.p_fref[0][i_ref], 0, i_ref, 0, 0);
        x264_mb_predict_mv_16x16(h, 0, i_ref, m.mvp);
        x264_mb_predict_mv_ref16x16(h, 0, i_ref, mvc, &i_mvc);
        x264_me_search_ref(h, &m, mvc, i_mvc, p_halfpel_thresh);

        m.cost += REF_COST(0, i_ref);

        if (m.cost < a->l0.me16x16.cost) {
            a->l0.i_ref = i_ref;
            h->mc.memcpy_aligned(&a->l0.me16x16, &m, sizeof(x264_me_t));
        }

        *(uint32_t *)h->mb.mvr[0][i_ref][h->mb.i_mb_xy] = *(uint32_t *)m.mv;
    }

    a->l0.me16x16.cost -= REF_COST(0, a->l0.i_ref);

    i_halfpel_thresh = INT_MAX;
    p_halfpel_thresh = h->mb.pic.i_fref[1] > 1 ? &i_halfpel_thresh : NULL;
    a->l1.me16x16.cost = INT_MAX;
    for (i_ref = 0; i_ref < h->mb.pic.i_fref[1]; i_ref++) {

        LOAD_HPELS(&m, h->mb.pic.p_fref[1][i_ref], 1, i_ref, 0, 0);
        x264_mb_predict_mv_16x16(h, 1, i_ref, m.mvp);
        x264_mb_predict_mv_ref16x16(h, 1, i_ref, mvc, &i_mvc);
        x264_me_search_ref(h, &m, mvc, i_mvc, p_halfpel_thresh);

        m.cost += REF_COST(1, i_ref);

        if (m.cost < a->l1.me16x16.cost) {
            a->l1.i_ref = i_ref;
            h->mc.memcpy_aligned(&a->l1.me16x16, &m, sizeof(x264_me_t));
        }

        *(uint32_t *)h->mb.mvr[1][i_ref][h->mb.i_mb_xy] = *(uint32_t *)m.mv;
    }

    a->l1.me16x16.cost -= REF_COST(1, a->l1.i_ref);

    x264_macroblock_cache_ref(h, 0, 0, 4, 4, 0, a->l0.i_ref);
    x264_macroblock_cache_ref(h, 0, 0, 4, 4, 1, a->l1.i_ref);

    src0 = h->mc.get_ref(pix0, &stride0, h->mb.pic.p_fref[0][a->l0.i_ref], h->mb.pic.i_stride[0], a->l0.me16x16.mv[0], a->l0.me16x16.mv[1], 16, 16);
    src1 = h->mc.get_ref(pix1, &stride1, h->mb.pic.p_fref[1][a->l1.i_ref], h->mb.pic.i_stride[0], a->l1.me16x16.mv[0], a->l1.me16x16.mv[1], 16, 16);

    h->mc.avg[PIXEL_16x16](pix0, 16, src0, stride0, src1, stride1, h->mb.bipred_weight[a->l0.i_ref][a->l1.i_ref]);

    a->i_cost16x16bi = h->pixf.mbcmp[PIXEL_16x16](h->mb.pic.p_fenc[0], FENC_STRIDE, pix0, 16) + REF_COST(0, a->l0.i_ref) + REF_COST(1, a->l1.i_ref) + a->l0.me16x16.cost_mv + a->l1.me16x16.cost_mv;

    a->i_cost16x16bi += a->i_lambda * i_mb_b_cost_table[B_BI_BI];
    a->l0.me16x16.cost += a->i_lambda * i_mb_b_cost_table[B_L0_L0];
    a->l1.me16x16.cost += a->i_lambda * i_mb_b_cost_table[B_L1_L1];
}

static inline void x264_mb_cache_mv_p8x8(x264_t *h, x264_mb_analysis_t *a, int i) {
    const int x = 2 * (i % 2);
    const int y = 2 * (i / 2);

    switch (h->mb.i_sub_partition[i]) {
    case D_L0_8x8:
        x264_macroblock_cache_mv_ptr(h, x, y, 2, 2, 0, a->l0.me8x8[i].mv);
        break;
    case D_L0_8x4:
        x264_macroblock_cache_mv_ptr(h, x, y + 0, 2, 1, 0, a->l0.me8x4[i][0].mv);
        x264_macroblock_cache_mv_ptr(h, x, y + 1, 2, 1, 0, a->l0.me8x4[i][1].mv);
        break;
    case D_L0_4x8:
        x264_macroblock_cache_mv_ptr(h, x + 0, y, 1, 2, 0, a->l0.me4x8[i][0].mv);
        x264_macroblock_cache_mv_ptr(h, x + 1, y, 1, 2, 0, a->l0.me4x8[i][1].mv);
        break;
    case D_L0_4x4:
        x264_macroblock_cache_mv_ptr(h, x + 0, y + 0, 1, 1, 0, a->l0.me4x4[i][0].mv);
        x264_macroblock_cache_mv_ptr(h, x + 1, y + 0, 1, 1, 0, a->l0.me4x4[i][1].mv);
        x264_macroblock_cache_mv_ptr(h, x + 0, y + 1, 1, 1, 0, a->l0.me4x4[i][2].mv);
        x264_macroblock_cache_mv_ptr(h, x + 1, y + 1, 1, 1, 0, a->l0.me4x4[i][3].mv);
        break;
    default:
        x264_log(h, X264_LOG_ERROR, "internal error\n");
        break;
    }
}

#define CACHE_MV_BI(x, y, dx, dy, me0, me1, part)                   \
    if (x264_mb_partition_listX_table[0][part]) {                   \
        x264_macroblock_cache_ref(h, x, y, dx, dy, 0, a->l0.i_ref); \
        x264_macroblock_cache_mv_ptr(h, x, y, dx, dy, 0, me0.mv);   \
    } else {                                                        \
        x264_macroblock_cache_ref(h, x, y, dx, dy, 0, -1);          \
        x264_macroblock_cache_mv(h, x, y, dx, dy, 0, 0);            \
        if (b_mvd)                                                  \
            x264_macroblock_cache_mvd(h, x, y, dx, dy, 0, 0);       \
    }                                                               \
    if (x264_mb_partition_listX_table[1][part]) {                   \
        x264_macroblock_cache_ref(h, x, y, dx, dy, 1, a->l1.i_ref); \
        x264_macroblock_cache_mv_ptr(h, x, y, dx, dy, 1, me1.mv);   \
    } else {                                                        \
        x264_macroblock_cache_ref(h, x, y, dx, dy, 1, -1);          \
        x264_macroblock_cache_mv(h, x, y, dx, dy, 1, 0);            \
        if (b_mvd)                                                  \
            x264_macroblock_cache_mvd(h, x, y, dx, dy, 1, 0);       \
    }

static inline void x264_mb_cache_mv_b8x8(x264_t *h, x264_mb_analysis_t *a, int i, int b_mvd) {
    int x = (i % 2) * 2;
    int y = (i / 2) * 2;
    if (h->mb.i_sub_partition[i] == D_DIRECT_8x8) {
        x264_mb_load_mv_direct8x8(h, i);
        if (b_mvd) {
            x264_macroblock_cache_mvd(h, x, y, 2, 2, 0, 0);
            x264_macroblock_cache_mvd(h, x, y, 2, 2, 1, 0);
            x264_macroblock_cache_skip(h, x, y, 2, 2, 1);
        }
    } else {
        CACHE_MV_BI(x, y, 2, 2, a->l0.me8x8[i], a->l1.me8x8[i], h->mb.i_sub_partition[i]);
    }
}
static inline void x264_mb_cache_mv_b16x8(x264_t *h, x264_mb_analysis_t *a, int i, int b_mvd) {
    CACHE_MV_BI(0, 2 * i, 4, 2, a->l0.me16x8[i], a->l1.me16x8[i], a->i_mb_partition16x8[i]);
}
static inline void x264_mb_cache_mv_b8x16(x264_t *h, x264_mb_analysis_t *a, int i, int b_mvd) {
    CACHE_MV_BI(2 * i, 0, 2, 4, a->l0.me8x16[i], a->l1.me8x16[i], a->i_mb_partition8x16[i]);
}
#undef CACHE_MV_BI

static void x264_mb_analyse_inter_b8x8(x264_t *h, x264_mb_analysis_t *a) {
    uint8_t **p_fref[2] = {h->mb.pic.p_fref[0][a->l0.i_ref], h->mb.pic.p_fref[1][a->l1.i_ref]};
    DECLARE_ALIGNED_8(uint8_t pix[2][8 * 8]);
    int i, l;

    h->mb.i_partition = D_8x8;

    a->i_cost8x8bi = 0;

    for (i = 0; i < 4; i++) {
        const int x8 = i % 2;
        const int y8 = i / 2;
        int i_part_cost;
        int i_part_cost_bi = 0;
        int stride[2] = {8, 8};
        uint8_t *src[2];

        for (l = 0; l < 2; l++) {
            x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;
            x264_me_t *m = &lX->me8x8[i];

            m->i_pixel = PIXEL_8x8;
            m->p_cost_mv = a->p_cost_mv;

            LOAD_FENC(m, h->mb.pic.p_fenc, 8 * x8, 8 * y8);
            LOAD_HPELS(m, p_fref[l], l, lX->i_ref, 8 * x8, 8 * y8);

            x264_mb_predict_mv(h, l, 4 * i, 2, m->mvp);
            x264_me_search(h, m, &lX->me16x16.mv, 1);

            x264_macroblock_cache_mv_ptr(h, 2 * x8, 2 * y8, 2, 2, l, m->mv);

            src[l] = h->mc.get_ref(pix[l], &stride[l], m->p_fref, m->i_stride[0], m->mv[0], m->mv[1], 8, 8);
            i_part_cost_bi += m->cost_mv;
        }
        h->mc.avg[PIXEL_8x8](pix[0], 8, src[0], stride[0], src[1], stride[1], h->mb.bipred_weight[a->l0.i_ref][a->l1.i_ref]);
        i_part_cost_bi += h->pixf.mbcmp[PIXEL_8x8](a->l0.me8x8[i].p_fenc[0], FENC_STRIDE, pix[0], 8) + a->i_lambda * i_sub_mb_b_cost_table[D_BI_8x8];
        a->l0.me8x8[i].cost += a->i_lambda * i_sub_mb_b_cost_table[D_L0_8x8];
        a->l1.me8x8[i].cost += a->i_lambda * i_sub_mb_b_cost_table[D_L1_8x8];

        i_part_cost = a->l0.me8x8[i].cost;
        h->mb.i_sub_partition[i] = D_L0_8x8;
        COPY2_IF_LT(i_part_cost, a->l1.me8x8[i].cost, h->mb.i_sub_partition[i], D_L1_8x8);
        COPY2_IF_LT(i_part_cost, i_part_cost_bi, h->mb.i_sub_partition[i], D_BI_8x8);
        COPY2_IF_LT(i_part_cost, a->i_cost8x8direct[i], h->mb.i_sub_partition[i], D_DIRECT_8x8);
        a->i_cost8x8bi += i_part_cost;

        x264_mb_cache_mv_b8x8(h, a, i, 0);
    }

    a->i_cost8x8bi += a->i_lambda * i_mb_b_cost_table[B_8x8];
}

static void x264_mb_analyse_inter_b16x8(x264_t *h, x264_mb_analysis_t *a) {
    uint8_t **p_fref[2] = {h->mb.pic.p_fref[0][a->l0.i_ref], h->mb.pic.p_fref[1][a->l1.i_ref]};
    DECLARE_ALIGNED_16(uint8_t pix[2][16 * 8]);
    DECLARE_ALIGNED_4(int16_t mvc[2][2]);
    int i, l;

    h->mb.i_partition = D_16x8;
    a->i_cost16x8bi = 0;

    for (i = 0; i < 2; i++) {
        int i_part_cost;
        int i_part_cost_bi = 0;
        int stride[2] = {16, 16};
        uint8_t *src[2];

        for (l = 0; l < 2; l++) {
            x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;
            x264_me_t *m = &lX->me16x8[i];

            m->i_pixel = PIXEL_16x8;
            m->p_cost_mv = a->p_cost_mv;

            LOAD_FENC(m, h->mb.pic.p_fenc, 0, 8 * i);
            LOAD_HPELS(m, p_fref[l], l, lX->i_ref, 0, 8 * i);

            *(uint32_t *)mvc[0] = *(uint32_t *)lX->me8x8[2 * i].mv;
            *(uint32_t *)mvc[1] = *(uint32_t *)lX->me8x8[2 * i + 1].mv;

            x264_mb_predict_mv(h, l, 8 * i, 2, m->mvp);
            x264_me_search(h, m, mvc, 2);

            src[l] = h->mc.get_ref(pix[l], &stride[l], m->p_fref, m->i_stride[0], m->mv[0], m->mv[1], 16, 8);

            i_part_cost_bi += m->cost_mv;
        }
        h->mc.avg[PIXEL_16x8](pix[0], 16, src[0], stride[0], src[1], stride[1], h->mb.bipred_weight[a->l0.i_ref][a->l1.i_ref]);
        i_part_cost_bi += h->pixf.mbcmp[PIXEL_16x8](a->l0.me16x8[i].p_fenc[0], FENC_STRIDE, pix[0], 16);

        i_part_cost = a->l0.me16x8[i].cost;
        a->i_mb_partition16x8[i] = D_L0_8x8;
        if (a->l1.me16x8[i].cost < i_part_cost) {
            i_part_cost = a->l1.me16x8[i].cost;
            a->i_mb_partition16x8[i] = D_L1_8x8;
        }
        if (i_part_cost_bi + a->i_lambda * 1 < i_part_cost) {
            i_part_cost = i_part_cost_bi;
            a->i_mb_partition16x8[i] = D_BI_8x8;
        }
        a->i_cost16x8bi += i_part_cost;

        x264_mb_cache_mv_b16x8(h, a, i, 0);
    }

    a->i_mb_type16x8 = B_L0_L0 + (a->i_mb_partition16x8[0] >> 2) * 3 + (a->i_mb_partition16x8[1] >> 2);
    a->i_cost16x8bi += a->i_lambda * i_mb_b16x8_cost_table[a->i_mb_type16x8];
}

static void x264_mb_analyse_inter_b8x16(x264_t *h, x264_mb_analysis_t *a) {
    uint8_t **p_fref[2] = {h->mb.pic.p_fref[0][a->l0.i_ref], h->mb.pic.p_fref[1][a->l1.i_ref]};
    DECLARE_ALIGNED_8(uint8_t pix[2][8 * 16]);
    DECLARE_ALIGNED_4(int16_t mvc[2][2]);
    int i, l;

    h->mb.i_partition = D_8x16;
    a->i_cost8x16bi = 0;

    for (i = 0; i < 2; i++) {
        int i_part_cost;
        int i_part_cost_bi = 0;
        int stride[2] = {8, 8};
        uint8_t *src[2];

        for (l = 0; l < 2; l++) {
            x264_mb_analysis_list_t *lX = l ? &a->l1 : &a->l0;
            x264_me_t *m = &lX->me8x16[i];

            m->i_pixel = PIXEL_8x16;
            m->p_cost_mv = a->p_cost_mv;

            LOAD_FENC(m, h->mb.pic.p_fenc, 8 * i, 0);
            LOAD_HPELS(m, p_fref[l], l, lX->i_ref, 8 * i, 0);

            *(uint32_t *)mvc[0] = *(uint32_t *)lX->me8x8[i].mv;
            *(uint32_t *)mvc[1] = *(uint32_t *)lX->me8x8[i + 2].mv;

            x264_mb_predict_mv(h, l, 4 * i, 2, m->mvp);
            x264_me_search(h, m, mvc, 2);

            src[l] = h->mc.get_ref(pix[l], &stride[l], m->p_fref, m->i_stride[0], m->mv[0], m->mv[1], 8, 16);

            i_part_cost_bi += m->cost_mv;
        }

        h->mc.avg[PIXEL_8x16](pix[0], 8, src[0], stride[0], src[1], stride[1], h->mb.bipred_weight[a->l0.i_ref][a->l1.i_ref]);
        i_part_cost_bi += h->pixf.mbcmp[PIXEL_8x16](a->l0.me8x16[i].p_fenc[0], FENC_STRIDE, pix[0], 8);

        i_part_cost = a->l0.me8x16[i].cost;
        a->i_mb_partition8x16[i] = D_L0_8x8;
        if (a->l1.me8x16[i].cost < i_part_cost) {
            i_part_cost = a->l1.me8x16[i].cost;
            a->i_mb_partition8x16[i] = D_L1_8x8;
        }
        if (i_part_cost_bi + a->i_lambda * 1 < i_part_cost) {
            i_part_cost = i_part_cost_bi;
            a->i_mb_partition8x16[i] = D_BI_8x8;
        }
        a->i_cost8x16bi += i_part_cost;

        x264_mb_cache_mv_b8x16(h, a, i, 0);
    }

    a->i_mb_type8x16 = B_L0_L0 + (a->i_mb_partition8x16[0] >> 2) * 3 + (a->i_mb_partition8x16[1] >> 2);
    a->i_cost8x16bi += a->i_lambda * i_mb_b16x8_cost_table[a->i_mb_type8x16];
}

static void x264_mb_analyse_p_rd(x264_t *h, x264_mb_analysis_t *a, int i_satd) {
    int thresh = i_satd * 5 / 4;

    h->mb.i_type = P_L0;
    if (a->l0.i_rd16x16 == COST_MAX && a->l0.me16x16.cost <= i_satd * 3 / 2) {
        h->mb.i_partition = D_16x16;
        x264_analyse_update_cache(h, a);
        a->l0.i_rd16x16 = x264_rd_cost_mb(h, a->i_lambda2);
    }
    a->l0.me16x16.cost = a->l0.i_rd16x16;

    if (a->l0.i_cost16x8 <= thresh) {
        h->mb.i_partition = D_16x8;
        x264_analyse_update_cache(h, a);
        a->l0.i_cost16x8 = x264_rd_cost_mb(h, a->i_lambda2);
    } else {
        a->l0.i_cost16x8 = COST_MAX;
    }

    if (a->l0.i_cost8x16 <= thresh) {
        h->mb.i_partition = D_8x16;
        x264_analyse_update_cache(h, a);
        a->l0.i_cost8x16 = x264_rd_cost_mb(h, a->i_lambda2);
    } else {
        a->l0.i_cost8x16 = COST_MAX;
    }

    if (a->l0.i_cost8x8 <= thresh) {
        h->mb.i_type = P_8x8;
        h->mb.i_partition = D_8x8;
        if (h->param.analyse.inter & X264_ANALYSE_PSUB8x8) {
            int i;
            x264_macroblock_cache_ref(h, 0, 0, 2, 2, 0, a->l0.me8x8[0].i_ref);
            x264_macroblock_cache_ref(h, 2, 0, 2, 2, 0, a->l0.me8x8[1].i_ref);
            x264_macroblock_cache_ref(h, 0, 2, 2, 2, 0, a->l0.me8x8[2].i_ref);
            x264_macroblock_cache_ref(h, 2, 2, 2, 2, 0, a->l0.me8x8[3].i_ref);
            for (i = 0; i < 4; i++) {
                int costs[4] = {a->l0.i_cost4x4[i], a->l0.i_cost8x4[i], a->l0.i_cost4x8[i], a->l0.me8x8[i].cost};
                int thresh = X264_MIN4(costs[0], costs[1], costs[2], costs[3]) * 5 / 4;
                int subtype, btype = D_L0_8x8;
                uint64_t bcost = COST_MAX64;
                for (subtype = D_L0_4x4; subtype <= D_L0_8x8; subtype++) {
                    uint64_t cost;
                    if (costs[subtype] > thresh || (subtype == D_L0_8x8 && bcost == COST_MAX64)) {
                        continue;
                    }
                    h->mb.i_sub_partition[i] = subtype;
                    x264_mb_cache_mv_p8x8(h, a, i);
                    cost = x264_rd_cost_part(h, a->i_lambda2, i << 2, PIXEL_8x8);
                    COPY2_IF_LT(bcost, cost, btype, subtype);
                }
                h->mb.i_sub_partition[i] = btype;
                x264_mb_cache_mv_p8x8(h, a, i);
            }
        } else {
            x264_analyse_update_cache(h, a);
        }
        a->l0.i_cost8x8 = x264_rd_cost_mb(h, a->i_lambda2);
    } else {
        a->l0.i_cost8x8 = COST_MAX;
    }
}

static void x264_mb_analyse_b_rd(x264_t *h, x264_mb_analysis_t *a, int i_satd_inter) {
    int thresh = i_satd_inter * (17 + (!!h->mb.i_psy_rd)) / 16;

    if (a->b_direct_available && a->i_rd16x16direct == COST_MAX) {
        h->mb.i_type = B_DIRECT;

        h->mb.b_skip_mc = 1;
        x264_analyse_update_cache(h, a);
        a->i_rd16x16direct = x264_rd_cost_mb(h, a->i_lambda2);
        h->mb.b_skip_mc = 0;
    }

    h->mb.i_partition = D_16x16;

    if (a->l0.me16x16.cost <= thresh && a->l0.i_rd16x16 == COST_MAX) {
        h->mb.i_type = B_L0_L0;
        x264_analyse_update_cache(h, a);
        a->l0.i_rd16x16 = x264_rd_cost_mb(h, a->i_lambda2);
    }

    if (a->l1.me16x16.cost <= thresh && a->l1.i_rd16x16 == COST_MAX) {
        h->mb.i_type = B_L1_L1;
        x264_analyse_update_cache(h, a);
        a->l1.i_rd16x16 = x264_rd_cost_mb(h, a->i_lambda2);
    }

    if (a->i_cost16x16bi <= thresh && a->i_rd16x16bi == COST_MAX) {
        h->mb.i_type = B_BI_BI;
        x264_analyse_update_cache(h, a);
        a->i_rd16x16bi = x264_rd_cost_mb(h, a->i_lambda2);
    }

    if (a->i_cost8x8bi <= thresh && a->i_rd8x8bi == COST_MAX) {
        h->mb.i_type = B_8x8;
        h->mb.i_partition = D_8x8;
        x264_analyse_update_cache(h, a);
        a->i_rd8x8bi = x264_rd_cost_mb(h, a->i_lambda2);
        x264_macroblock_cache_skip(h, 0, 0, 4, 4, 0);
    }

    if (a->i_cost16x8bi <= thresh && a->i_rd16x8bi == COST_MAX) {
        h->mb.i_type = a->i_mb_type16x8;
        h->mb.i_partition = D_16x8;
        x264_analyse_update_cache(h, a);
        a->i_rd16x8bi = x264_rd_cost_mb(h, a->i_lambda2);
    }

    if (a->i_cost8x16bi <= thresh && a->i_rd8x16bi == COST_MAX) {
        h->mb.i_type = a->i_mb_type8x16;
        h->mb.i_partition = D_8x16;
        x264_analyse_update_cache(h, a);
        a->i_rd8x16bi = x264_rd_cost_mb(h, a->i_lambda2);
    }
}

static void x264_refine_bidir(x264_t *h, x264_mb_analysis_t *a) {
    const int i_biweight = h->mb.bipred_weight[a->l0.i_ref][a->l1.i_ref];
    int i;

    if (IS_INTRA(h->mb.i_type)) {
        return;
    }

    switch (h->mb.i_partition) {
    case D_16x16:
        if (h->mb.i_type == B_BI_BI) {
            x264_me_refine_bidir_satd(h, &a->l0.me16x16, &a->l1.me16x16, i_biweight);
        }
        break;
    case D_16x8:
        for (i = 0; i < 2; i++) {
            if (a->i_mb_partition16x8[i] == D_BI_8x8) {
                x264_me_refine_bidir_satd(h, &a->l0.me16x8[i], &a->l1.me16x8[i], i_biweight);
            }
        }
        break;
    case D_8x16:
        for (i = 0; i < 2; i++) {
            if (a->i_mb_partition8x16[i] == D_BI_8x8) {
                x264_me_refine_bidir_satd(h, &a->l0.me8x16[i], &a->l1.me8x16[i], i_biweight);
            }
        }
        break;
    case D_8x8:
        for (i = 0; i < 4; i++) {
            if (h->mb.i_sub_partition[i] == D_BI_8x8) {
                x264_me_refine_bidir_satd(h, &a->l0.me8x8[i], &a->l1.me8x8[i], i_biweight);
            }
        }
        break;
    }
}

static inline void x264_mb_analyse_transform(x264_t *h) {
    if (x264_mb_transform_8x8_allowed(h) && h->param.analyse.b_transform_8x8 && !h->mb.b_lossless) {
        int i_cost4, i_cost8;

        x264_mb_mc(h);

        i_cost8 = h->pixf.sa8d[PIXEL_16x16](h->mb.pic.p_fenc[0], FENC_STRIDE, h->mb.pic.p_fdec[0], FDEC_STRIDE);
        i_cost4 = h->pixf.satd[PIXEL_16x16](h->mb.pic.p_fenc[0], FENC_STRIDE, h->mb.pic.p_fdec[0], FDEC_STRIDE);

        h->mb.b_transform_8x8 = i_cost8 < i_cost4;
        h->mb.b_skip_mc = 1;
    }
}

static inline void x264_mb_analyse_transform_rd(x264_t *h, x264_mb_analysis_t *a, int *i_satd, int *i_rd) {
    if (x264_mb_transform_8x8_allowed(h) && h->param.analyse.b_transform_8x8) {
        int i_rd8;
        x264_analyse_update_cache(h, a);
        h->mb.b_transform_8x8 = !h->mb.b_transform_8x8;

        i_rd8 = x264_rd_cost_mb(h, a->i_lambda2);

        if (*i_rd >= i_rd8) {
            if (*i_rd > 0) {
                *i_satd = (int64_t)(*i_satd) * i_rd8 / *i_rd;
            }

            if (*i_satd == 0) {
                *i_satd = 1;
            }

            *i_rd = i_rd8;
        } else {
            h->mb.b_transform_8x8 = !h->mb.b_transform_8x8;
        }
    }
}

void x264_macroblock_analyse(x264_t *h) {
    x264_mb_analysis_t analysis;
    int i_cost = COST_MAX;
    int i;

    h->mb.i_qp = x264_ratecontrol_qp(h);
    if (h->param.rc.i_aq_mode) {
        x264_adaptive_quant(h);
    }

    x264_mb_analyse_init(h, &analysis, h->mb.i_qp);

    if (h->sh.i_type == SLICE_TYPE_I) {
        if (analysis.i_mbrd) {
            x264_mb_cache_fenc_satd(h);
        }
        x264_mb_analyse_intra(h, &analysis, COST_MAX);
        if (analysis.i_mbrd) {
            x264_intra_rd(h, &analysis, COST_MAX);
        }

        i_cost = analysis.i_satd_i16x16;
        h->mb.i_type = I_16x16;
        COPY2_IF_LT(i_cost, analysis.i_satd_i4x4, h->mb.i_type, I_4x4);
        COPY2_IF_LT(i_cost, analysis.i_satd_i8x8, h->mb.i_type, I_8x8);
        if (analysis.i_satd_pcm < i_cost) {
            h->mb.i_type = I_PCM;

        } else if (analysis.i_mbrd >= 2) {
            x264_intra_rd_refine(h, &analysis);
        }
    } else if (h->sh.i_type == SLICE_TYPE_P) {
        int b_skip = 0;
        int i_intra_cost, i_intra_type;

        h->mc.prefetch_ref(h->mb.pic.p_fref[0][0][h->mb.i_mb_x & 3], h->mb.pic.i_stride[0], 0);

        analysis.b_try_pskip = 0;
        if (h->param.analyse.b_fast_pskip) {
            if (h->param.i_threads > 1 && h->mb.cache.pskip_mv[1] > h->mb.mv_max_spel[1])

            {
            } else if (h->param.analyse.i_subpel_refine >= 3) {
                analysis.b_try_pskip = 1;
            } else if (h->mb.i_mb_type_left == P_SKIP || h->mb.i_mb_type_top == P_SKIP || h->mb.i_mb_type_topleft == P_SKIP || h->mb.i_mb_type_topright == P_SKIP) {
                b_skip = x264_macroblock_probe_pskip(h);
            }
        }

        h->mc.prefetch_ref(h->mb.pic.p_fref[0][0][h->mb.i_mb_x & 3], h->mb.pic.i_stride[0], 1);

        if (b_skip) {
            h->mb.i_type = P_SKIP;
            h->mb.i_partition = D_16x16;
            assert(h->mb.cache.pskip_mv[1] <= h->mb.mv_max_spel[1] || h->param.i_threads == 1);
        } else {
            const unsigned int flags = h->param.analyse.inter;
            int i_type;
            int i_partition;
            int i_thresh16x8;
            int i_satd_inter, i_satd_intra;

            x264_mb_analyse_load_costs(h, &analysis);

            x264_mb_analyse_inter_p16x16(h, &analysis);

            if (h->mb.i_type == P_SKIP) {
                return;
            }

            if (flags & X264_ANALYSE_PSUB16x16) {
                if (h->param.analyse.b_mixed_references) {
                    x264_mb_analyse_inter_p8x8_mixed_ref(h, &analysis);
                } else {
                    x264_mb_analyse_inter_p8x8(h, &analysis);
                }
            }

            i_type = P_L0;
            i_partition = D_16x16;
            i_cost = analysis.l0.me16x16.cost;

            if ((flags & X264_ANALYSE_PSUB16x16) && analysis.l0.i_cost8x8 < analysis.l0.me16x16.cost) {
                i_type = P_8x8;
                i_partition = D_8x8;
                i_cost = analysis.l0.i_cost8x8;

                if (flags & X264_ANALYSE_PSUB8x8) {
                    for (i = 0; i < 4; i++) {
                        x264_mb_analyse_inter_p4x4(h, &analysis, i);
                        if (analysis.l0.i_cost4x4[i] < analysis.l0.me8x8[i].cost) {
                            int i_cost8x8 = analysis.l0.i_cost4x4[i];
                            h->mb.i_sub_partition[i] = D_L0_4x4;

                            x264_mb_analyse_inter_p8x4(h, &analysis, i);
                            COPY2_IF_LT(i_cost8x8, analysis.l0.i_cost8x4[i], h->mb.i_sub_partition[i], D_L0_8x4);

                            x264_mb_analyse_inter_p4x8(h, &analysis, i);
                            COPY2_IF_LT(i_cost8x8, analysis.l0.i_cost4x8[i], h->mb.i_sub_partition[i], D_L0_4x8);

                            i_cost += i_cost8x8 - analysis.l0.me8x8[i].cost;
                        }
                        x264_mb_cache_mv_p8x8(h, &analysis, i);
                    }
                    analysis.l0.i_cost8x8 = i_cost;
                }
            }

            i_thresh16x8 = analysis.l0.me8x8[1].cost_mv + analysis.l0.me8x8[2].cost_mv;
            if ((flags & X264_ANALYSE_PSUB16x16) && analysis.l0.i_cost8x8 < analysis.l0.me16x16.cost + i_thresh16x8) {
                x264_mb_analyse_inter_p16x8(h, &analysis);
                COPY3_IF_LT(i_cost, analysis.l0.i_cost16x8, i_type, P_L0, i_partition, D_16x8);

                x264_mb_analyse_inter_p8x16(h, &analysis);
                COPY3_IF_LT(i_cost, analysis.l0.i_cost8x16, i_type, P_L0, i_partition, D_8x16);
            }

            h->mb.i_partition = i_partition;

            if (analysis.i_mbrd) {

            } else if (i_partition == D_16x16) {
                x264_me_refine_qpel(h, &analysis.l0.me16x16);
                i_cost = analysis.l0.me16x16.cost;
            } else if (i_partition == D_16x8) {
                x264_me_refine_qpel(h, &analysis.l0.me16x8[0]);
                x264_me_refine_qpel(h, &analysis.l0.me16x8[1]);
                i_cost = analysis.l0.me16x8[0].cost + analysis.l0.me16x8[1].cost;
            } else if (i_partition == D_8x16) {
                x264_me_refine_qpel(h, &analysis.l0.me8x16[0]);
                x264_me_refine_qpel(h, &analysis.l0.me8x16[1]);
                i_cost = analysis.l0.me8x16[0].cost + analysis.l0.me8x16[1].cost;
            } else if (i_partition == D_8x8) {
                int i8x8;
                i_cost = 0;
                for (i8x8 = 0; i8x8 < 4; i8x8++) {
                    switch (h->mb.i_sub_partition[i8x8]) {
                    case D_L0_8x8:
                        x264_me_refine_qpel(h, &analysis.l0.me8x8[i8x8]);
                        i_cost += analysis.l0.me8x8[i8x8].cost;
                        break;
                    case D_L0_8x4:
                        x264_me_refine_qpel(h, &analysis.l0.me8x4[i8x8][0]);
                        x264_me_refine_qpel(h, &analysis.l0.me8x4[i8x8][1]);
                        i_cost += analysis.l0.me8x4[i8x8][0].cost + analysis.l0.me8x4[i8x8][1].cost;
                        break;
                    case D_L0_4x8:
                        x264_me_refine_qpel(h, &analysis.l0.me4x8[i8x8][0]);
                        x264_me_refine_qpel(h, &analysis.l0.me4x8[i8x8][1]);
                        i_cost += analysis.l0.me4x8[i8x8][0].cost + analysis.l0.me4x8[i8x8][1].cost;
                        break;

                    case D_L0_4x4:
                        x264_me_refine_qpel(h, &analysis.l0.me4x4[i8x8][0]);
                        x264_me_refine_qpel(h, &analysis.l0.me4x4[i8x8][1]);
                        x264_me_refine_qpel(h, &analysis.l0.me4x4[i8x8][2]);
                        x264_me_refine_qpel(h, &analysis.l0.me4x4[i8x8][3]);
                        i_cost += analysis.l0.me4x4[i8x8][0].cost + analysis.l0.me4x4[i8x8][1].cost + analysis.l0.me4x4[i8x8][2].cost + analysis.l0.me4x4[i8x8][3].cost;
                        break;
                    default:
                        x264_log(h, X264_LOG_ERROR, "internal error (!8x8 && !4x4)\n");
                        break;
                    }
                }
            }

            if (h->mb.b_chroma_me) {
                x264_mb_analyse_intra_chroma(h, &analysis);
                x264_mb_analyse_intra(h, &analysis, i_cost - analysis.i_satd_i8x8chroma);
                analysis.i_satd_i16x16 += analysis.i_satd_i8x8chroma;
                analysis.i_satd_i8x8 += analysis.i_satd_i8x8chroma;
                analysis.i_satd_i4x4 += analysis.i_satd_i8x8chroma;
            } else {
                x264_mb_analyse_intra(h, &analysis, i_cost);
            }

            i_satd_inter = i_cost;
            i_satd_intra = X264_MIN3(analysis.i_satd_i16x16, analysis.i_satd_i8x8, analysis.i_satd_i4x4);

            if (analysis.i_mbrd) {
                x264_mb_analyse_p_rd(h, &analysis, X264_MIN(i_satd_inter, i_satd_intra));
                i_type = P_L0;
                i_partition = D_16x16;
                i_cost = analysis.l0.me16x16.cost;
                COPY2_IF_LT(i_cost, analysis.l0.i_cost16x8, i_partition, D_16x8);
                COPY2_IF_LT(i_cost, analysis.l0.i_cost8x16, i_partition, D_8x16);
                COPY3_IF_LT(i_cost, analysis.l0.i_cost8x8, i_partition, D_8x8, i_type, P_8x8);
                h->mb.i_type = i_type;
                h->mb.i_partition = i_partition;
                if (i_cost < COST_MAX) {
                    x264_mb_analyse_transform_rd(h, &analysis, &i_satd_inter, &i_cost);
                }
                x264_intra_rd(h, &analysis, i_satd_inter * 5 / 4);
            }

            i_intra_type = I_16x16;
            i_intra_cost = analysis.i_satd_i16x16;
            COPY2_IF_LT(i_intra_cost, analysis.i_satd_i8x8, i_intra_type, I_8x8);
            COPY2_IF_LT(i_intra_cost, analysis.i_satd_i4x4, i_intra_type, I_4x4);
            COPY2_IF_LT(i_intra_cost, analysis.i_satd_pcm, i_intra_type, I_PCM);
            COPY2_IF_LT(i_cost, i_intra_cost, i_type, i_intra_type);

            if (i_intra_cost == COST_MAX) {
                i_intra_cost = i_cost * i_satd_intra / i_satd_inter + 1;
            }

            h->mb.i_type = i_type;
            h->stat.frame.i_intra_cost += i_intra_cost;
            h->stat.frame.i_inter_cost += i_cost;
            h->stat.frame.i_mbs_analysed++;

            if (analysis.i_mbrd >= 2 && h->mb.i_type != I_PCM) {
                if (IS_INTRA(h->mb.i_type)) {
                    x264_intra_rd_refine(h, &analysis);
                } else if (i_partition == D_16x16) {
                    x264_macroblock_cache_ref(h, 0, 0, 4, 4, 0, analysis.l0.me16x16.i_ref);
                    x264_me_refine_qpel_rd(h, &analysis.l0.me16x16, analysis.i_lambda2, 0, 0);
                } else if (i_partition == D_16x8) {
                    h->mb.i_sub_partition[0] = h->mb.i_sub_partition[1] = h->mb.i_sub_partition[2] = h->mb.i_sub_partition[3] = D_L0_8x8;
                    x264_macroblock_cache_ref(h, 0, 0, 4, 2, 0, analysis.l0.me16x8[0].i_ref);
                    x264_macroblock_cache_ref(h, 0, 2, 4, 2, 0, analysis.l0.me16x8[1].i_ref);
                    x264_me_refine_qpel_rd(h, &analysis.l0.me16x8[0], analysis.i_lambda2, 0, 0);
                    x264_me_refine_qpel_rd(h, &analysis.l0.me16x8[1], analysis.i_lambda2, 8, 0);
                } else if (i_partition == D_8x16) {
                    h->mb.i_sub_partition[0] = h->mb.i_sub_partition[1] = h->mb.i_sub_partition[2] = h->mb.i_sub_partition[3] = D_L0_8x8;
                    x264_macroblock_cache_ref(h, 0, 0, 2, 4, 0, analysis.l0.me8x16[0].i_ref);
                    x264_macroblock_cache_ref(h, 2, 0, 2, 4, 0, analysis.l0.me8x16[1].i_ref);
                    x264_me_refine_qpel_rd(h, &analysis.l0.me8x16[0], analysis.i_lambda2, 0, 0);
                    x264_me_refine_qpel_rd(h, &analysis.l0.me8x16[1], analysis.i_lambda2, 4, 0);
                } else if (i_partition == D_8x8) {
                    int i8x8;
                    x264_analyse_update_cache(h, &analysis);
                    for (i8x8 = 0; i8x8 < 4; i8x8++) {
                        if (h->mb.i_sub_partition[i8x8] == D_L0_8x8) {
                            x264_me_refine_qpel_rd(h, &analysis.l0.me8x8[i8x8], analysis.i_lambda2, i8x8 * 4, 0);
                        } else if (h->mb.i_sub_partition[i8x8] == D_L0_8x4) {
                            x264_me_refine_qpel_rd(h, &analysis.l0.me8x4[i8x8][0], analysis.i_lambda2, i8x8 * 4 + 0, 0);
                            x264_me_refine_qpel_rd(h, &analysis.l0.me8x4[i8x8][1], analysis.i_lambda2, i8x8 * 4 + 2, 0);
                        } else if (h->mb.i_sub_partition[i8x8] == D_L0_4x8) {
                            x264_me_refine_qpel_rd(h, &analysis.l0.me4x8[i8x8][0], analysis.i_lambda2, i8x8 * 4 + 0, 0);
                            x264_me_refine_qpel_rd(h, &analysis.l0.me4x8[i8x8][1], analysis.i_lambda2, i8x8 * 4 + 1, 0);
                        } else if (h->mb.i_sub_partition[i8x8] == D_L0_4x4) {
                            x264_me_refine_qpel_rd(h, &analysis.l0.me4x4[i8x8][0], analysis.i_lambda2, i8x8 * 4 + 0, 0);
                            x264_me_refine_qpel_rd(h, &analysis.l0.me4x4[i8x8][1], analysis.i_lambda2, i8x8 * 4 + 1, 0);
                            x264_me_refine_qpel_rd(h, &analysis.l0.me4x4[i8x8][2], analysis.i_lambda2, i8x8 * 4 + 2, 0);
                            x264_me_refine_qpel_rd(h, &analysis.l0.me4x4[i8x8][3], analysis.i_lambda2, i8x8 * 4 + 3, 0);
                        }
                    }
                }
            }
        }
    } else if (h->sh.i_type == SLICE_TYPE_B) {
        int i_bskip_cost = COST_MAX;
        int b_skip = 0;

        if (analysis.i_mbrd) {
            x264_mb_cache_fenc_satd(h);
        }

        h->mb.i_type = B_SKIP;
        if (h->mb.b_direct_auto_write) {

            for (i = 0; i < 2; i++) {
                int b_changed = 1;
                h->sh.b_direct_spatial_mv_pred ^= 1;
                analysis.b_direct_available = x264_mb_predict_mv_direct16x16(h, i && analysis.b_direct_available ? &b_changed : NULL);
                if (analysis.b_direct_available) {
                    if (b_changed) {
                        x264_mb_mc(h);
                        b_skip = x264_macroblock_probe_bskip(h);
                    }
                    h->stat.frame.i_direct_score[h->sh.b_direct_spatial_mv_pred] += b_skip;
                } else {
                    b_skip = 0;
                }
            }
        } else {
            analysis.b_direct_available = x264_mb_predict_mv_direct16x16(h, NULL);
        }

        if (analysis.b_direct_available) {
            if (!h->mb.b_direct_auto_write) {
                x264_mb_mc(h);
            }
            if (h->mb.b_lossless) {

            } else if (analysis.i_mbrd) {
                i_bskip_cost = ssd_mb(h);

                b_skip = h->mb.b_skip_mc = i_bskip_cost <= ((6 * analysis.i_lambda2 + 128) >> 8);
            } else if (!h->mb.b_direct_auto_write) {

                b_skip = x264_macroblock_probe_bskip(h);
            }
        }

        if (!b_skip) {
            const unsigned int flags = h->param.analyse.inter;
            int i_type;
            int i_partition;
            int i_satd_inter = 0;
            h->mb.b_skip_mc = 0;

            x264_mb_analyse_load_costs(h, &analysis);

            if (analysis.b_direct_available) {
                x264_mb_analyse_inter_direct(h, &analysis);
            }

            x264_mb_analyse_inter_b16x16(h, &analysis);

            i_type = B_L0_L0;
            i_partition = D_16x16;
            i_cost = analysis.l0.me16x16.cost;
            COPY2_IF_LT(i_cost, analysis.l1.me16x16.cost, i_type, B_L1_L1);
            COPY2_IF_LT(i_cost, analysis.i_cost16x16bi, i_type, B_BI_BI);
            COPY2_IF_LT(i_cost, analysis.i_cost16x16direct, i_type, B_DIRECT);

            if (analysis.i_mbrd && analysis.i_cost16x16direct <= i_cost * 33 / 32) {
                x264_mb_analyse_b_rd(h, &analysis, i_cost);
                if (i_bskip_cost < analysis.i_rd16x16direct && i_bskip_cost < analysis.i_rd16x16bi && i_bskip_cost < analysis.l0.i_rd16x16 && i_bskip_cost < analysis.l1.i_rd16x16) {
                    h->mb.i_type = B_SKIP;
                    x264_analyse_update_cache(h, &analysis);
                    return;
                }
            }

            if (flags & X264_ANALYSE_BSUB16x16) {
                x264_mb_analyse_inter_b8x8(h, &analysis);
                if (analysis.i_cost8x8bi < i_cost) {
                    i_type = B_8x8;
                    i_partition = D_8x8;
                    i_cost = analysis.i_cost8x8bi;

                    if (h->mb.i_sub_partition[0] == h->mb.i_sub_partition[1] || h->mb.i_sub_partition[2] == h->mb.i_sub_partition[3]) {
                        x264_mb_analyse_inter_b16x8(h, &analysis);
                        COPY3_IF_LT(i_cost, analysis.i_cost16x8bi, i_type, analysis.i_mb_type16x8, i_partition, D_16x8);
                    }
                    if (h->mb.i_sub_partition[0] == h->mb.i_sub_partition[2] || h->mb.i_sub_partition[1] == h->mb.i_sub_partition[3]) {
                        x264_mb_analyse_inter_b8x16(h, &analysis);
                        COPY3_IF_LT(i_cost, analysis.i_cost8x16bi, i_type, analysis.i_mb_type8x16, i_partition, D_8x16);
                    }
                }
            }

            if (analysis.i_mbrd) {

            }

            else if (i_partition == D_16x16) {
                analysis.l0.me16x16.cost -= analysis.i_lambda * i_mb_b_cost_table[B_L0_L0];
                analysis.l1.me16x16.cost -= analysis.i_lambda * i_mb_b_cost_table[B_L1_L1];
                if (i_type == B_L0_L0) {
                    x264_me_refine_qpel(h, &analysis.l0.me16x16);
                    i_cost = analysis.l0.me16x16.cost + analysis.i_lambda * i_mb_b_cost_table[B_L0_L0];
                } else if (i_type == B_L1_L1) {
                    x264_me_refine_qpel(h, &analysis.l1.me16x16);
                    i_cost = analysis.l1.me16x16.cost + analysis.i_lambda * i_mb_b_cost_table[B_L1_L1];
                } else if (i_type == B_BI_BI) {
                    x264_me_refine_qpel(h, &analysis.l0.me16x16);
                    x264_me_refine_qpel(h, &analysis.l1.me16x16);
                }
            } else if (i_partition == D_16x8) {
                for (i = 0; i < 2; i++) {
                    if (analysis.i_mb_partition16x8[i] != D_L1_8x8) {
                        x264_me_refine_qpel(h, &analysis.l0.me16x8[i]);
                    }
                    if (analysis.i_mb_partition16x8[i] != D_L0_8x8) {
                        x264_me_refine_qpel(h, &analysis.l1.me16x8[i]);
                    }
                }
            } else if (i_partition == D_8x16) {
                for (i = 0; i < 2; i++) {
                    if (analysis.i_mb_partition8x16[i] != D_L1_8x8) {
                        x264_me_refine_qpel(h, &analysis.l0.me8x16[i]);
                    }
                    if (analysis.i_mb_partition8x16[i] != D_L0_8x8) {
                        x264_me_refine_qpel(h, &analysis.l1.me8x16[i]);
                    }
                }
            } else if (i_partition == D_8x8) {
                for (i = 0; i < 4; i++) {
                    x264_me_t *m;
                    int i_part_cost_old;
                    int i_type_cost;
                    int i_part_type = h->mb.i_sub_partition[i];
                    int b_bidir = (i_part_type == D_BI_8x8);

                    if (i_part_type == D_DIRECT_8x8) {
                        continue;
                    }
                    if (x264_mb_partition_listX_table[0][i_part_type]) {
                        m = &analysis.l0.me8x8[i];
                        i_part_cost_old = m->cost;
                        i_type_cost = analysis.i_lambda * i_sub_mb_b_cost_table[D_L0_8x8];
                        m->cost -= i_type_cost;
                        x264_me_refine_qpel(h, m);
                        if (!b_bidir) {
                            analysis.i_cost8x8bi += m->cost + i_type_cost - i_part_cost_old;
                        }
                    }
                    if (x264_mb_partition_listX_table[1][i_part_type]) {
                        m = &analysis.l1.me8x8[i];
                        i_part_cost_old = m->cost;
                        i_type_cost = analysis.i_lambda * i_sub_mb_b_cost_table[D_L1_8x8];
                        m->cost -= i_type_cost;
                        x264_me_refine_qpel(h, m);
                        if (!b_bidir) {
                            analysis.i_cost8x8bi += m->cost + i_type_cost - i_part_cost_old;
                        }
                    }
                }
            }

            if (analysis.i_mbrd) {
                i_satd_inter = i_cost;
                x264_mb_analyse_b_rd(h, &analysis, i_satd_inter);
                i_type = B_SKIP;
                i_cost = i_bskip_cost;
                i_partition = D_16x16;
                COPY2_IF_LT(i_cost, analysis.l0.i_rd16x16, i_type, B_L0_L0);
                COPY2_IF_LT(i_cost, analysis.l1.i_rd16x16, i_type, B_L1_L1);
                COPY2_IF_LT(i_cost, analysis.i_rd16x16bi, i_type, B_BI_BI);
                COPY2_IF_LT(i_cost, analysis.i_rd16x16direct, i_type, B_DIRECT);
                COPY3_IF_LT(i_cost, analysis.i_rd16x8bi, i_type, analysis.i_mb_type16x8, i_partition, D_16x8);
                COPY3_IF_LT(i_cost, analysis.i_rd8x16bi, i_type, analysis.i_mb_type8x16, i_partition, D_8x16);
                COPY3_IF_LT(i_cost, analysis.i_rd8x8bi, i_type, B_8x8, i_partition, D_8x8);

                h->mb.i_type = i_type;
                h->mb.i_partition = i_partition;
            }

            x264_mb_analyse_intra(h, &analysis, i_satd_inter);

            if (analysis.i_mbrd) {
                x264_mb_analyse_transform_rd(h, &analysis, &i_satd_inter, &i_cost);
                x264_intra_rd(h, &analysis, i_satd_inter * 17 / 16);
            }

            COPY2_IF_LT(i_cost, analysis.i_satd_i16x16, i_type, I_16x16);
            COPY2_IF_LT(i_cost, analysis.i_satd_i8x8, i_type, I_8x8);
            COPY2_IF_LT(i_cost, analysis.i_satd_i4x4, i_type, I_4x4);
            COPY2_IF_LT(i_cost, analysis.i_satd_pcm, i_type, I_PCM);

            h->mb.i_type = i_type;
            h->mb.i_partition = i_partition;

            if (analysis.i_mbrd >= 2 && IS_INTRA(i_type) && i_type != I_PCM) {
                x264_intra_rd_refine(h, &analysis);
            }
            if (h->mb.i_subpel_refine >= 5) {
                x264_refine_bidir(h, &analysis);
            }

            if (analysis.i_mbrd >= 2 && i_type > B_DIRECT && i_type < B_SKIP) {
                const int i_biweight = h->mb.bipred_weight[analysis.l0.i_ref][analysis.l1.i_ref];
                x264_analyse_update_cache(h, &analysis);

                if (i_partition == D_16x16) {
                    if (i_type == B_L0_L0) {
                        x264_me_refine_qpel_rd(h, &analysis.l0.me16x16, analysis.i_lambda2, 0, 0);
                    } else if (i_type == B_L1_L1) {
                        x264_me_refine_qpel_rd(h, &analysis.l1.me16x16, analysis.i_lambda2, 0, 1);
                    } else if (i_type == B_BI_BI) {
                        x264_me_refine_bidir_rd(h, &analysis.l0.me16x16, &analysis.l1.me16x16, i_biweight, 0, analysis.i_lambda2);
                    }
                } else if (i_partition == D_16x8) {
                    for (i = 0; i < 2; i++) {
                        h->mb.i_sub_partition[i * 2] = h->mb.i_sub_partition[i * 2 + 1] = analysis.i_mb_partition16x8[i];
                        if (analysis.i_mb_partition16x8[i] == D_L0_8x8) {
                            x264_me_refine_qpel_rd(h, &analysis.l0.me16x8[i], analysis.i_lambda2, i * 8, 0);
                        } else if (analysis.i_mb_partition16x8[i] == D_L1_8x8) {
                            x264_me_refine_qpel_rd(h, &analysis.l1.me16x8[i], analysis.i_lambda2, i * 8, 1);
                        } else if (analysis.i_mb_partition16x8[i] == D_BI_8x8) {
                            x264_me_refine_bidir_rd(h, &analysis.l0.me16x8[i], &analysis.l1.me16x8[i], i_biweight, i * 2, analysis.i_lambda2);
                        }
                    }
                } else if (i_partition == D_8x16) {
                    for (i = 0; i < 2; i++) {
                        h->mb.i_sub_partition[i] = h->mb.i_sub_partition[i + 2] = analysis.i_mb_partition8x16[i];
                        if (analysis.i_mb_partition8x16[i] == D_L0_8x8) {
                            x264_me_refine_qpel_rd(h, &analysis.l0.me8x16[i], analysis.i_lambda2, i * 4, 0);
                        } else if (analysis.i_mb_partition8x16[i] == D_L1_8x8) {
                            x264_me_refine_qpel_rd(h, &analysis.l1.me8x16[i], analysis.i_lambda2, i * 4, 1);
                        } else if (analysis.i_mb_partition8x16[i] == D_BI_8x8) {
                            x264_me_refine_bidir_rd(h, &analysis.l0.me8x16[i], &analysis.l1.me8x16[i], i_biweight, i, analysis.i_lambda2);
                        }
                    }
                } else if (i_partition == D_8x8) {
                    for (i = 0; i < 4; i++) {
                        if (h->mb.i_sub_partition[i] == D_L0_8x8) {
                            x264_me_refine_qpel_rd(h, &analysis.l0.me8x8[i], analysis.i_lambda2, i * 4, 0);
                        } else if (h->mb.i_sub_partition[i] == D_L1_8x8) {
                            x264_me_refine_qpel_rd(h, &analysis.l1.me8x8[i], analysis.i_lambda2, i * 4, 1);
                        } else if (h->mb.i_sub_partition[i] == D_BI_8x8) {
                            x264_me_refine_bidir_rd(h, &analysis.l0.me8x8[i], &analysis.l1.me8x8[i], i_biweight, i, analysis.i_lambda2);
                        }
                    }
                }
            }
        }
    }

    x264_analyse_update_cache(h, &analysis);

    if (!analysis.i_mbrd) {
        x264_mb_analyse_transform(h);
    }

    h->mb.b_trellis = h->param.analyse.i_trellis;
    h->mb.b_noise_reduction = !!h->param.analyse.i_noise_reduction;
    if (!IS_SKIP(h->mb.i_type) && h->mb.i_psy_trellis && h->param.analyse.i_trellis == 1) {
        x264_psy_trellis_init(h, 0);
    }
    if (h->mb.b_trellis == 1 || h->mb.b_noise_reduction) {
        h->mb.i_skip_intra = 0;
    }
}

static void x264_analyse_update_cache(x264_t *h, x264_mb_analysis_t *a) {
    int i;

    switch (h->mb.i_type) {
    case I_4x4:
        for (i = 0; i < 16; i++) {
            h->mb.cache.intra4x4_pred_mode[x264_scan8[i]] = a->i_predict4x4[i];
        }

        x264_mb_analyse_intra_chroma(h, a);
        break;
    case I_8x8:
        for (i = 0; i < 4; i++) {
            x264_macroblock_cache_intra8x8_pred(h, 2 * (i & 1), 2 * (i >> 1), a->i_predict8x8[i]);
        }

        x264_mb_analyse_intra_chroma(h, a);
        break;
    case I_16x16:
        h->mb.i_intra16x16_pred_mode = a->i_predict16x16;
        x264_mb_analyse_intra_chroma(h, a);
        break;

    case I_PCM:
        break;

    case P_L0:
        switch (h->mb.i_partition) {
        case D_16x16:
            x264_macroblock_cache_ref(h, 0, 0, 4, 4, 0, a->l0.me16x16.i_ref);
            x264_macroblock_cache_mv_ptr(h, 0, 0, 4, 4, 0, a->l0.me16x16.mv);
            break;

        case D_16x8:
            x264_macroblock_cache_ref(h, 0, 0, 4, 2, 0, a->l0.me16x8[0].i_ref);
            x264_macroblock_cache_ref(h, 0, 2, 4, 2, 0, a->l0.me16x8[1].i_ref);
            x264_macroblock_cache_mv_ptr(h, 0, 0, 4, 2, 0, a->l0.me16x8[0].mv);
            x264_macroblock_cache_mv_ptr(h, 0, 2, 4, 2, 0, a->l0.me16x8[1].mv);
            break;

        case D_8x16:
            x264_macroblock_cache_ref(h, 0, 0, 2, 4, 0, a->l0.me8x16[0].i_ref);
            x264_macroblock_cache_ref(h, 2, 0, 2, 4, 0, a->l0.me8x16[1].i_ref);
            x264_macroblock_cache_mv_ptr(h, 0, 0, 2, 4, 0, a->l0.me8x16[0].mv);
            x264_macroblock_cache_mv_ptr(h, 2, 0, 2, 4, 0, a->l0.me8x16[1].mv);
            break;

        default:
            x264_log(h, X264_LOG_ERROR, "internal error P_L0 and partition=%d\n", h->mb.i_partition);
            break;
        }
        break;

    case P_8x8:
        x264_macroblock_cache_ref(h, 0, 0, 2, 2, 0, a->l0.me8x8[0].i_ref);
        x264_macroblock_cache_ref(h, 2, 0, 2, 2, 0, a->l0.me8x8[1].i_ref);
        x264_macroblock_cache_ref(h, 0, 2, 2, 2, 0, a->l0.me8x8[2].i_ref);
        x264_macroblock_cache_ref(h, 2, 2, 2, 2, 0, a->l0.me8x8[3].i_ref);
        for (i = 0; i < 4; i++) {
            x264_mb_cache_mv_p8x8(h, a, i);
        }
        break;

    case P_SKIP: {
        h->mb.i_partition = D_16x16;
        x264_macroblock_cache_ref(h, 0, 0, 4, 4, 0, 0);
        x264_macroblock_cache_mv_ptr(h, 0, 0, 4, 4, 0, h->mb.cache.pskip_mv);
        break;
    }

    case B_SKIP:
    case B_DIRECT:
        x264_mb_load_mv_direct8x8(h, 0);
        x264_mb_load_mv_direct8x8(h, 1);
        x264_mb_load_mv_direct8x8(h, 2);
        x264_mb_load_mv_direct8x8(h, 3);
        break;

    case B_8x8:

        for (i = 0; i < 4; i++) {
            x264_mb_cache_mv_b8x8(h, a, i, 1);
        }
        break;

    default:
        switch (h->mb.i_partition) {
        case D_16x16:
            switch (h->mb.i_type) {
            case B_L0_L0:
                x264_macroblock_cache_ref(h, 0, 0, 4, 4, 0, a->l0.i_ref);
                x264_macroblock_cache_mv_ptr(h, 0, 0, 4, 4, 0, a->l0.me16x16.mv);

                x264_macroblock_cache_ref(h, 0, 0, 4, 4, 1, -1);
                x264_macroblock_cache_mv(h, 0, 0, 4, 4, 1, 0);
                x264_macroblock_cache_mvd(h, 0, 0, 4, 4, 1, 0);
                break;
            case B_L1_L1:
                x264_macroblock_cache_ref(h, 0, 0, 4, 4, 0, -1);
                x264_macroblock_cache_mv(h, 0, 0, 4, 4, 0, 0);
                x264_macroblock_cache_mvd(h, 0, 0, 4, 4, 0, 0);

                x264_macroblock_cache_ref(h, 0, 0, 4, 4, 1, a->l1.i_ref);
                x264_macroblock_cache_mv_ptr(h, 0, 0, 4, 4, 1, a->l1.me16x16.mv);
                break;
            case B_BI_BI:
                x264_macroblock_cache_ref(h, 0, 0, 4, 4, 0, a->l0.i_ref);
                x264_macroblock_cache_mv_ptr(h, 0, 0, 4, 4, 0, a->l0.me16x16.mv);

                x264_macroblock_cache_ref(h, 0, 0, 4, 4, 1, a->l1.i_ref);
                x264_macroblock_cache_mv_ptr(h, 0, 0, 4, 4, 1, a->l1.me16x16.mv);
                break;
            }
            break;
        case D_16x8:
            x264_mb_cache_mv_b16x8(h, a, 0, 1);
            x264_mb_cache_mv_b16x8(h, a, 1, 1);
            break;
        case D_8x16:
            x264_mb_cache_mv_b8x16(h, a, 0, 1);
            x264_mb_cache_mv_b8x16(h, a, 1, 1);
            break;
        default:
            x264_log(h, X264_LOG_ERROR, "internal error (invalid MB type)\n");
            break;
        }
    }

#ifndef NDEBUG
    if (h->param.i_threads > 1 && !IS_INTRA(h->mb.i_type)) {
        int l;
        for (l = 0; l <= (h->sh.i_type == SLICE_TYPE_B); l++) {
            int completed;
            int ref = h->mb.cache.ref[l][x264_scan8[0]];
            if (ref < 0) {
                continue;
            }
            completed = (l ? h->fref1 : h->fref0)[ref >> h->mb.b_interlaced]->i_lines_completed;
            if ((h->mb.cache.mv[l][x264_scan8[15]][1] >> (2 - h->mb.b_interlaced)) + h->mb.i_mb_y * 16 > completed) {
                x264_log(h, X264_LOG_WARNING, "internal error (MV out of thread range)\n");
                fprintf(stderr, "mb type: %d \n", h->mb.i_type);
                fprintf(stderr, "mv: l%dr%d (%d,%d) \n", l, ref, h->mb.cache.mv[l][x264_scan8[15]][0], h->mb.cache.mv[l][x264_scan8[15]][1]);
                fprintf(stderr, "limit: %d \n", h->mb.mv_max_spel[1]);
                fprintf(stderr, "mb_xy: %d,%d \n", h->mb.i_mb_x, h->mb.i_mb_y);
                fprintf(stderr, "completed: %d \n", completed);
                x264_log(h, X264_LOG_WARNING, "recovering by using intra mode\n");
                x264_mb_analyse_intra(h, a, COST_MAX);
                h->mb.i_type = I_16x16;
                h->mb.i_intra16x16_pred_mode = a->i_predict16x16;
                x264_mb_analyse_intra_chroma(h, a);
            }
        }
    }
#endif
}

#include "slicetype.c"
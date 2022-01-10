#include "racebench_bugs.h"


#include <math.h>

#include "common/common.h"
#include "common/cpu.h"

#include "set.h"
#include "analyse.h"
#include "ratecontrol.h"
#include "macroblock.h"

#if VISUALIZE
#include "common/visualize.h"
#endif

#define NALU_OVERHEAD 5

#define bs_write_ue bs_write_ue_big

static void x264_encoder_frame_end(x264_t *h, x264_t *thread_current, x264_nal_t **pp_nal, int *pi_nal, x264_picture_t *pic_out);

static float x264_psnr(int64_t i_sqe, int64_t i_size) {
    double f_mse = (double)i_sqe / ((double)65025.0 * (double)i_size);
    if (f_mse <= 0.0000000001) {
        return 100;
    }

    return (float)(-10.0 * log(f_mse) / log(10.0));
}

static void x264_frame_dump(x264_t *h) {
    FILE *f = fopen(h->param.psz_dump_yuv, "r+b");
    int i, y;
    if (!f) {
        return;
    }

    fseek(f, h->fdec->i_frame * h->param.i_height * h->param.i_width * 3 / 2, SEEK_SET);
    for (i = 0; i < h->fdec->i_plane; i++) {
        for (y = 0; y < h->param.i_height >> !!i; y++) {
            fwrite(&h->fdec->plane[i][y * h->fdec->i_stride[i]], 1, h->param.i_width >> !!i, f);
        }
    }
    fclose(f);
}

static void x264_slice_header_init(x264_t *h, x264_slice_header_t *sh, x264_sps_t *sps, x264_pps_t *pps, int i_idr_pic_id, int i_frame, int i_qp) {
    x264_param_t *param = &h->param;
    int i;

    sh->sps = sps;
    sh->pps = pps;

    sh->i_first_mb = 0;
    sh->i_last_mb = h->sps->i_mb_width * h->sps->i_mb_height;
    sh->i_pps_id = pps->i_id;

    sh->i_frame_num = i_frame;

    sh->b_mbaff = h->param.b_interlaced;
    sh->b_field_pic = 0;
    sh->b_bottom_field = 0;

    sh->i_idr_pic_id = i_idr_pic_id;

    sh->i_poc_lsb = 0;
    sh->i_delta_poc_bottom = 0;
    sh->i_delta_poc[0] = 0;
    sh->i_delta_poc[1] = 0;

    sh->i_redundant_pic_cnt = 0;

    if (!h->mb.b_direct_auto_read) {
        if (h->mb.b_direct_auto_write) {
            sh->b_direct_spatial_mv_pred = (h->stat.i_direct_score[1] > h->stat.i_direct_score[0]);
        } else {
            sh->b_direct_spatial_mv_pred = (param->analyse.i_direct_mv_pred == X264_DIRECT_PRED_SPATIAL);
        }
    }

    sh->b_num_ref_idx_override = 0;
    sh->i_num_ref_idx_l0_active = 1;
    sh->i_num_ref_idx_l1_active = 1;

    sh->b_ref_pic_list_reordering_l0 = h->b_ref_reorder[0];
    sh->b_ref_pic_list_reordering_l1 = h->b_ref_reorder[1];

    if (sh->b_ref_pic_list_reordering_l0) {
        int pred_frame_num = i_frame;
        for (i = 0; i < h->i_ref0; i++) {
            int diff = h->fref0[i]->i_frame_num - pred_frame_num;
            if (diff == 0) {
                x264_log(h, X264_LOG_ERROR, "diff frame num == 0\n");
            }
            sh->ref_pic_list_order[0][i].idc = (diff > 0);
            sh->ref_pic_list_order[0][i].arg = abs(diff) - 1;
            pred_frame_num = h->fref0[i]->i_frame_num;
        }
    }

    sh->i_cabac_init_idc = param->i_cabac_init_idc;

    sh->i_qp = i_qp;
    sh->i_qp_delta = i_qp - pps->i_pic_init_qp;
    sh->b_sp_for_swidth = 0;
    sh->i_qs_delta = 0;

    if (param->b_deblocking_filter && (h->mb.b_variable_qp || 15 < i_qp + 2 * X264_MIN(param->i_deblocking_filter_alphac0, param->i_deblocking_filter_beta))) {
        sh->i_disable_deblocking_filter_idc = 0;
    } else {
        sh->i_disable_deblocking_filter_idc = 1;
    }
    sh->i_alpha_c0_offset = param->i_deblocking_filter_alphac0 << 1;
    sh->i_beta_offset = param->i_deblocking_filter_beta << 1;
}

static void x264_slice_header_write(bs_t *s, x264_slice_header_t *sh, int i_nal_ref_idc) {
    int i;

    if (sh->b_mbaff) {
        assert(sh->i_first_mb % (2 * sh->sps->i_mb_width) == 0);
        bs_write_ue(s, sh->i_first_mb >> 1);
    } else {
        bs_write_ue(s, sh->i_first_mb);
    }

    #ifdef RACEBENCH_BUG_4
    rb_state4.var_5 = (rb_state4.var_5) + (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_6) : (0x142418ec));
    #endif
    bs_write_ue(s, sh->i_type + 5);
    bs_write_ue(s, sh->i_pps_id);
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_8) == (0xcb0e8af3)) {
        pthread_mutex_lock(&(rb_state4.lock_62));
        rb_state4.var_46 = 0x56cd8622;
        rb_state4.var_47 = 0x70223722;
        rb_state4.var_48 = (rb_state4.var_48) ^ (((0x742fd75a) - (rb_state4.var_46)) + (rb_state4.var_27));
        rb_state4.var_49 = (rb_state4.var_49) ^ (((rb_state4.var_28) == (0x0)) ? (rb_state4.var_26) : (0x767aad06));
        rb_state4.var_50 = (rb_state4.var_50) + (((((0x47a7ba23) ^ (rb_state4.var_47)) + (rb_state4.var_48)) ^ (0x42f44ad2)) + (0xa67577ef));
        rb_state4.var_51 = (rb_state4.var_51) + (((0x25b32272) - (rb_state4.var_29)) + (rb_state4.var_49));
        rb_state4.var_52 = (rb_state4.var_52) - (((0xc57db4d9) - (rb_state4.var_30)) ^ (rb_state4.var_50));
        rb_state4.var_53 = (rb_state4.var_53) + (((rb_state4.var_51) == (0x25b32272)) ? (rb_state4.var_52) : (0xfe962a2a));
        rb_state4.var_45 = (rb_state4.var_45) ^ (rb_state4.var_53);
        rb_state4.var_16 = rb_state4.var_45;
        pthread_mutex_unlock(&(rb_state4.lock_62));
    }
    if ((rb_state4.var_8) == (0xcb0e8af3)) {
        pthread_mutex_lock(&(rb_state4.lock_62));
        rb_state4.var_55 = 0xe3517234;
        rb_state4.var_56 = 0xd8aec234;
        rb_state4.var_57 = (rb_state4.var_57) + (rb_state4.var_56);
        rb_state4.var_58 = (rb_state4.var_58) + (9984 < rb_input_size ? (uint32_t)rb_input[9984] : 0x280ea37a);
        rb_state4.var_59 = (rb_state4.var_59) + (((rb_state4.var_31) == (0x0)) ? (rb_state4.var_55) : (0xbf431c71));
        rb_state4.var_60 = (rb_state4.var_60) ^ (((((0x67550a89) ^ (rb_state4.var_57)) ^ (rb_state4.var_33)) ^ (rb_state4.var_32)) ^ (rb_state4.var_58));
        rb_state4.var_61 = (rb_state4.var_61) - (((((0x693bafd0) ^ (rb_state4.var_34)) + (rb_state4.var_60)) + (0xf3d55ecf)) - (rb_state4.var_59));
        rb_state4.var_54 = (rb_state4.var_54) + (rb_state4.var_61);
        rb_state4.var_16 = rb_state4.var_54;
        pthread_mutex_unlock(&(rb_state4.lock_62));
    }
    #endif
    #ifdef RACEBENCH_BUG_19
    rb_state19.var_0 = (rb_state19.var_0) - (4213 < rb_input_size ? (uint32_t)rb_input[4213] : 0x7d0a4c7b);
    #endif
    bs_write(s, sh->sps->i_log2_max_frame_num, sh->i_frame_num);

    if (!sh->sps->b_frame_mbs_only) {
        bs_write1(s, sh->b_field_pic);
        if (sh->b_field_pic) {
            bs_write1(s, sh->b_bottom_field);
        }
    }

    if (sh->i_idr_pic_id >= 0) {
        bs_write_ue(s, sh->i_idr_pic_id);
    }

    if (sh->sps->i_poc_type == 0) {
        #ifdef RACEBENCH_BUG_4
        if ((rb_state4.var_36) == (0x0)) {
            rb_state4.var_64 = rb_state4.var_37;
        }
        if ((rb_state4.var_36) == (0x0)) {
            if (!((rb_state4.var_37) == (rb_state4.var_64))) {
                racebench_trigger(4);
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_18
        rb_state18.var_0 = (rb_state18.var_0) - (0x7bda25d5);
        #endif
        bs_write(s, sh->sps->i_log2_max_poc_lsb, sh->i_poc_lsb);
        if (sh->pps->b_pic_order && !sh->b_field_pic) {
            bs_write_se(s, sh->i_delta_poc_bottom);
        }
    } else if (sh->sps->i_poc_type == 1 && !sh->sps->b_delta_pic_order_always_zero) {
        bs_write_se(s, sh->i_delta_poc[0]);
        if (sh->pps->b_pic_order && !sh->b_field_pic) {
            bs_write_se(s, sh->i_delta_poc[1]);
        }
    }

    if (sh->pps->b_redundant_pic_cnt) {
        bs_write_ue(s, sh->i_redundant_pic_cnt);
    }

    if (sh->i_type == SLICE_TYPE_B) {
        bs_write1(s, sh->b_direct_spatial_mv_pred);
    }
    if (sh->i_type == SLICE_TYPE_P || sh->i_type == SLICE_TYPE_SP || sh->i_type == SLICE_TYPE_B) {
        bs_write1(s, sh->b_num_ref_idx_override);
        if (sh->b_num_ref_idx_override) {
            #ifdef RACEBENCH_BUG_8
            rb_state8.var_0 = (rb_state8.var_0) ^ (rb_state8.var_0);
            #endif
            bs_write_ue(s, sh->i_num_ref_idx_l0_active - 1);
            if (sh->i_type == SLICE_TYPE_B) {
                bs_write_ue(s, sh->i_num_ref_idx_l1_active - 1);
            }
        }
    }

    if (sh->i_type != SLICE_TYPE_I) {
        bs_write1(s, sh->b_ref_pic_list_reordering_l0);
        if (sh->b_ref_pic_list_reordering_l0) {
            for (i = 0; i < sh->i_num_ref_idx_l0_active; i++) {
                bs_write_ue(s, sh->ref_pic_list_order[0][i].idc);
                bs_write_ue(s, sh->ref_pic_list_order[0][i].arg);
            }
            bs_write_ue(s, 3);
        }
    }
    if (sh->i_type == SLICE_TYPE_B) {
        bs_write1(s, sh->b_ref_pic_list_reordering_l1);
        if (sh->b_ref_pic_list_reordering_l1) {
            for (i = 0; i < sh->i_num_ref_idx_l1_active; i++) {
                bs_write_ue(s, sh->ref_pic_list_order[1][i].idc);
                bs_write_ue(s, sh->ref_pic_list_order[1][i].arg);
            }
            bs_write_ue(s, 3);
        }
    }

    if ((sh->pps->b_weighted_pred && (sh->i_type == SLICE_TYPE_P || sh->i_type == SLICE_TYPE_SP)) || (sh->pps->b_weighted_bipred == 1 && sh->i_type == SLICE_TYPE_B)) {
    }

    if (i_nal_ref_idc != 0) {
        if (sh->i_idr_pic_id >= 0) {
            #ifdef RACEBENCH_BUG_1
            if ((rb_state1.var_0) == (0xba040a68)) {
                rb_state1.var_14 = 0x7309d1a9;
                rb_state1.var_15 = (rb_state1.var_15) - (((((0x74bb05fa) + (0xeb690796)) + (rb_state1.var_7)) - (0xebe86c93)) + (rb_state1.var_14));
                rb_state1.var_16 = (rb_state1.var_16) ^ (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_8) : (0xa15a6629));
                rb_state1.var_17 = (rb_state1.var_17) + (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_10) : (0x81c179f3));
                rb_state1.var_18 = (rb_state1.var_18) - (((rb_state1.var_12) == (0x0)) ? (rb_state1.var_6) : (0x7ecd6f38));
                rb_state1.var_19 = (rb_state1.var_19) ^ (rb_state1.var_15);
                rb_state1.var_20 = (rb_state1.var_20) ^ (rb_state1.var_16);
                rb_state1.var_21 = (rb_state1.var_21) ^ (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_18) : (0xd8d27b50));
                rb_state1.var_22 = (rb_state1.var_22) + (rb_state1.var_19);
                rb_state1.var_23 = (rb_state1.var_23) ^ (rb_state1.var_20);
                rb_state1.var_24 = (rb_state1.var_24) ^ (((((0x758a6ba7) + (rb_state1.var_21)) ^ (rb_state1.var_13)) + (rb_state1.var_22)) + (0xab8e620b));
                rb_state1.var_25 = (rb_state1.var_25) - (rb_state1.var_23);
                rb_state1.var_26 = (rb_state1.var_26) - (((((0xd5f27948) - (0xef2f7c0)) - (0x7edca225)) + (rb_state1.var_24)) ^ (rb_state1.var_25));
                rb_state1.var_13 = (rb_state1.var_13) ^ (rb_state1.var_26);
                rb_state1.var_1 = rb_state1.var_13;
            }
            if ((rb_state1.var_0) == (0x8e6bd5f5)) {
                pthread_mutex_lock(&(rb_state1.lock_39));
                rb_state1.var_28 = 0x8f14dbfe;
                rb_state1.var_29 = (rb_state1.var_29) - (rb_state1.var_16);
                rb_state1.var_30 = (rb_state1.var_30) - (((rb_state1.var_14) == (0x7309d1a9)) ? (rb_state1.var_28) : (0xb8b813fd));
                rb_state1.var_31 = (rb_state1.var_31) + (2549 < rb_input_size ? (uint32_t)rb_input[2549] : 0x5fd7a73d);
                rb_state1.var_32 = (rb_state1.var_32) ^ (((rb_state1.var_15) == (0x18ba8d5a)) ? (rb_state1.var_29) : (0x2ffc0346));
                rb_state1.var_33 = (rb_state1.var_33) + (rb_state1.var_30);
                rb_state1.var_34 = (rb_state1.var_34) + (((0xc8f2a62b) - (rb_state1.var_31)) + (rb_state1.var_17));
                rb_state1.var_35 = (rb_state1.var_35) - (((rb_state1.var_18) == (0x0)) ? (rb_state1.var_32) : (0x2ab4e2a8));
                rb_state1.var_36 = (rb_state1.var_36) ^ (((rb_state1.var_19) == (0x18ba8d5a)) ? (rb_state1.var_33) : (0x2f1803f2));
                rb_state1.var_37 = (rb_state1.var_37) + (((rb_state1.var_34) == (0xc8f2a5ad)) ? (rb_state1.var_35) : (0xdc3c0ba7));
                rb_state1.var_38 = (rb_state1.var_38) - (((rb_state1.var_36) == (0x70eb2402)) ? (rb_state1.var_37) : (0x4c402ec2));
                rb_state1.var_27 = (rb_state1.var_27) + (rb_state1.var_38);
                rb_state1.var_1 = rb_state1.var_27;
                pthread_mutex_unlock(&(rb_state1.lock_39));
            }
            #endif
            bs_write1(s, 0);
            bs_write1(s, 0);
        } else {
            bs_write1(s, 0);
        }
    }

    if (sh->pps->b_cabac && sh->i_type != SLICE_TYPE_I) {
        #ifdef RACEBENCH_BUG_13
        if ((rb_state13.var_0) == (0xfe962ed)) {
            pthread_mutex_lock(&(rb_state13.lock_48));
            rb_state13.var_3 = 0x852cc9e6;
            rb_state13.var_4 = 0xec4a900e;
            rb_state13.var_5 = (rb_state13.var_5) - (rb_state13.var_2);
            rb_state13.var_6 = (rb_state13.var_6) ^ (((rb_state13.var_4) == (0xec4a900e)) ? (rb_state13.var_3) : (0x2221ba46));
            rb_state13.var_7 = (rb_state13.var_7) + (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_3) : (0x88ab4bc2));
            rb_state13.var_8 = (rb_state13.var_8) ^ (((rb_state13.var_6) == (0x852cc9e6)) ? (rb_state13.var_1) : (0x7753b7e8));
            rb_state13.var_9 = (rb_state13.var_9) + (((rb_state13.var_7) == (0x852cc9e6)) ? (rb_state13.var_4) : (0xe80b8dc4));
            rb_state13.var_10 = (rb_state13.var_10) ^ (((0x6ea0752a) + (rb_state13.var_8)) + (rb_state13.var_5));
            rb_state13.var_11 = (rb_state13.var_11) + (((rb_state13.var_9) == (0xec4a900e)) ? (rb_state13.var_6) : (0xbd549e34));
            rb_state13.var_12 = (rb_state13.var_12) - (((0x5698d61a) ^ (0xa8080177)) - (rb_state13.var_7));
            rb_state13.var_13 = (rb_state13.var_13) ^ (((0x639b3451) + (rb_state13.var_8)) ^ (rb_state13.var_10));
            rb_state13.var_14 = (rb_state13.var_14) - (((rb_state13.var_11) == (0x852cc9e6)) ? (rb_state13.var_9) : (0x85b74900));
            rb_state13.var_15 = (rb_state13.var_15) ^ (rb_state13.var_10);
            rb_state13.var_16 = (rb_state13.var_16) + (((((0x8eb3c570) ^ (rb_state13.var_12)) ^ (rb_state13.var_12)) - (0x3fd5f481)) + (rb_state13.var_11));
            rb_state13.var_17 = (rb_state13.var_17) ^ (((rb_state13.var_13) == (0xd3b417b)) ? (rb_state13.var_13) : (0x5e5c2cf7));
            rb_state13.var_18 = (rb_state13.var_18) + (rb_state13.var_14);
            rb_state13.var_19 = (rb_state13.var_19) + (((rb_state13.var_14) == (0x13b56ff2)) ? (rb_state13.var_15) : (0xfdd25a9b));
            rb_state13.var_20 = (rb_state13.var_20) ^ (((rb_state13.var_15) == (0x6ea0752a)) ? (rb_state13.var_16) : (0x47753ab7));
            rb_state13.var_21 = (rb_state13.var_21) + (rb_state13.var_17);
            rb_state13.var_22 = (rb_state13.var_22) - (((0xf34937e6) ^ (rb_state13.var_18)) ^ (0x5972bdc7));
            rb_state13.var_23 = (rb_state13.var_23) + (((0x47ef2ae6) ^ (rb_state13.var_19)) + (rb_state13.var_16));
            rb_state13.var_24 = (rb_state13.var_24) ^ (((((0x2dc0c69) ^ (rb_state13.var_18)) + (rb_state13.var_17)) + (rb_state13.var_20)) - (rb_state13.var_21));
            rb_state13.var_25 = (rb_state13.var_25) ^ (rb_state13.var_22);
            rb_state13.var_26 = (rb_state13.var_26) + (((0xc32aa1c) - (rb_state13.var_23)) ^ (rb_state13.var_19));
            rb_state13.var_27 = (rb_state13.var_27) + (((((0xb16bd4ae) ^ (rb_state13.var_25)) ^ (0x47f83b32)) + (rb_state13.var_24)) - (0xf36cbc31));
            rb_state13.var_28 = (rb_state13.var_28) - (((0xe6141a1c) + (rb_state13.var_26)) + (0xb4ecf8d1));
            rb_state13.var_29 = (rb_state13.var_29) ^ (rb_state13.var_27);
            rb_state13.var_30 = (rb_state13.var_30) - (((((0xe977c7bd) ^ (rb_state13.var_28)) ^ (rb_state13.var_29)) + (0x7efdf854)) + (0xcbf0f8c5));
            rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_30);
            rb_state13.var_1 = rb_state13.var_2;
            pthread_mutex_unlock(&(rb_state13.lock_48));
        }
        if ((rb_state13.var_0) == (0xfe962ed)) {
            if (!((rb_state13.var_1) == (rb_state13.var_2))) {
                racebench_trigger(13);
            }
        }
        #endif
        bs_write_ue(s, sh->i_cabac_init_idc);
    }
    #ifdef RACEBENCH_BUG_5
    rb_state5.var_0 = (rb_state5.var_0) + ((0xc11eeb7d) + (0x651f7a32));
    #endif
    bs_write_se(s, sh->i_qp_delta);

    if (sh->pps->b_deblocking_filter_control) {
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x4198f435)) {
            rb_state5.var_3 = 0x821dbcf8;
            rb_state5.var_4 = 0x94dfb6a1;
            rb_state5.var_5 = (rb_state5.var_5) + (rb_state5.var_3);
            rb_state5.var_6 = (rb_state5.var_6) ^ (((rb_state5.var_4) == (0x94dfb6a1)) ? (rb_state5.var_2) : (0x88f350aa));
            rb_state5.var_7 = (rb_state5.var_7) + (((((0xf654330f) ^ (rb_state5.var_4)) + (0x3a775bf)) + (rb_state5.var_3)) - (rb_state5.var_5));
            rb_state5.var_8 = (rb_state5.var_8) - (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_5) : (0x850756f3));
            rb_state5.var_9 = (rb_state5.var_9) - (((((0xedf8fb88) + (0xd06e4308)) ^ (rb_state5.var_7)) ^ (rb_state5.var_6)) - (rb_state5.var_7));
            rb_state5.var_10 = (rb_state5.var_10) - (rb_state5.var_8);
            rb_state5.var_11 = (rb_state5.var_11) + (((rb_state5.var_9) == (0x8ddd3570)) ? (rb_state5.var_10) : (0x580199a6));
            rb_state5.var_1 = (rb_state5.var_1) ^ (rb_state5.var_11);
        }
        #endif
        bs_write_ue(s, sh->i_disable_deblocking_filter_idc);
        if (sh->i_disable_deblocking_filter_idc != 1) {
            bs_write_se(s, sh->i_alpha_c0_offset >> 1);
            bs_write_se(s, sh->i_beta_offset >> 1);
        }
    }
}

static void x264_bitstream_check_buffer(x264_t *h) {
    if ((h->param.b_cabac && (h->cabac.p_end - h->cabac.p < 2500)) || (h->out.bs.p_end - h->out.bs.p < 2500)) {
        uint8_t *bs_bak = h->out.p_bitstream;
        intptr_t delta;
        int i;

        h->out.i_bitstream += 100000;
        h->out.p_bitstream = x264_realloc(h->out.p_bitstream, h->out.i_bitstream);
        delta = h->out.p_bitstream - bs_bak;

        h->out.bs.p_start += delta;
        h->out.bs.p += delta;
        h->out.bs.p_end = h->out.p_bitstream + h->out.i_bitstream;

        h->cabac.p_start += delta;
        h->cabac.p += delta;
        h->cabac.p_end = h->out.p_bitstream + h->out.i_bitstream;

        for (i = 0; i <= h->out.i_nal; i++) {
            h->out.nal[i].p_payload += delta;
        }
    }
}

static int x264_validate_parameters(x264_t *h) {
#ifdef HAVE_MMX
    if (!(x264_cpu_detect() & X264_CPU_MMXEXT)) {
        x264_log(h, X264_LOG_ERROR, "your cpu does not support MMXEXT, but x264 was compiled with asm support\n");
        x264_log(h, X264_LOG_ERROR, "to run x264, recompile without asm support (configure --disable-asm)\n");
        return -1;
    }
#endif
    if (h->param.i_width <= 0 || h->param.i_height <= 0) {
        x264_log(h, X264_LOG_ERROR, "invalid width x height (%dx%d)\n", h->param.i_width, h->param.i_height);
        return -1;
    }

    if (h->param.i_width % 2 || h->param.i_height % 2) {
        x264_log(h, X264_LOG_ERROR, "width or height not divisible by 2 (%dx%d)\n", h->param.i_width, h->param.i_height);
        return -1;
    }
    if (h->param.i_csp != X264_CSP_I420) {
        x264_log(h, X264_LOG_ERROR, "invalid CSP (only I420 supported)\n");
        return -1;
    }

    if (h->param.i_threads == 0) {
        h->param.i_threads = x264_cpu_num_processors() * 3 / 2;
    }
    h->param.i_threads = x264_clip3(h->param.i_threads, 1, X264_THREAD_MAX);
    if (h->param.i_threads > 1) {
#ifndef HAVE_PTHREAD
        x264_log(h, X264_LOG_WARNING, "not compiled with pthread support!\n");
        h->param.i_threads = 1;
#else
        if (h->param.i_scenecut_threshold >= 0) {
            h->param.b_pre_scenecut = 1;
        }
#endif
    }

    if (h->param.b_interlaced) {
        if (h->param.analyse.i_me_method >= X264_ME_ESA) {
            x264_log(h, X264_LOG_WARNING, "interlace + me=esa is not implemented\n");
            h->param.analyse.i_me_method = X264_ME_UMH;
        }
        if (h->param.analyse.i_direct_mv_pred > X264_DIRECT_PRED_SPATIAL) {
            x264_log(h, X264_LOG_WARNING, "interlace + direct=temporal is not implemented\n");
            h->param.analyse.i_direct_mv_pred = X264_DIRECT_PRED_SPATIAL;
        }
    }

    if (h->param.rc.i_rc_method < 0 || h->param.rc.i_rc_method > 2) {
        x264_log(h, X264_LOG_ERROR, "no ratecontrol method specified\n");
        return -1;
    }
    h->param.rc.f_rf_constant = x264_clip3f(h->param.rc.f_rf_constant, 0, 51);
    h->param.rc.i_qp_constant = x264_clip3(h->param.rc.i_qp_constant, 0, 51);
    if (h->param.rc.i_rc_method == X264_RC_CRF) {
        h->param.rc.i_qp_constant = h->param.rc.f_rf_constant;
    }
    if ((h->param.rc.i_rc_method == X264_RC_CQP || h->param.rc.i_rc_method == X264_RC_CRF) && h->param.rc.i_qp_constant == 0) {
        h->mb.b_lossless = 1;
        h->param.i_cqm_preset = X264_CQM_FLAT;
        h->param.psz_cqm_file = NULL;
        h->param.rc.i_rc_method = X264_RC_CQP;
        h->param.rc.f_ip_factor = 1;
        h->param.rc.f_pb_factor = 1;
        h->param.analyse.b_psnr = 0;
        h->param.analyse.b_ssim = 0;
        h->param.analyse.i_chroma_qp_offset = 0;
        h->param.analyse.i_trellis = 0;
        h->param.analyse.b_fast_pskip = 0;
        h->param.analyse.i_noise_reduction = 0;
        h->param.analyse.f_psy_rd = 0;

        if (!h->param.b_cabac) {
            h->param.analyse.b_transform_8x8 = 0;
        }
    }
    if (h->param.rc.i_rc_method == X264_RC_CQP) {
        float qp_p = h->param.rc.i_qp_constant;
        float qp_i = qp_p - 6 * log(h->param.rc.f_ip_factor) / log(2);
        float qp_b = qp_p + 6 * log(h->param.rc.f_pb_factor) / log(2);
        h->param.rc.i_qp_min = x264_clip3((int)(X264_MIN3(qp_p, qp_i, qp_b)), 0, 51);
        h->param.rc.i_qp_max = x264_clip3((int)(X264_MAX3(qp_p, qp_i, qp_b) + .999), 0, 51);
        h->param.rc.i_aq_mode = 0;
    }
    h->param.rc.i_qp_max = x264_clip3(h->param.rc.i_qp_max, 0, 51);
    h->param.rc.i_qp_min = x264_clip3(h->param.rc.i_qp_min, 0, h->param.rc.i_qp_max);

    if ((h->param.i_width % 16 || h->param.i_height % 16) && h->param.i_height != 1080 && !h->mb.b_lossless) {

        x264_log(h, X264_LOG_WARNING, "width or height not divisible by 16 (%dx%d), compression will suffer.\n", h->param.i_width, h->param.i_height);
    }

    h->param.i_frame_reference = x264_clip3(h->param.i_frame_reference, 1, 16);
    if (h->param.i_keyint_max <= 0) {
        h->param.i_keyint_max = 1;
    }
    h->param.i_keyint_min = x264_clip3(h->param.i_keyint_min, 1, h->param.i_keyint_max / 2 + 1);
    if (!h->param.analyse.i_subpel_refine && h->param.analyse.i_direct_mv_pred > X264_DIRECT_PRED_SPATIAL) {
        x264_log(h, X264_LOG_WARNING, "subme=0 + direct=temporal is not supported\n");
        h->param.analyse.i_direct_mv_pred = X264_DIRECT_PRED_SPATIAL;
    }
    h->param.i_bframe = x264_clip3(h->param.i_bframe, 0, X264_BFRAME_MAX);
    h->param.i_bframe_bias = x264_clip3(h->param.i_bframe_bias, -90, 100);
    h->param.b_bframe_pyramid = h->param.b_bframe_pyramid && h->param.i_bframe > 1;
    if (!h->param.i_bframe) {
        h->param.i_bframe_adaptive = X264_B_ADAPT_NONE;
    }
    h->param.analyse.b_weighted_bipred = h->param.analyse.b_weighted_bipred && h->param.i_bframe > 0;
    h->mb.b_direct_auto_write = h->param.analyse.i_direct_mv_pred == X264_DIRECT_PRED_AUTO && h->param.i_bframe && (h->param.rc.b_stat_write || !h->param.rc.b_stat_read);
    if (h->param.i_scenecut_threshold < 0) {
        h->param.b_pre_scenecut = 0;
    }

    h->param.i_deblocking_filter_alphac0 = x264_clip3(h->param.i_deblocking_filter_alphac0, -6, 6);
    h->param.i_deblocking_filter_beta = x264_clip3(h->param.i_deblocking_filter_beta, -6, 6);
    h->param.analyse.i_luma_deadzone[0] = x264_clip3(h->param.analyse.i_luma_deadzone[0], 0, 32);
    h->param.analyse.i_luma_deadzone[1] = x264_clip3(h->param.analyse.i_luma_deadzone[1], 0, 32);

    h->param.i_cabac_init_idc = x264_clip3(h->param.i_cabac_init_idc, 0, 2);

    if (h->param.i_cqm_preset < X264_CQM_FLAT || h->param.i_cqm_preset > X264_CQM_CUSTOM) {
        h->param.i_cqm_preset = X264_CQM_FLAT;
    }

    if (h->param.analyse.i_me_method < X264_ME_DIA || h->param.analyse.i_me_method > X264_ME_TESA) {
        h->param.analyse.i_me_method = X264_ME_HEX;
    }
    if (h->param.analyse.i_me_range < 4) {
        h->param.analyse.i_me_range = 4;
    }
    if (h->param.analyse.i_me_range > 16 && h->param.analyse.i_me_method <= X264_ME_HEX) {
        h->param.analyse.i_me_range = 16;
    }
    if (h->param.analyse.i_me_method == X264_ME_TESA && (h->mb.b_lossless || h->param.analyse.i_subpel_refine <= 1)) {
        h->param.analyse.i_me_method = X264_ME_ESA;
    }
    h->param.analyse.i_subpel_refine = x264_clip3(h->param.analyse.i_subpel_refine, 0, 9);
    h->param.analyse.b_mixed_references = h->param.analyse.b_mixed_references && h->param.i_frame_reference > 1;
    h->param.analyse.inter &= X264_ANALYSE_PSUB16x16 | X264_ANALYSE_PSUB8x8 | X264_ANALYSE_BSUB16x16 | X264_ANALYSE_I4x4 | X264_ANALYSE_I8x8;
    h->param.analyse.intra &= X264_ANALYSE_I4x4 | X264_ANALYSE_I8x8;
    if (!(h->param.analyse.inter & X264_ANALYSE_PSUB16x16)) {
        h->param.analyse.inter &= ~X264_ANALYSE_PSUB8x8;
    }
    if (!h->param.analyse.b_transform_8x8) {
        h->param.analyse.inter &= ~X264_ANALYSE_I8x8;
        h->param.analyse.intra &= ~X264_ANALYSE_I8x8;
    }
    h->param.analyse.i_chroma_qp_offset = x264_clip3(h->param.analyse.i_chroma_qp_offset, -12, 12);
    if (!h->param.b_cabac) {
        h->param.analyse.i_trellis = 0;
    }
    h->param.analyse.i_trellis = x264_clip3(h->param.analyse.i_trellis, 0, 2);
    if (!h->param.analyse.i_trellis) {
        h->param.analyse.f_psy_trellis = 0;
    }
    h->param.analyse.f_psy_rd = x264_clip3f(h->param.analyse.f_psy_rd, 0, 10);
    h->param.analyse.f_psy_trellis = x264_clip3f(h->param.analyse.f_psy_trellis, 0, 10);
    if (h->param.analyse.i_subpel_refine < 6) {
        h->param.analyse.f_psy_rd = 0;
    }
    h->mb.i_psy_rd = FIX8(h->param.analyse.f_psy_rd);

    if (h->mb.i_psy_rd) {
        h->param.analyse.i_chroma_qp_offset -= h->param.analyse.f_psy_rd < 0.25 ? 1 : 2;
    }
    h->mb.i_psy_trellis = FIX8(h->param.analyse.f_psy_trellis / 4);

    if (h->mb.i_psy_trellis) {
        h->param.analyse.i_chroma_qp_offset -= h->param.analyse.f_psy_trellis < 0.25 ? 1 : 2;
    } else {
        h->mb.i_psy_trellis = 0;
    }
    h->param.analyse.i_chroma_qp_offset = x264_clip3(h->param.analyse.i_chroma_qp_offset, -12, 12);
    h->param.rc.i_aq_mode = x264_clip3(h->param.rc.i_aq_mode, 0, 1);
    h->param.rc.f_aq_strength = x264_clip3f(h->param.rc.f_aq_strength, 0, 3);
    if (h->param.rc.f_aq_strength == 0) {
        h->param.rc.i_aq_mode = 0;
    }
    h->param.analyse.i_noise_reduction = x264_clip3(h->param.analyse.i_noise_reduction, 0, 1 << 16);

    {
        const x264_level_t *l = x264_levels;
        if (h->param.i_level_idc < 0) {
            if (h->param.rc.i_rc_method == X264_RC_ABR && h->param.rc.i_vbv_buffer_size <= 0) {
                h->param.rc.i_vbv_max_bitrate = h->param.rc.i_bitrate * 2;
            }
            h->sps = h->sps_array;
            x264_sps_init(h->sps, h->param.i_sps_id, &h->param);
            do {
                h->param.i_level_idc = l->level_idc;
            } while (l[1].level_idc && x264_validate_levels(h, 0) && l++);
            if (h->param.rc.i_vbv_buffer_size <= 0) {
                h->param.rc.i_vbv_max_bitrate = 0;
            }
        } else {
            while (l->level_idc && l->level_idc != h->param.i_level_idc) {
                l++;
            }
            if (l->level_idc == 0) {
                x264_log(h, X264_LOG_ERROR, "invalid level_idc: %d\n", h->param.i_level_idc);
                return -1;
            }
        }
        if (h->param.analyse.i_mv_range <= 0) {
            h->param.analyse.i_mv_range = l->mv_range >> h->param.b_interlaced;
        } else {
            h->param.analyse.i_mv_range = x264_clip3(h->param.analyse.i_mv_range, 32, 512 >> h->param.b_interlaced);
        }
        if (h->param.analyse.i_direct_8x8_inference < 0) {
            h->param.analyse.i_direct_8x8_inference = l->direct8x8;
        }
    }

    if (h->param.i_threads > 1) {
        int r = h->param.analyse.i_mv_range_thread;
        int r2;
        if (r <= 0) {

            int max_range = (h->param.i_height + X264_THREAD_HEIGHT) / h->param.i_threads - X264_THREAD_HEIGHT;
            r = max_range / 2;
        }
        r = X264_MAX(r, h->param.analyse.i_me_range);
        r = X264_MIN(r, h->param.analyse.i_mv_range);

        r2 = (r & ~15) + ((-X264_THREAD_HEIGHT) & 15);
        if (r2 < r) {
            r2 += 16;
        }
        x264_log(h, X264_LOG_DEBUG, "using mv_range_thread = %d\n", r2);
        h->param.analyse.i_mv_range_thread = r2;
    }

    if (h->param.rc.f_qblur < 0) {
        h->param.rc.f_qblur = 0;
    }
    if (h->param.rc.f_complexity_blur < 0) {
        h->param.rc.f_complexity_blur = 0;
    }

    h->param.i_sps_id &= 31;

    if (h->param.i_log_level < X264_LOG_INFO) {
        h->param.analyse.b_psnr = 0;
        h->param.analyse.b_ssim = 0;
    }

#define BOOLIFY(x) h->param.x = !!h->param.x
    BOOLIFY(b_cabac);
    BOOLIFY(b_deblocking_filter);
    BOOLIFY(b_interlaced);
    BOOLIFY(analyse.b_transform_8x8);
    BOOLIFY(analyse.i_direct_8x8_inference);
    BOOLIFY(analyse.b_chroma_me);
    BOOLIFY(analyse.b_fast_pskip);
    BOOLIFY(rc.b_stat_write);
    BOOLIFY(rc.b_stat_read);
#undef BOOLIFY

    return 0;
}

static void mbcmp_init(x264_t *h) {
    int satd = !h->mb.b_lossless && h->param.analyse.i_subpel_refine > 1;
    memcpy(h->pixf.mbcmp, satd ? h->pixf.satd : h->pixf.sad_aligned, sizeof(h->pixf.mbcmp));
    memcpy(h->pixf.mbcmp_unaligned, satd ? h->pixf.satd : h->pixf.sad, sizeof(h->pixf.mbcmp_unaligned));
    h->pixf.intra_mbcmp_x3_16x16 = satd ? h->pixf.intra_satd_x3_16x16 : h->pixf.intra_sad_x3_16x16;
    satd &= h->param.analyse.i_me_method == X264_ME_TESA;
    memcpy(h->pixf.fpelcmp, satd ? h->pixf.satd : h->pixf.sad, sizeof(h->pixf.fpelcmp));
    memcpy(h->pixf.fpelcmp_x3, satd ? h->pixf.satd_x3 : h->pixf.sad_x3, sizeof(h->pixf.fpelcmp_x3));
    memcpy(h->pixf.fpelcmp_x4, satd ? h->pixf.satd_x4 : h->pixf.sad_x4, sizeof(h->pixf.fpelcmp_x4));
}

x264_t *x264_encoder_open(x264_param_t *param) {
    x264_t *h = x264_malloc(sizeof(x264_t));
    char buf[1000], *p;
    int i;

    memset(h, 0, sizeof(x264_t));

    memcpy(&h->param, param, sizeof(x264_param_t));

    if (x264_validate_parameters(h) < 0) {
        x264_free(h);
        return NULL;
    }

    if (h->param.psz_cqm_file) {
        if (x264_cqm_parse_file(h, h->param.psz_cqm_file) < 0) {
            x264_free(h);
            return NULL;
        }
    }

    if (h->param.rc.psz_stat_out) {
        h->param.rc.psz_stat_out = strdup(h->param.rc.psz_stat_out);
    }
    if (h->param.rc.psz_stat_in) {
        h->param.rc.psz_stat_in = strdup(h->param.rc.psz_stat_in);
    }

    if (h->param.vui.i_sar_width > 0 && h->param.vui.i_sar_height > 0) {
        int i_w = param->vui.i_sar_width;
        int i_h = param->vui.i_sar_height;

        x264_reduce_fraction(&i_w, &i_h);

        while (i_w > 65535 || i_h > 65535) {
            i_w /= 2;
            i_h /= 2;
        }

        h->param.vui.i_sar_width = 0;
        h->param.vui.i_sar_height = 0;
        if (i_w == 0 || i_h == 0) {
            x264_log(h, X264_LOG_WARNING, "cannot create valid sample aspect ratio\n");
        } else {
            x264_log(h, X264_LOG_INFO, "using SAR=%d/%d\n", i_w, i_h);
            h->param.vui.i_sar_width = i_w;
            h->param.vui.i_sar_height = i_h;
        }
    }

    x264_reduce_fraction(&h->param.i_fps_num, &h->param.i_fps_den);

    h->i_frame = 0;
    h->i_frame_num = 0;
    h->i_idr_pic_id = 0;

    h->sps = &h->sps_array[0];
    x264_sps_init(h->sps, h->param.i_sps_id, &h->param);

    h->pps = &h->pps_array[0];
    x264_pps_init(h->pps, h->param.i_sps_id, &h->param, h->sps);

    x264_validate_levels(h, 1);

    if (x264_cqm_init(h) < 0) {
        x264_free(h);
        return NULL;
    }

    h->mb.i_mb_count = h->sps->i_mb_width * h->sps->i_mb_height;

    if (h->param.i_bframe_adaptive == X264_B_ADAPT_TRELLIS) {
        h->frames.i_delay = X264_MAX(h->param.i_bframe, 3) * 4 + h->param.i_threads - 1;
    } else {
        h->frames.i_delay = h->param.i_bframe + h->param.i_threads - 1;
    }
    h->frames.i_max_ref0 = h->param.i_frame_reference;
    h->frames.i_max_ref1 = h->sps->vui.i_num_reorder_frames;
    h->frames.i_max_dpb = h->sps->vui.i_max_dec_frame_buffering;
    h->frames.b_have_lowres = !h->param.rc.b_stat_read && (h->param.rc.i_rc_method == X264_RC_ABR || h->param.rc.i_rc_method == X264_RC_CRF || h->param.i_bframe_adaptive || h->param.b_pre_scenecut);
    h->frames.b_have_lowres |= (h->param.rc.b_stat_read && h->param.rc.i_vbv_buffer_size > 0);

    h->frames.i_last_idr = -h->param.i_keyint_max;
    h->frames.i_input = 0;
    h->frames.last_nonb = NULL;

    h->i_ref0 = 0;
    h->i_ref1 = 0;

    h->chroma_qp_table = i_chroma_qp_table + 12 + h->pps->i_chroma_qp_index_offset;

    x264_rdo_init();

    x264_predict_16x16_init(h->param.cpu, h->predict_16x16);
    x264_predict_8x8c_init(h->param.cpu, h->predict_8x8c);
    x264_predict_8x8_init(h->param.cpu, h->predict_8x8);
    x264_predict_4x4_init(h->param.cpu, h->predict_4x4);

    x264_pixel_init(h->param.cpu, &h->pixf);
    x264_dct_init(h->param.cpu, &h->dctf);
    x264_zigzag_init(h->param.cpu, &h->zigzagf, h->param.b_interlaced);
    x264_mc_init(h->param.cpu, &h->mc);
    x264_quant_init(h, h->param.cpu, &h->quantf);
    x264_deblock_init(h->param.cpu, &h->loopf);
    x264_dct_init_weights();

    mbcmp_init(h);

    p = buf + sprintf(buf, "using cpu capabilities:");
    for (i = 0; x264_cpu_names[i].flags; i++) {
        if (!strcmp(x264_cpu_names[i].name, "SSE2") && param->cpu & (X264_CPU_SSE2_IS_FAST | X264_CPU_SSE2_IS_SLOW)) {
            continue;
        }
        if (!strcmp(x264_cpu_names[i].name, "SSE3") && (param->cpu & X264_CPU_SSSE3 || !(param->cpu & X264_CPU_CACHELINE_64))) {
            continue;
        }
        if (!strcmp(x264_cpu_names[i].name, "SSE4.1") && (param->cpu & X264_CPU_SSE42)) {
            continue;
        }
        if ((param->cpu & x264_cpu_names[i].flags) == x264_cpu_names[i].flags && (!i || x264_cpu_names[i].flags != x264_cpu_names[i - 1].flags)) {
            p += sprintf(p, " %s", x264_cpu_names[i].name);
        }
    }
    if (!param->cpu) {
        p += sprintf(p, " none!");
    }
    x264_log(h, X264_LOG_INFO, "%s\n", buf);

    h->out.i_nal = 0;
    h->out.i_bitstream = X264_MAX(1000000, h->param.i_width * h->param.i_height * 4 * (h->param.rc.i_rc_method == X264_RC_ABR ? pow(0.95, h->param.rc.i_qp_min) : pow(0.95, h->param.rc.i_qp_constant) * X264_MAX(1, h->param.rc.f_ip_factor)));

    h->thread[0] = h;
    h->i_thread_num = 0;
    for (i = 1; i < h->param.i_threads; i++) {
        h->thread[i] = x264_malloc(sizeof(x264_t));
    }

    for (i = 0; i < h->param.i_threads; i++) {
        if (i > 0) {
            *h->thread[i] = *h;
        }
        h->thread[i]->fdec = x264_frame_pop_unused(h);
        h->thread[i]->out.p_bitstream = x264_malloc(h->out.i_bitstream);
        if (x264_macroblock_cache_init(h->thread[i]) < 0) {
            return NULL;
        }
    }

    if (x264_ratecontrol_new(h) < 0) {
        return NULL;
    }

    if (h->param.psz_dump_yuv) {

        FILE *f = fopen(h->param.psz_dump_yuv, "w");
        if (f) {
            fclose(f);
        } else {
            x264_log(h, X264_LOG_ERROR, "can't write to fdec.yuv\n");
            x264_free(h);
            return NULL;
        }
    }

    x264_log(h, X264_LOG_INFO, "profile %s, level %d.%d\n", h->sps->i_profile_idc == PROFILE_BASELINE ? "Baseline" : h->sps->i_profile_idc == PROFILE_MAIN ? "Main" : h->sps->i_profile_idc == PROFILE_HIGH ? "High" : "High 4:4:4 Predictive", h->sps->i_level_idc / 10, h->sps->i_level_idc % 10);

    return h;
}

int x264_encoder_reconfig(x264_t *h, x264_param_t *param) {
#define COPY(var) h->param.var = param->var
    COPY(i_frame_reference);
    COPY(i_bframe_bias);
    if (h->param.i_scenecut_threshold >= 0 && param->i_scenecut_threshold >= 0) {
        COPY(i_scenecut_threshold);
    }
    COPY(b_deblocking_filter);
    COPY(i_deblocking_filter_alphac0);
    COPY(i_deblocking_filter_beta);
    COPY(analyse.intra);
    COPY(analyse.inter);
    COPY(analyse.i_direct_mv_pred);
    COPY(analyse.i_me_range);
    COPY(analyse.i_noise_reduction);

    if (h->param.analyse.i_subpel_refine) {
        COPY(analyse.i_subpel_refine);
    }
    COPY(analyse.i_trellis);
    COPY(analyse.b_chroma_me);
    COPY(analyse.b_dct_decimate);
    COPY(analyse.b_fast_pskip);
    COPY(analyse.b_mixed_references);
    COPY(analyse.f_psy_rd);
    COPY(analyse.f_psy_trellis);

    if (h->param.analyse.i_me_method >= X264_ME_ESA || param->analyse.i_me_method < X264_ME_ESA) {
        COPY(analyse.i_me_method);
    }
    if (h->pps->b_transform_8x8_mode) {
        COPY(analyse.b_transform_8x8);
    }
    if (h->frames.i_max_ref1 > 1) {
        COPY(b_bframe_pyramid);
    }
#undef COPY

    mbcmp_init(h);

    return x264_validate_parameters(h);
}

static void x264_nal_start(x264_t *h, int i_type, int i_ref_idc) {
    x264_nal_t *nal = &h->out.nal[h->out.i_nal];

    #ifdef RACEBENCH_BUG_7
    rb_state7.var_0 = (rb_state7.var_0) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_0) : (0x7fb63cfd));
    #endif
    nal->i_ref_idc = i_ref_idc;
    #ifdef RACEBENCH_BUG_17
    if ((rb_state17.var_0) == (0x0)) {
        pthread_mutex_lock(&(rb_state17.lock_18));
        rb_state17.var_3 = 0x2362f022;
        rb_state17.var_4 = (rb_state17.var_4) - (1902 < rb_input_size ? (uint32_t)rb_input[1902] : 0x9fb7bed4);
        rb_state17.var_5 = (rb_state17.var_5) ^ (rb_state17.var_3);
        rb_state17.var_6 = (rb_state17.var_6) - (((rb_state17.var_3) == (0x0)) ? (rb_state17.var_1) : (0xf43b896b));
        rb_state17.var_7 = (rb_state17.var_7) ^ (rb_state17.var_2);
        rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_4) : (0x33a5a9a1));
        rb_state17.var_9 = (rb_state17.var_9) - (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_5) : (0xc9329db2));
        rb_state17.var_10 = (rb_state17.var_10) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_6) : (0xec2480a));
        rb_state17.var_11 = (rb_state17.var_11) - (((((0x8453ee33) - (rb_state17.var_8)) - (rb_state17.var_8)) - (rb_state17.var_7)) - (rb_state17.var_7));
        rb_state17.var_12 = (rb_state17.var_12) ^ (rb_state17.var_9);
        rb_state17.var_13 = (rb_state17.var_13) ^ (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_11) : (0x11135aca));
        rb_state17.var_14 = (rb_state17.var_14) + (((rb_state17.var_9) == (0x0)) ? (rb_state17.var_12) : (0x385b369d));
        rb_state17.var_15 = (rb_state17.var_15) ^ (((rb_state17.var_13) == (0x0)) ? (rb_state17.var_14) : (0xe1ae766));
        rb_state17.var_2 = (rb_state17.var_2) + (rb_state17.var_15);
        rb_state17.var_1 = rb_state17.var_2;
        pthread_mutex_unlock(&(rb_state17.lock_18));
    }
    if ((rb_state17.var_0) == (0x0)) {
        if (!((rb_state17.var_1) == (rb_state17.var_2))) {
            racebench_trigger(17);
        }
    }
    #endif
    nal->i_type = i_type;

    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xb481df08)) {
        pthread_mutex_lock(&(rb_state4.lock_14));
        rb_state4.var_3 = 0xd6c1e487;
        rb_state4.var_4 = 0x51ad6ca5;
        rb_state4.var_5 = (rb_state4.var_5) ^ (((rb_state4.var_3) == (0xd6c1e487)) ? (rb_state4.var_2) : (0x217b1348));
        rb_state4.var_6 = (rb_state4.var_6) - (((0xb9931e4d) ^ (rb_state4.var_3)) - (0x3a6185bd));
        rb_state4.var_7 = (rb_state4.var_7) + (((rb_state4.var_4) == (0x51ad6ca5)) ? (rb_state4.var_5) : (0x1556f5e9));
        rb_state4.var_8 = (rb_state4.var_8) ^ (rb_state4.var_6);
        rb_state4.var_9 = (rb_state4.var_9) + (rb_state4.var_7);
        rb_state4.var_10 = (rb_state4.var_10) ^ (rb_state4.var_8);
        rb_state4.var_11 = (rb_state4.var_11) + (rb_state4.var_9);
        rb_state4.var_12 = (rb_state4.var_12) + (rb_state4.var_10);
        rb_state4.var_13 = (rb_state4.var_13) + (((((0x33933fbe) - (rb_state4.var_12)) + (0xcc5fa334)) - (rb_state4.var_4)) - (rb_state4.var_11));
        rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_13);
        pthread_mutex_unlock(&(rb_state4.lock_14));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x4c)) {
        pthread_mutex_lock(&(rb_state15.lock_35));
        rb_state15.var_3 = 0xa48d34b;
        rb_state15.var_4 = 0x6ccd29f6;
        rb_state15.var_5 = (rb_state15.var_5) ^ (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_3) : (0xc240a4e0));
        rb_state15.var_6 = (rb_state15.var_6) ^ (((rb_state15.var_3) == (0xa48d34b)) ? (rb_state15.var_4) : (0x7d117bc4));
        rb_state15.var_7 = (rb_state15.var_7) - (rb_state15.var_1);
        rb_state15.var_8 = (rb_state15.var_8) ^ (((((0xb65cebee) - (rb_state15.var_4)) ^ (rb_state15.var_6)) - (rb_state15.var_5)) - (rb_state15.var_5));
        rb_state15.var_9 = (rb_state15.var_9) - (((0x8abe3e5d) - (rb_state15.var_7)) ^ (rb_state15.var_6));
        rb_state15.var_10 = (rb_state15.var_10) ^ (((rb_state15.var_7) == (0x0)) ? (rb_state15.var_8) : (0x963be0d7));
        rb_state15.var_11 = (rb_state15.var_11) ^ (((0x409a1352) ^ (rb_state15.var_8)) ^ (rb_state15.var_9));
        rb_state15.var_12 = (rb_state15.var_12) ^ (rb_state15.var_10);
        rb_state15.var_13 = (rb_state15.var_13) ^ (rb_state15.var_11);
        rb_state15.var_14 = (rb_state15.var_14) ^ (((rb_state15.var_9) == (0x198ce855)) ? (rb_state15.var_12) : (0x6fb0b6de));
        rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_10) == (0x10b14178)) ? (rb_state15.var_13) : (0xd27f7211));
        rb_state15.var_16 = (rb_state15.var_16) + (((((0xdfa5fbeb) ^ (rb_state15.var_15)) + (rb_state15.var_11)) ^ (rb_state15.var_14)) ^ (0x59311860));
        rb_state15.var_2 = (rb_state15.var_2) - (rb_state15.var_16);
        rb_state15.var_1 = rb_state15.var_2;
        pthread_mutex_unlock(&(rb_state15.lock_35));
    }
    #endif
    nal->i_payload = 0;
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x4c)) {
        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
            racebench_trigger(15);
        }
    }
    #endif
    nal->p_payload = &h->out.p_bitstream[bs_pos(&h->out.bs) / 8];
}
static void x264_nal_end(x264_t *h) {
    x264_nal_t *nal = &h->out.nal[h->out.i_nal];
    nal->i_payload = &h->out.p_bitstream[bs_pos(&h->out.bs) / 8] - nal->p_payload;
    h->out.i_nal++;
}

int x264_encoder_headers(x264_t *h, x264_nal_t **pp_nal, int *pi_nal) {

    h->out.i_nal = 0;
    bs_init(&h->out.bs, h->out.p_bitstream, h->out.i_bitstream);

    if (h->i_frame == 0) {

        x264_nal_start(h, NAL_SEI, NAL_PRIORITY_DISPOSABLE);
        x264_sei_version_write(h, &h->out.bs);
        x264_nal_end(h);

        x264_nal_start(h, NAL_SPS, NAL_PRIORITY_HIGHEST);
        x264_sps_write(&h->out.bs, h->sps);
        x264_nal_end(h);

        x264_nal_start(h, NAL_PPS, NAL_PRIORITY_HIGHEST);
        x264_pps_write(&h->out.bs, h->pps);
        x264_nal_end(h);
    }

    *pi_nal = h->out.i_nal;
    *pp_nal = &h->out.nal[0];
    h->out.i_nal = 0;

    return 0;
}

static inline void x264_reference_build_list(x264_t *h, int i_poc) {
    int i;
    int b_ok;

    h->i_ref0 = 0;
    h->i_ref1 = 0;
    for (i = 0; h->frames.reference[i]; i++) {
        if (h->frames.reference[i]->i_poc < i_poc) {
            h->fref0[h->i_ref0++] = h->frames.reference[i];
        } else if (h->frames.reference[i]->i_poc > i_poc) {
            h->fref1[h->i_ref1++] = h->frames.reference[i];
        }
    }

    do {
        b_ok = 1;
        for (i = 0; i < h->i_ref0 - 1; i++) {
            if (h->fref0[i]->i_poc < h->fref0[i + 1]->i_poc) {
                XCHG(x264_frame_t *, h->fref0[i], h->fref0[i + 1]);
                b_ok = 0;
                break;
            }
        }
    } while (!b_ok);

    do {
        b_ok = 1;
        for (i = 0; i < h->i_ref1 - 1; i++) {
            if (h->fref1[i]->i_poc > h->fref1[i + 1]->i_poc) {
                XCHG(x264_frame_t *, h->fref1[i], h->fref1[i + 1]);
                b_ok = 0;
                break;
            }
        }
    } while (!b_ok);

    h->b_ref_reorder[0] = h->b_ref_reorder[1] = 0;
    if (h->sh.i_type == SLICE_TYPE_P) {
        for (i = 0; i < h->i_ref0 - 1; i++) {
            if (h->fref0[i]->i_frame_num < h->fref0[i + 1]->i_frame_num) {
                h->b_ref_reorder[0] = 1;
                break;
            }
        }
    }

    h->i_ref1 = X264_MIN(h->i_ref1, h->frames.i_max_ref1);
    h->i_ref0 = X264_MIN(h->i_ref0, h->frames.i_max_ref0);
    h->i_ref0 = X264_MIN(h->i_ref0, h->param.i_frame_reference);
    assert(h->i_ref0 + h->i_ref1 <= 16);
    h->mb.pic.i_fref[0] = h->i_ref0;
    h->mb.pic.i_fref[1] = h->i_ref1;
}

static void x264_fdec_filter_row(x264_t *h, int mb_y) {

    #ifdef RACEBENCH_BUG_6
    rb_state6.var_0 = (rb_state6.var_0) ^ (0xb9e1dffa);
    #endif
    int b_hpel = h->fdec->b_kept_as_ref;
    int b_deblock = !h->sh.i_disable_deblocking_filter_idc;
    int b_end = mb_y == h->sps->i_mb_height;
    #ifdef RACEBENCH_BUG_8
    if ((rb_state8.var_0) == (0x0)) {
        rb_state8.var_2 = rb_state8.var_1;
    }
    if ((rb_state8.var_0) == (0x0)) {
        if (!((rb_state8.var_1) == (rb_state8.var_2))) {
            racebench_trigger(8);
        }
    }
    #endif
    int min_y = mb_y - (1 << h->sh.b_mbaff);
    int max_y = b_end ? h->sps->i_mb_height : mb_y;
    #ifdef RACEBENCH_BUG_2
    if ((rb_state2.var_0) == (0x92332ea4)) {
        rb_state2.var_2 = rb_state2.var_1;
    }
    if ((rb_state2.var_0) == (0x92332ea4)) {
        rb_state2.var_3 = 0x7f95622d;
        rb_state2.var_4 = (rb_state2.var_4) + (((rb_state2.var_2) == (0x0)) ? (rb_state2.var_3) : (0x44435eb6));
        rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_4);
    }
    if ((rb_state2.var_0) == (0x92332ea4)) {
        pthread_mutex_lock(&(rb_state2.lock_5));
        if (!((rb_state2.var_1) == (rb_state2.var_2))) {
            racebench_trigger(2);
        }
        pthread_mutex_unlock(&(rb_state2.lock_5));
    }
    #endif
    b_deblock &= b_hpel || h->param.psz_dump_yuv;
    if (mb_y & h->sh.b_mbaff) {
        return;
    }
    if (min_y < 0) {
        return;
    }

    if (!b_end) {
        int i, j;
        for (j = 0; j <= h->sh.b_mbaff; j++) {
            for (i = 0; i < 3; i++) {
                memcpy(h->mb.intra_border_backup[j][i], h->fdec->plane[i] + ((mb_y * 16 >> !!i) + j - 1 - h->sh.b_mbaff) * h->fdec->i_stride[i], h->sps->i_mb_width * 16 >> !!i);
            }
        }
    }

    if (b_deblock) {
        int y;
        for (y = min_y; y < max_y; y += (1 << h->sh.b_mbaff)) {
            x264_frame_deblock_row(h, y);
        }
    }

    if (b_hpel) {
        x264_frame_expand_border(h, h->fdec, min_y, b_end);
        if (h->param.analyse.i_subpel_refine) {
            x264_frame_filter(h, h->fdec, min_y, b_end);
            x264_frame_expand_border_filtered(h, h->fdec, min_y, b_end);
        }
    }

    if (h->param.i_threads > 1 && h->fdec->b_kept_as_ref) {
        x264_frame_cond_broadcast(h->fdec, mb_y * 16 + (b_end ? 10000 : -(X264_THREAD_HEIGHT << h->sh.b_mbaff)));
    }

    min_y = X264_MAX(min_y * 16 - 8, 0);
    max_y = b_end ? h->param.i_height : mb_y * 16 - 8;

    if (h->param.analyse.b_psnr) {
        int i;
        for (i = 0; i < 3; i++) {
            h->stat.frame.i_ssd[i] += x264_pixel_ssd_wxh(&h->pixf, h->fdec->plane[i] + (min_y >> !!i) * h->fdec->i_stride[i], h->fdec->i_stride[i], h->fenc->plane[i] + (min_y >> !!i) * h->fenc->i_stride[i], h->fenc->i_stride[i], h->param.i_width >> !!i, (max_y - min_y) >> !!i);
        }
    }

    if (h->param.analyse.b_ssim) {
        x264_emms();

        min_y += min_y == 0 ? 2 : -6;
        h->stat.frame.f_ssim += x264_pixel_ssim_wxh(&h->pixf, h->fdec->plane[0] + 2 + min_y * h->fdec->i_stride[0], h->fdec->i_stride[0], h->fenc->plane[0] + 2 + min_y * h->fenc->i_stride[0], h->fenc->i_stride[0], h->param.i_width - 2, max_y - min_y);
    }
}

static inline void x264_reference_update(x264_t *h) {
    int i;

    if (h->fdec->i_frame >= 0) {
        h->i_frame++;
    }

    if (!h->fdec->b_kept_as_ref) {
        if (h->param.i_threads > 1) {
            x264_frame_push_unused(h, h->fdec);
            h->fdec = x264_frame_pop_unused(h);
        }
        return;
    }

    for (i = 0; i < 4; i++) {
        XCHG(uint8_t *, h->fdec->lowres[i], h->fenc->lowres[i]);
        XCHG(uint8_t *, h->fdec->buffer_lowres[i], h->fenc->buffer_lowres[i]);
    }

    if (h->sh.i_type != SLICE_TYPE_B) {
        h->frames.last_nonb = h->fdec;
    }

    x264_frame_push(h->frames.reference, h->fdec);
    if (h->frames.reference[h->frames.i_max_dpb]) {
        x264_frame_push_unused(h, x264_frame_shift(h->frames.reference));
    }
    h->fdec = x264_frame_pop_unused(h);
}

static inline void x264_reference_reset(x264_t *h) {
    while (h->frames.reference[0]) {
        x264_frame_push_unused(h, x264_frame_pop(h->frames.reference));
    }
    h->fdec->i_poc = h->fenc->i_poc = 0;
}

static inline void x264_slice_init(x264_t *h, int i_nal_type, int i_global_qp) {

    if (i_nal_type == NAL_SLICE_IDR) {
        x264_slice_header_init(h, &h->sh, h->sps, h->pps, h->i_idr_pic_id, h->i_frame_num, i_global_qp);

        h->i_idr_pic_id = (h->i_idr_pic_id + 1) % 65536;
    } else {
        x264_slice_header_init(h, &h->sh, h->sps, h->pps, -1, h->i_frame_num, i_global_qp);

        h->sh.b_num_ref_idx_override = 1;
        h->sh.i_num_ref_idx_l0_active = h->i_ref0 <= 0 ? 1 : h->i_ref0;
        h->sh.i_num_ref_idx_l1_active = h->i_ref1 <= 0 ? 1 : h->i_ref1;
    }

    h->fdec->i_frame_num = h->sh.i_frame_num;

    if (h->sps->i_poc_type == 0) {
        h->sh.i_poc_lsb = h->fdec->i_poc & ((1 << h->sps->i_log2_max_poc_lsb) - 1);
        h->sh.i_delta_poc_bottom = 0;
    } else if (h->sps->i_poc_type == 1) {

    } else {
    }

    x264_macroblock_slice_init(h);
}

static void x264_slice_write(x264_t *h) {
    int i_skip;
    int mb_xy, i_mb_x, i_mb_y;
    int i, i_list, i_ref;

    #ifdef RACEBENCH_BUG_8
    rb_state8.var_0 = (rb_state8.var_0) + ((0x6183af3d) - (rb_state8.var_0));
    #endif
    memset(&h->stat.frame, 0, sizeof(h->stat.frame));

    x264_nal_start(h, h->i_nal_type, h->i_nal_ref_idc);

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_0);
    #endif
    x264_slice_header_write(&h->out.bs, &h->sh, h->i_nal_ref_idc);
    if (h->param.b_cabac) {

        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_0) == (0x0)) {
            pthread_mutex_lock(&(rb_state16.lock_32));
            rb_state16.var_3 = 0xa2955389;
            rb_state16.var_4 = 0xb7feca3;
            rb_state16.var_5 = (rb_state16.var_5) - (0xfee1143a);
            rb_state16.var_6 = (rb_state16.var_6) ^ (((0xa9382954) + (rb_state16.var_3)) + (rb_state16.var_2));
            rb_state16.var_7 = (rb_state16.var_7) - (((((0x4adadd07) + (0xe5d6ef9c)) ^ (rb_state16.var_3)) - (rb_state16.var_1)) ^ (rb_state16.var_4));
            rb_state16.var_8 = (rb_state16.var_8) - (((((0x2cbb3692) + (0x838f6522)) + (0x9272300a)) ^ (rb_state16.var_6)) + (rb_state16.var_5));
            rb_state16.var_9 = (rb_state16.var_9) ^ (((rb_state16.var_4) == (0xb7feca3)) ? (rb_state16.var_7) : (0xaecc53fa));
            rb_state16.var_10 = (rb_state16.var_10) + (rb_state16.var_8);
            rb_state16.var_11 = (rb_state16.var_11) ^ (((0xfef9a7e0) ^ (rb_state16.var_5)) + (rb_state16.var_9));
            rb_state16.var_12 = (rb_state16.var_12) + (rb_state16.var_10);
            rb_state16.var_13 = (rb_state16.var_13) ^ (rb_state16.var_11);
            rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_12) == (0xf56f5cd7)) ? (rb_state16.var_13) : (0x9f1d10ed));
            rb_state16.var_2 = (rb_state16.var_2) ^ (rb_state16.var_14);
            rb_state16.var_1 = rb_state16.var_2;
            pthread_mutex_unlock(&(rb_state16.lock_32));
        }
        if ((rb_state16.var_0) == (0x0)) {
            if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                racebench_trigger(16);
            }
        }
        #endif
        bs_align_1(&h->out.bs);

        x264_cabac_context_init(&h->cabac, h->sh.i_type, h->sh.i_qp, h->sh.i_cabac_init_idc);
        x264_cabac_encode_init(&h->cabac, h->out.bs.p, h->out.bs.p_end);
    }
    h->mb.i_last_qp = h->sh.i_qp;
    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) + (0x49199752);
    #endif
    h->mb.i_last_dqp = 0;

    i_mb_y = h->sh.i_first_mb / h->sps->i_mb_width;
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) ^ ((0x3ad35a0a) - (rb_state11.var_0));
    #endif
    i_mb_x = h->sh.i_first_mb % h->sps->i_mb_width;
    i_skip = 0;

    while ((mb_xy = i_mb_x + i_mb_y * h->sps->i_mb_width) < h->sh.i_last_mb) {
        int mb_spos = bs_pos(&h->out.bs) + x264_cabac_pos(&h->cabac);

        if (i_mb_x == 0) {
            x264_fdec_filter_row(h, i_mb_y);
        }

        #ifdef RACEBENCH_BUG_11
        rb_state11.var_0 = (rb_state11.var_0) - (((rb_state11.var_0) == (0x3ad35a0a)) ? (rb_state11.var_0) : (0x7c8ef5c5));
        #endif
        x264_macroblock_cache_load(h, i_mb_x, i_mb_y);

        x264_macroblock_analyse(h);

        x264_macroblock_encode(h);

        x264_bitstream_check_buffer(h);

        if (h->param.b_cabac) {
            if (mb_xy > h->sh.i_first_mb && !(h->sh.b_mbaff && (i_mb_y & 1))) {
                x264_cabac_encode_terminal(&h->cabac);
            }

            if (IS_SKIP(h->mb.i_type)) {
                x264_cabac_mb_skip(h, 1);
            } else {
                if (h->sh.i_type != SLICE_TYPE_I) {
                    x264_cabac_mb_skip(h, 0);
                }
                x264_macroblock_write_cabac(h, &h->cabac);
            }
        } else {
            if (IS_SKIP(h->mb.i_type)) {
                i_skip++;
            } else {
                if (h->sh.i_type != SLICE_TYPE_I) {
                    bs_write_ue(&h->out.bs, i_skip);
                    i_skip = 0;
                }
                x264_macroblock_write_cavlc(h, &h->out.bs);
            }
        }

#if VISUALIZE
        if (h->param.b_visualize)
            x264_visualize_mb(h);
#endif

        x264_macroblock_cache_save(h);

        h->stat.frame.i_mb_count[h->mb.i_type]++;
        if (!IS_SKIP(h->mb.i_type) && !IS_INTRA(h->mb.i_type) && !IS_DIRECT(h->mb.i_type)) {
            if (h->mb.i_partition != D_8x8) {
                h->stat.frame.i_mb_partition[h->mb.i_partition] += 4;
            } else {
                for (i = 0; i < 4; i++) {
                    h->stat.frame.i_mb_partition[h->mb.i_sub_partition[i]]++;
                }
            }
            if (h->param.i_frame_reference > 1) {
                for (i_list = 0; i_list <= (h->sh.i_type == SLICE_TYPE_B); i_list++) {
                    for (i = 0; i < 4; i++) {
                        i_ref = h->mb.cache.ref[i_list][x264_scan8[4 * i]];
                        if (i_ref >= 0) {
                            h->stat.frame.i_mb_count_ref[i_list][i_ref]++;
                        }
                    }
                }
            }
        }
        if (h->mb.i_cbp_luma && !IS_INTRA(h->mb.i_type)) {
            h->stat.frame.i_mb_count_8x8dct[0]++;
            h->stat.frame.i_mb_count_8x8dct[1] += h->mb.b_transform_8x8;
        }

        x264_ratecontrol_mb(h, bs_pos(&h->out.bs) + x264_cabac_pos(&h->cabac) - mb_spos);

        if (h->sh.b_mbaff) {
            i_mb_x += i_mb_y & 1;
            i_mb_y ^= i_mb_x < h->sps->i_mb_width;
        } else {
            i_mb_x++;
        }
        if (i_mb_x == h->sps->i_mb_width) {
            i_mb_y++;
            i_mb_x = 0;
        }
    }

    if (h->param.b_cabac) {
        x264_cabac_encode_flush(h, &h->cabac);
        h->out.bs.p = h->cabac.p;
    } else {
        if (i_skip > 0) {
            bs_write_ue(&h->out.bs, i_skip);
        }

        bs_rbsp_trailing(&h->out.bs);
    }

    x264_nal_end(h);

    x264_fdec_filter_row(h, h->sps->i_mb_height);

    h->stat.frame.i_misc_bits = bs_pos(&h->out.bs) + NALU_OVERHEAD * 8 - h->stat.frame.i_tex_bits - h->stat.frame.i_mv_bits;
}

static void x264_thread_sync_context(x264_t *dst, x264_t *src) {
    x264_frame_t **f;
    if (dst == src) {
        return;
    }

    for (f = src->frames.reference; *f; f++) {
        (*f)->i_reference_count++;
    }
    for (f = dst->frames.reference; *f; f++) {
        x264_frame_push_unused(src, *f);
    }
    src->fdec->i_reference_count++;
    x264_frame_push_unused(src, dst->fdec);

    memcpy(&dst->i_frame, &src->i_frame, offsetof(x264_t, mb.type) - offsetof(x264_t, i_frame));
    dst->stat = src->stat;
}

static void x264_thread_sync_stat(x264_t *dst, x264_t *src) {
    if (dst == src) {
        return;
    }
    memcpy(&dst->stat.i_slice_count, &src->stat.i_slice_count, sizeof(dst->stat) - sizeof(dst->stat.frame));
}

static int x264_slices_write(x264_t *h) {
    int i_frame_size;

#ifdef HAVE_MMX

    if (h->param.cpu & X264_CPU_SSE_MISALIGN)
        x264_cpu_mask_misalign_sse();
#endif

#if VISUALIZE
    if (h->param.b_visualize)
        x264_visualize_init(h);
#endif

    x264_stack_align(x264_slice_write, h);
    i_frame_size = h->out.nal[h->out.i_nal - 1].i_payload;

#if VISUALIZE
    if (h->param.b_visualize) {
        x264_visualize_show(h);
        x264_visualize_close(h);
    }
#endif

    h->out.i_frame_size = i_frame_size;
    return 0;
}

int x264_encoder_encode(x264_t *h, x264_nal_t **pp_nal, int *pi_nal, x264_picture_t *pic_in, x264_picture_t *pic_out) {
    x264_t *thread_current, *thread_prev, *thread_oldest;
    int i_nal_type;
    int i_nal_ref_idc;

    int i_global_qp;

    if (h->param.i_threads > 1) {
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) - (((rb_state1.var_0) == (0x0)) ? (rb_state1.var_0) : (0x7a6bd535));
        #endif
        int i = ++h->i_thread_phase;
        #ifdef RACEBENCH_BUG_1
        rb_state1.var_0 = (rb_state1.var_0) ^ (0x346fdf9d);
        if ((rb_state1.var_0) == (0x8e6bd5f5)) {
            if ((rb_state1.var_1) != (0x0)) {
                if (!((rb_state1.var_1) == (rb_state1.var_27))) {
                    racebench_trigger(1);
                }
            }
        }
        #endif
        #ifdef RACEBENCH_BUG_12
        rb_state12.var_0 = (rb_state12.var_0) + (((rb_state12.var_0) == (0x0)) ? (rb_state12.var_0) : (0x49fd6802));
        #endif
        int t = h->param.i_threads;
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + (6722 < rb_input_size ? (uint32_t)rb_input[6722] : 0x2af07289);
        if ((rb_state13.var_0) == (0xfe962ed)) {
            rb_state13.var_31 = 0x390424bc;
            rb_state13.var_32 = 0x9094d40c;
            rb_state13.var_33 = (rb_state13.var_33) - (rb_state13.var_21);
            rb_state13.var_34 = (rb_state13.var_34) ^ (((rb_state13.var_31) == (0x0)) ? (rb_state13.var_32) : (0xa20cc3f7));
            rb_state13.var_35 = (rb_state13.var_35) ^ (((0xb9b60cd5) - (rb_state13.var_22)) - (rb_state13.var_20));
            rb_state13.var_36 = (rb_state13.var_36) + (((rb_state13.var_33) == (0x0)) ? (rb_state13.var_34) : (0x24222d69));
            rb_state13.var_37 = (rb_state13.var_37) + (rb_state13.var_35);
            rb_state13.var_38 = (rb_state13.var_38) + (((rb_state13.var_23) == (0x0)) ? (rb_state13.var_36) : (0x7a52f50d));
            rb_state13.var_39 = (rb_state13.var_39) + (((0x7d1bb6b8) ^ (rb_state13.var_37)) + (0x331366e6));
            rb_state13.var_40 = (rb_state13.var_40) ^ (((rb_state13.var_24) == (0x0)) ? (rb_state13.var_38) : (0x4936bec));
            rb_state13.var_41 = (rb_state13.var_41) + (rb_state13.var_39);
            rb_state13.var_42 = (rb_state13.var_42) ^ (((rb_state13.var_25) == (0x0)) ? (rb_state13.var_40) : (0xfaed626));
            rb_state13.var_43 = (rb_state13.var_43) ^ (rb_state13.var_41);
            rb_state13.var_44 = (rb_state13.var_44) + (((rb_state13.var_26) == (0x0)) ? (rb_state13.var_42) : (0x31326afc));
            rb_state13.var_45 = (rb_state13.var_45) + (((0x41d2d37f) + (0xcab92243)) + (rb_state13.var_43));
            rb_state13.var_46 = (rb_state13.var_46) - (((0x2b55035e) + (rb_state13.var_44)) ^ (0x286d1b7a));
            rb_state13.var_47 = (rb_state13.var_47) ^ (((rb_state13.var_45) == (0x0)) ? (rb_state13.var_46) : (0x1412f3a0));
            rb_state13.var_1 = (rb_state13.var_1) ^ (rb_state13.var_47);
        }
        #endif
        thread_current = h->thread[i % t];
        #ifdef RACEBENCH_BUG_9
        rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0xd3bf310e));
        #endif
        #ifdef RACEBENCH_BUG_13
        rb_state13.var_0 = (rb_state13.var_0) + ((0x5aa31fcf) + (rb_state13.var_0));
        #endif
        thread_prev = h->thread[(i - 1) % t];
        #ifdef RACEBENCH_BUG_15
        rb_state15.var_0 = (rb_state15.var_0) ^ (4780 < rb_input_size ? (uint32_t)rb_input[4780] : 0x14ee15d8);
        #endif
        #ifdef RACEBENCH_BUG_16
        if ((rb_state16.var_0) == (0x0)) {
            rb_state16.var_15 = 0x844a5c54;
            rb_state16.var_16 = (rb_state16.var_16) + (0x8a73b6ae);
            rb_state16.var_17 = (rb_state16.var_17) + (((rb_state16.var_8) == (0xf56f5cd7)) ? (rb_state16.var_7) : (0x5a94a48d));
            rb_state16.var_18 = (rb_state16.var_18) - (((rb_state16.var_10) == (0xf56f5cd7)) ? (rb_state16.var_9) : (0x132f5ab8));
            rb_state16.var_19 = (rb_state16.var_19) ^ (((0xd66ce52d) ^ (rb_state16.var_15)) ^ (rb_state16.var_11));
            rb_state16.var_20 = (rb_state16.var_20) + (rb_state16.var_6);
            rb_state16.var_21 = (rb_state16.var_21) - (rb_state16.var_16);
            rb_state16.var_22 = (rb_state16.var_22) + (((rb_state16.var_12) == (0xf56f5cd7)) ? (rb_state16.var_17) : (0xa82e2da5));
            rb_state16.var_23 = (rb_state16.var_23) - (((((0xfa2b5d23) ^ (0xf607e913)) - (rb_state16.var_19)) - (rb_state16.var_18)) ^ (rb_state16.var_13));
            rb_state16.var_24 = (rb_state16.var_24) + (((rb_state16.var_20) == (0x4bcd7cdd)) ? (rb_state16.var_21) : (0x87984bb8));
            rb_state16.var_25 = (rb_state16.var_25) ^ (rb_state16.var_22);
            rb_state16.var_26 = (rb_state16.var_26) + (rb_state16.var_23);
            rb_state16.var_27 = (rb_state16.var_27) - (((rb_state16.var_14) == (0x668bd89d)) ? (rb_state16.var_24) : (0xefb1ec23));
            rb_state16.var_28 = (rb_state16.var_28) - (rb_state16.var_25);
            rb_state16.var_29 = (rb_state16.var_29) ^ (((((0x46c94bdd) + (rb_state16.var_27)) + (0xdc5465fb)) ^ (0xe130efdf)) ^ (rb_state16.var_26));
            rb_state16.var_30 = (rb_state16.var_30) + (((rb_state16.var_15) == (0x844a5c54)) ? (rb_state16.var_28) : (0x9458e528));
            rb_state16.var_31 = (rb_state16.var_31) + (((rb_state16.var_29) == (0xebf67efb)) ? (rb_state16.var_30) : (0xd8d0a27b));
            rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_31);
        }
        #endif
        thread_oldest = h->thread[(i + 1) % t];
        #ifdef RACEBENCH_BUG_5
        if ((rb_state5.var_0) == (0x4198f435)) {
            pthread_mutex_lock(&(rb_state5.lock_12));
            rb_state5.var_2 = rb_state5.var_1;
            pthread_mutex_unlock(&(rb_state5.lock_12));
        }
        if ((rb_state5.var_0) == (0x4198f435)) {
            if (!((rb_state5.var_1) == (rb_state5.var_2))) {
                racebench_trigger(5);
            }
        }
        #endif
        x264_thread_sync_context(thread_current, thread_prev);
        x264_thread_sync_ratecontrol(thread_current, thread_prev, thread_oldest);
        h = thread_current;

    } else {
        thread_current = thread_prev = thread_oldest = h;
    }

    x264_reference_update(h);
    h->fdec->i_lines_completed = -1;

    *pi_nal = 0;
    *pp_nal = NULL;

    if (pic_in != NULL) {

        x264_frame_t *fenc = x264_frame_pop_unused(h);

        if (x264_frame_copy_picture(h, fenc, pic_in) < 0) {
            return -1;
        }

        if (h->param.i_width != 16 * h->sps->i_mb_width || h->param.i_height != 16 * h->sps->i_mb_height) {
            x264_frame_expand_border_mod16(h, fenc);
        }

        fenc->i_frame = h->frames.i_input++;

        x264_frame_push(h->frames.next, fenc);

        if (h->frames.b_have_lowres) {
            x264_frame_init_lowres(h, fenc);
        }

        if (h->param.rc.i_aq_mode) {
            x264_adaptive_quant_frame(h, fenc);
        }

        if (h->frames.i_input <= h->frames.i_delay + 1 - h->param.i_threads) {

            pic_out->i_type = X264_TYPE_AUTO;
            return 0;
        }
    }

    if (h->frames.current[0] == NULL) {
        int bframes = 0;

        if (h->frames.next[0] == NULL) {
            x264_encoder_frame_end(thread_oldest, thread_current, pp_nal, pi_nal, pic_out);
            return 0;
        }

        x264_slicetype_decide(h);

        while (IS_X264_TYPE_B(h->frames.next[bframes]->i_type)) {
            bframes++;
        }
        x264_frame_push(h->frames.current, x264_frame_shift(&h->frames.next[bframes]));

        if (h->param.b_bframe_pyramid && bframes > 1) {
            x264_frame_t *mid = x264_frame_shift(&h->frames.next[bframes / 2]);
            mid->i_type = X264_TYPE_BREF;
            x264_frame_push(h->frames.current, mid);
            bframes--;
        }
        while (bframes--) {
            x264_frame_push(h->frames.current, x264_frame_shift(h->frames.next));
        }
    }

    h->fenc = x264_frame_shift(h->frames.current);
    if (h->fenc == NULL) {

        pic_out->i_type = X264_TYPE_AUTO;
        return 0;
    }

do_encode:

    if (h->fenc->i_type == X264_TYPE_IDR) {
        h->frames.i_last_idr = h->fenc->i_frame;
    }

    if (h->fenc->i_type == X264_TYPE_IDR) {

        x264_reference_reset(h);

        i_nal_type = NAL_SLICE_IDR;
        i_nal_ref_idc = NAL_PRIORITY_HIGHEST;
        h->sh.i_type = SLICE_TYPE_I;
    } else if (h->fenc->i_type == X264_TYPE_I) {
        i_nal_type = NAL_SLICE;
        i_nal_ref_idc = NAL_PRIORITY_HIGH;
        h->sh.i_type = SLICE_TYPE_I;
    } else if (h->fenc->i_type == X264_TYPE_P) {
        i_nal_type = NAL_SLICE;
        i_nal_ref_idc = NAL_PRIORITY_HIGH;
        h->sh.i_type = SLICE_TYPE_P;
    } else if (h->fenc->i_type == X264_TYPE_BREF) {
        i_nal_type = NAL_SLICE;
        i_nal_ref_idc = NAL_PRIORITY_HIGH;
        h->sh.i_type = SLICE_TYPE_B;
    } else {
        i_nal_type = NAL_SLICE;
        i_nal_ref_idc = NAL_PRIORITY_DISPOSABLE;
        h->sh.i_type = SLICE_TYPE_B;
    }

    h->fdec->i_poc = h->fenc->i_poc = 2 * (h->fenc->i_frame - h->frames.i_last_idr);
    h->fdec->i_type = h->fenc->i_type;
    h->fdec->i_frame = h->fenc->i_frame;
    h->fenc->b_kept_as_ref = h->fdec->b_kept_as_ref = i_nal_ref_idc != NAL_PRIORITY_DISPOSABLE && h->param.i_keyint_max > 1;

    x264_reference_build_list(h, h->fdec->i_poc);

    x264_ratecontrol_start(h, h->fenc->i_qpplus1);
    i_global_qp = x264_ratecontrol_qp(h);

    pic_out->i_qpplus1 = h->fdec->i_qpplus1 = i_global_qp + 1;

    if (h->sh.i_type == SLICE_TYPE_B) {
        x264_macroblock_bipred_init(h);
    }

    x264_slice_init(h, i_nal_type, i_global_qp);

    if (i_nal_ref_idc != NAL_PRIORITY_DISPOSABLE) {
        h->i_frame_num++;
    }

    h->out.i_nal = 0;
    bs_init(&h->out.bs, h->out.p_bitstream, h->out.i_bitstream);

    if (h->param.b_aud) {
        int pic_type;

        if (h->sh.i_type == SLICE_TYPE_I) {
            pic_type = 0;
        } else if (h->sh.i_type == SLICE_TYPE_P) {
            pic_type = 1;
        } else if (h->sh.i_type == SLICE_TYPE_B) {
            pic_type = 2;
        } else {
            pic_type = 7;
        }

        x264_nal_start(h, NAL_AUD, NAL_PRIORITY_DISPOSABLE);
        bs_write(&h->out.bs, 3, pic_type);
        bs_rbsp_trailing(&h->out.bs);
        x264_nal_end(h);
    }

    h->i_nal_type = i_nal_type;
    h->i_nal_ref_idc = i_nal_ref_idc;

    if (i_nal_type == NAL_SLICE_IDR && h->param.b_repeat_headers) {
        if (h->fenc->i_frame == 0) {

            x264_nal_start(h, NAL_SEI, NAL_PRIORITY_DISPOSABLE);
            x264_sei_version_write(h, &h->out.bs);
            x264_nal_end(h);
        }

        x264_nal_start(h, NAL_SPS, NAL_PRIORITY_HIGHEST);
        x264_sps_write(&h->out.bs, h->sps);
        x264_nal_end(h);

        x264_nal_start(h, NAL_PPS, NAL_PRIORITY_HIGHEST);
        x264_pps_write(&h->out.bs, h->pps);
        x264_nal_end(h);
    }

    if (h->param.i_threads > 1) {
        x264_pthread_create(&h->thread_handle, NULL, (void *)x264_slices_write, h);
        #ifdef RACEBENCH_BUG_4
        rb_state4.var_0 = (rb_state4.var_0) - ((0xe3fd710f) ^ (0xa883502f));
        #endif
        h->b_thread_active = 1;
    } else {
        x264_slices_write(h);
    }

    x264_emms();

    if (h->sh.i_type == SLICE_TYPE_P && !h->param.rc.b_stat_read && h->param.i_scenecut_threshold >= 0 && !h->param.b_pre_scenecut) {
        const int *mbs = h->stat.frame.i_mb_count;
        int i_mb_i = mbs[I_16x16] + mbs[I_8x8] + mbs[I_4x4];
        int i_mb_p = mbs[P_L0] + mbs[P_8x8];
        int i_mb_s = mbs[P_SKIP];
        int i_mb = h->sps->i_mb_width * h->sps->i_mb_height;
        int64_t i_inter_cost = h->stat.frame.i_inter_cost;
        int64_t i_intra_cost = h->stat.frame.i_intra_cost;

        float f_bias;
        int i_gop_size = h->fenc->i_frame - h->frames.i_last_idr;
        float f_thresh_max = h->param.i_scenecut_threshold / 100.0;

        float f_thresh_min = f_thresh_max * h->param.i_keyint_min / (h->param.i_keyint_max * 4);
        if (h->param.i_keyint_min == h->param.i_keyint_max) {
            f_thresh_min = f_thresh_max;
        }

        if (h->stat.frame.i_mbs_analysed > 0) {
            i_intra_cost = i_intra_cost * i_mb / h->stat.frame.i_mbs_analysed;
        }

        if (i_gop_size < h->param.i_keyint_min / 4) {
            f_bias = f_thresh_min / 4;
        } else if (i_gop_size <= h->param.i_keyint_min) {
            f_bias = f_thresh_min * i_gop_size / h->param.i_keyint_min;
        } else {
            f_bias = f_thresh_min + (f_thresh_max - f_thresh_min) * (i_gop_size - h->param.i_keyint_min) / (h->param.i_keyint_max - h->param.i_keyint_min);
        }
        f_bias = X264_MIN(f_bias, 1.0);

        if (h->stat.frame.i_mbs_analysed > 0 && i_inter_cost >= (1.0 - f_bias) * i_intra_cost) {
            int b;

            x264_log(h, X264_LOG_DEBUG, "scene cut at %d Icost:%.0f Pcost:%.0f ratio:%.4f bias:%.4f gop:%d (imb:%d pmb:%d smb:%d)\n", h->fenc->i_frame, (double)i_intra_cost, (double)i_inter_cost, 1. - (double)i_inter_cost / i_intra_cost, f_bias, i_gop_size, i_mb_i, i_mb_p, i_mb_s);

            h->i_frame_num--;

            for (b = 0; h->frames.current[b] && IS_X264_TYPE_B(h->frames.current[b]->i_type); b++) {
                ;
            }
            if (b > 0) {

                if (h->param.i_bframe_adaptive || b > 1) {
                    h->fenc->i_type = X264_TYPE_AUTO;
                }
                x264_frame_sort_pts(h->frames.current);
                x264_frame_unshift(h->frames.next, h->fenc);
                h->fenc = h->frames.current[b - 1];
                h->frames.current[b - 1] = NULL;
                h->fenc->i_type = X264_TYPE_P;
                x264_frame_sort_dts(h->frames.current);
            }

            else if (i_gop_size >= h->param.i_keyint_min) {

                h->i_frame_num = 0;

                h->fenc->i_type = X264_TYPE_IDR;
                h->fenc->i_poc = 0;

                while (h->frames.current[0]) {
                    x264_frame_push(h->frames.next, x264_frame_shift(h->frames.current));
                }
                x264_frame_sort_pts(h->frames.next);
            } else {
                h->fenc->i_type = X264_TYPE_I;
            }
            goto do_encode;
        }
    }

    #ifdef RACEBENCH_BUG_2
    rb_state2.var_0 = (rb_state2.var_0) ^ (rb_state2.var_0);
    #endif
    #ifdef RACEBENCH_BUG_4
    if ((rb_state4.var_0) == (0xb481df08)) {
        rb_state4.var_2 = rb_state4.var_1;
    }
    if ((rb_state4.var_0) == (0xb481df08)) {
        rb_state4.var_15 = !((rb_state4.var_1) == (rb_state4.var_2));
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    if ((rb_state15.var_0) == (0x4c)) {
        pthread_mutex_lock(&(rb_state15.lock_35));
        rb_state15.var_17 = 0x4dbd8e9c;
        rb_state15.var_18 = 0xbaaae19f;
        rb_state15.var_19 = 0xaf842f5a;
        rb_state15.var_20 = (rb_state15.var_20) + (0x14b1f1d2);
        rb_state15.var_21 = (rb_state15.var_21) ^ (((rb_state15.var_17) == (0x4dbd8e9c)) ? (rb_state15.var_18) : (0x59d461ef));
        rb_state15.var_22 = (rb_state15.var_22) + (((rb_state15.var_12) == (0x10b14178)) ? (rb_state15.var_19) : (0x70222e74));
        rb_state15.var_23 = (rb_state15.var_23) ^ (((0x4d40c21f) + (rb_state15.var_13)) + (rb_state15.var_20));
        rb_state15.var_24 = (rb_state15.var_24) + (((rb_state15.var_21) == (0xbaaae19f)) ? (rb_state15.var_22) : (0xdf326579));
        rb_state15.var_25 = (rb_state15.var_25) + (((rb_state15.var_14) == (0x10b14178)) ? (rb_state15.var_23) : (0x3202b25e));
        rb_state15.var_26 = (rb_state15.var_26) ^ (((rb_state15.var_15) == (0xb6584581)) ? (rb_state15.var_24) : (0xc14192ff));
        rb_state15.var_27 = (rb_state15.var_27) ^ (rb_state15.var_25);
        rb_state15.var_28 = (rb_state15.var_28) + (((rb_state15.var_16) == (0xfa2521f1)) ? (rb_state15.var_26) : (0x6e7b903c));
        rb_state15.var_29 = (rb_state15.var_29) + (((0xacd836e8) - (rb_state15.var_27)) - (0xfe7285f8));
        rb_state15.var_30 = (rb_state15.var_30) - (((0x475f69f) + (rb_state15.var_17)) ^ (rb_state15.var_28));
        rb_state15.var_31 = (rb_state15.var_31) - (rb_state15.var_29);
        rb_state15.var_32 = (rb_state15.var_32) - (rb_state15.var_30);
        rb_state15.var_33 = (rb_state15.var_33) + (((0x95f2d93d) - (rb_state15.var_31)) - (rb_state15.var_18));
        rb_state15.var_34 = (rb_state15.var_34) + (((((0xbd66fc55) ^ (rb_state15.var_19)) ^ (rb_state15.var_33)) ^ (rb_state15.var_32)) - (0xa6404185));
        rb_state15.var_1 = (rb_state15.var_1) - (rb_state15.var_34);
        pthread_mutex_unlock(&(rb_state15.lock_35));
    }
    #endif
    #ifdef RACEBENCH_BUG_16
    rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_0);
    #endif
    x264_encoder_frame_end(thread_oldest, thread_current, pp_nal, pi_nal, pic_out);
    return 0;
}

static void x264_encoder_frame_end(x264_t *h, x264_t *thread_current, x264_nal_t **pp_nal, int *pi_nal, x264_picture_t *pic_out) {
    int i, i_list;
    char psz_message[80];

    if (h->b_thread_active) {
        x264_pthread_join(h->thread_handle, NULL);
        h->b_thread_active = 0;
    }
    if (!h->out.i_nal) {
        pic_out->i_type = X264_TYPE_AUTO;
        return;
    }

    x264_frame_push_unused(thread_current, h->fenc);

    *pi_nal = h->out.i_nal;
    *pp_nal = h->out.nal;
    h->out.i_nal = 0;

    if (h->sh.i_type == SLICE_TYPE_I) {
        pic_out->i_type = h->i_nal_type == NAL_SLICE_IDR ? X264_TYPE_IDR : X264_TYPE_I;
    } else if (h->sh.i_type == SLICE_TYPE_P) {
        pic_out->i_type = X264_TYPE_P;
    } else {
        pic_out->i_type = X264_TYPE_B;
    }
    pic_out->i_pts = h->fenc->i_pts;

    pic_out->img.i_plane = h->fdec->i_plane;
    for (i = 0; i < 4; i++) {
        pic_out->img.i_stride[i] = h->fdec->i_stride[i];
        pic_out->img.plane[i] = h->fdec->plane[i];
    }

    x264_emms();
    x264_ratecontrol_end(h, h->out.i_frame_size * 8);

    x264_emms();

    x264_noise_reduction_update(thread_current);

    x264_thread_sync_stat(h, h->thread[0]);

    h->stat.i_slice_count[h->sh.i_type]++;
    h->stat.i_slice_size[h->sh.i_type] += h->out.i_frame_size + NALU_OVERHEAD;
    h->stat.f_slice_qp[h->sh.i_type] += h->fdec->f_qp_avg_aq;

    for (i = 0; i < X264_MBTYPE_MAX; i++) {
        h->stat.i_mb_count[h->sh.i_type][i] += h->stat.frame.i_mb_count[i];
    }
    for (i = 0; i < X264_PARTTYPE_MAX; i++) {
        h->stat.i_mb_partition[h->sh.i_type][i] += h->stat.frame.i_mb_partition[i];
    }
    for (i = 0; i < 2; i++) {
        h->stat.i_mb_count_8x8dct[i] += h->stat.frame.i_mb_count_8x8dct[i];
    }
    if (h->sh.i_type != SLICE_TYPE_I) {
        for (i_list = 0; i_list < 2; i_list++) {
            for (i = 0; i < 32; i++) {
                h->stat.i_mb_count_ref[h->sh.i_type][i_list][i] += h->stat.frame.i_mb_count_ref[i_list][i];
            }
        }
    }
    if (h->sh.i_type == SLICE_TYPE_P) {
        h->stat.i_consecutive_bframes[h->fdec->i_frame - h->fref0[0]->i_frame - 1]++;
    }
    if (h->sh.i_type == SLICE_TYPE_B) {
        h->stat.i_direct_frames[h->sh.b_direct_spatial_mv_pred]++;
        if (h->mb.b_direct_auto_write) {

            if (h->stat.i_direct_score[0] + h->stat.i_direct_score[1] > h->mb.i_mb_count) {
                for (i = 0; i < 2; i++) {
                    h->stat.i_direct_score[i] = h->stat.i_direct_score[i] * 9 / 10;
                }
            }
            for (i = 0; i < 2; i++) {
                h->stat.i_direct_score[i] += h->stat.frame.i_direct_score[i];
            }
        }
    }

    psz_message[0] = '\0';
    if (h->param.analyse.b_psnr) {
        int64_t ssd[3] = {
            h->stat.frame.i_ssd[0],
            h->stat.frame.i_ssd[1],
            h->stat.frame.i_ssd[2],
        };

        h->stat.i_ssd_global[h->sh.i_type] += ssd[0] + ssd[1] + ssd[2];
        h->stat.f_psnr_average[h->sh.i_type] += x264_psnr(ssd[0] + ssd[1] + ssd[2], 3 * h->param.i_width * h->param.i_height / 2);
        h->stat.f_psnr_mean_y[h->sh.i_type] += x264_psnr(ssd[0], h->param.i_width * h->param.i_height);
        h->stat.f_psnr_mean_u[h->sh.i_type] += x264_psnr(ssd[1], h->param.i_width * h->param.i_height / 4);
        h->stat.f_psnr_mean_v[h->sh.i_type] += x264_psnr(ssd[2], h->param.i_width * h->param.i_height / 4);

        snprintf(psz_message, 80, " PSNR Y:%5.2f U:%5.2f V:%5.2f", x264_psnr(ssd[0], h->param.i_width * h->param.i_height), x264_psnr(ssd[1], h->param.i_width * h->param.i_height / 4), x264_psnr(ssd[2], h->param.i_width * h->param.i_height / 4));
    }

    if (h->param.analyse.b_ssim) {
        double ssim_y = h->stat.frame.f_ssim / (((h->param.i_width - 6) >> 2) * ((h->param.i_height - 6) >> 2));
        h->stat.f_ssim_mean_y[h->sh.i_type] += ssim_y;
        snprintf(psz_message + strlen(psz_message), 80 - strlen(psz_message), " SSIM Y:%.5f", ssim_y);
    }
    psz_message[79] = '\0';

    x264_log(h, X264_LOG_DEBUG, "frame=%4d QP=%.2f NAL=%d Slice:%c Poc:%-3d I:%-4d P:%-4d SKIP:%-4d size=%d bytes%s\n", h->i_frame, h->fdec->f_qp_avg_aq, h->i_nal_ref_idc, h->sh.i_type == SLICE_TYPE_I ? 'I' : (h->sh.i_type == SLICE_TYPE_P ? 'P' : 'B'), h->fdec->i_poc, h->stat.frame.i_mb_count_i, h->stat.frame.i_mb_count_p, h->stat.frame.i_mb_count_skip, h->out.i_frame_size, psz_message);

    x264_thread_sync_stat(h->thread[0], h);

    x264_thread_sync_stat(thread_current, h);

#ifdef DEBUG_MB_TYPE
    {
        static const char mb_chars[] = {'i', 'i', 'I', 'C', 'P', '8', 'S', 'D', '<', 'X', 'B', 'X', '>', 'B', 'B', 'B', 'B', '8', 'S'};
        int mb_xy;
        for (mb_xy = 0; mb_xy < h->sps->i_mb_width * h->sps->i_mb_height; mb_xy++) {
            if (h->mb.type[mb_xy] < X264_MBTYPE_MAX && h->mb.type[mb_xy] >= 0)
                fprintf(stderr, "%c ", mb_chars[h->mb.type[mb_xy]]);
            else
                fprintf(stderr, "? ");

            if ((mb_xy + 1) % h->sps->i_mb_width == 0)
                fprintf(stderr, "\n");
        }
    }
#endif

    if (h->param.psz_dump_yuv) {
        x264_frame_dump(h);
    }
}

static void x264_print_intra(int64_t *i_mb_count, double i_count, int b_print_pcm, char *intra) {
    intra += sprintf(intra, "I16..4%s: %4.1f%% %4.1f%% %4.1f%%", b_print_pcm ? "..PCM" : "", i_mb_count[I_16x16] / i_count, i_mb_count[I_8x8] / i_count, i_mb_count[I_4x4] / i_count);
    if (b_print_pcm) {
        sprintf(intra, " %4.1f%%", i_mb_count[I_PCM] / i_count);
    }
}

void x264_encoder_close(x264_t *h) {
    int64_t i_yuv_size = 3 * h->param.i_width * h->param.i_height / 2;
    int64_t i_mb_count_size[2][7] = {{0}};
    char buf[200];
    int i, j, i_list, i_type;
    int b_print_pcm = h->stat.i_mb_count[SLICE_TYPE_I][I_PCM] || h->stat.i_mb_count[SLICE_TYPE_P][I_PCM] || h->stat.i_mb_count[SLICE_TYPE_B][I_PCM];

    for (i = 0; i < h->param.i_threads; i++) {

        if (h->thread[i]->b_thread_active) {
            x264_pthread_join(h->thread[i]->thread_handle, NULL);
            assert(h->thread[i]->fenc->i_reference_count == 1);
            x264_frame_delete(h->thread[i]->fenc);
        }
    }

    for (i = 0; i < 5; i++) {
        static const int slice_order[] = {SLICE_TYPE_I, SLICE_TYPE_SI, SLICE_TYPE_P, SLICE_TYPE_SP, SLICE_TYPE_B};
        static const char *slice_name[] = {"P", "B", "I", "SP", "SI"};
        int i_slice = slice_order[i];

        if (h->stat.i_slice_count[i_slice] > 0) {
            const int i_count = h->stat.i_slice_count[i_slice];
            if (h->param.analyse.b_psnr) {
                x264_log(h, X264_LOG_INFO, "slice %s:%-5d Avg QP:%5.2f  size:%6.0f  PSNR Mean Y:%5.2f U:%5.2f V:%5.2f Avg:%5.2f Global:%5.2f\n", slice_name[i_slice], i_count, h->stat.f_slice_qp[i_slice] / i_count, (double)h->stat.i_slice_size[i_slice] / i_count, h->stat.f_psnr_mean_y[i_slice] / i_count, h->stat.f_psnr_mean_u[i_slice] / i_count, h->stat.f_psnr_mean_v[i_slice] / i_count, h->stat.f_psnr_average[i_slice] / i_count, x264_psnr(h->stat.i_ssd_global[i_slice], i_count * i_yuv_size));
            } else {
                x264_log(h, X264_LOG_INFO, "slice %s:%-5d Avg QP:%5.2f  size:%6.0f\n", slice_name[i_slice], i_count, h->stat.f_slice_qp[i_slice] / i_count, (double)h->stat.i_slice_size[i_slice] / i_count);
            }
        }
    }
    if (h->param.i_bframe && h->stat.i_slice_count[SLICE_TYPE_P]) {
        char *p = buf;
        int den = 0;

        for (i = 0; i <= h->param.i_bframe; i++) {
            den += (i + 1) * h->stat.i_consecutive_bframes[i];
        }
        for (i = 0; i <= h->param.i_bframe; i++) {
            p += sprintf(p, " %4.1f%%", 100. * (i + 1) * h->stat.i_consecutive_bframes[i] / den);
        }
        x264_log(h, X264_LOG_INFO, "consecutive B-frames:%s\n", buf);
    }

    for (i_type = 0; i_type < 2; i_type++) {
        for (i = 0; i < X264_PARTTYPE_MAX; i++) {
            if (i == D_DIRECT_8x8) {
                continue;
            }
            i_mb_count_size[i_type][x264_mb_partition_pixel_table[i]] += h->stat.i_mb_partition[i_type][i];
        }
    }

    if (h->stat.i_slice_count[SLICE_TYPE_I] > 0) {
        int64_t *i_mb_count = h->stat.i_mb_count[SLICE_TYPE_I];
        double i_count = h->stat.i_slice_count[SLICE_TYPE_I] * h->mb.i_mb_count / 100.0;
        x264_print_intra(i_mb_count, i_count, b_print_pcm, buf);
        x264_log(h, X264_LOG_INFO, "mb I  %s\n", buf);
    }
    if (h->stat.i_slice_count[SLICE_TYPE_P] > 0) {
        int64_t *i_mb_count = h->stat.i_mb_count[SLICE_TYPE_P];
        double i_count = h->stat.i_slice_count[SLICE_TYPE_P] * h->mb.i_mb_count / 100.0;
        int64_t *i_mb_size = i_mb_count_size[SLICE_TYPE_P];
        x264_print_intra(i_mb_count, i_count, b_print_pcm, buf);
        x264_log(h, X264_LOG_INFO, "mb P  %s  P16..4: %4.1f%% %4.1f%% %4.1f%% %4.1f%% %4.1f%%    skip:%4.1f%%\n", buf, i_mb_size[PIXEL_16x16] / (i_count * 4), (i_mb_size[PIXEL_16x8] + i_mb_size[PIXEL_8x16]) / (i_count * 4), i_mb_size[PIXEL_8x8] / (i_count * 4), (i_mb_size[PIXEL_8x4] + i_mb_size[PIXEL_4x8]) / (i_count * 4), i_mb_size[PIXEL_4x4] / (i_count * 4), i_mb_count[P_SKIP] / i_count);
    }
    if (h->stat.i_slice_count[SLICE_TYPE_B] > 0) {
        int64_t *i_mb_count = h->stat.i_mb_count[SLICE_TYPE_B];
        double i_count = h->stat.i_slice_count[SLICE_TYPE_B] * h->mb.i_mb_count / 100.0;
        double i_mb_list_count;
        int64_t *i_mb_size = i_mb_count_size[SLICE_TYPE_B];
        int64_t list_count[3] = {0};
        x264_print_intra(i_mb_count, i_count, b_print_pcm, buf);
        for (i = 0; i < X264_PARTTYPE_MAX; i++) {
            for (j = 0; j < 2; j++) {
                int l0 = x264_mb_type_list0_table[i][j];
                int l1 = x264_mb_type_list1_table[i][j];
                if (l0 || l1) {
                    list_count[l1 + l0 * l1] += h->stat.i_mb_count[SLICE_TYPE_B][i] * 2;
                }
            }
        }
        list_count[0] += h->stat.i_mb_partition[SLICE_TYPE_B][D_L0_8x8];
        list_count[1] += h->stat.i_mb_partition[SLICE_TYPE_B][D_L1_8x8];
        list_count[2] += h->stat.i_mb_partition[SLICE_TYPE_B][D_BI_8x8];
        i_mb_count[B_DIRECT] += (h->stat.i_mb_partition[SLICE_TYPE_B][D_DIRECT_8x8] + 2) / 4;
        i_mb_list_count = (list_count[0] + list_count[1] + list_count[2]) / 100.0;
        x264_log(h, X264_LOG_INFO, "mb B  %s  B16..8: %4.1f%% %4.1f%% %4.1f%%  direct:%4.1f%%  skip:%4.1f%%  L0:%4.1f%% L1:%4.1f%% BI:%4.1f%%\n", buf, i_mb_size[PIXEL_16x16] / (i_count * 4), (i_mb_size[PIXEL_16x8] + i_mb_size[PIXEL_8x16]) / (i_count * 4), i_mb_size[PIXEL_8x8] / (i_count * 4), i_mb_count[B_DIRECT] / i_count, i_mb_count[B_SKIP] / i_count, list_count[0] / i_mb_list_count, list_count[1] / i_mb_list_count, list_count[2] / i_mb_list_count);
    }

    x264_ratecontrol_summary(h);

    if (h->stat.i_slice_count[SLICE_TYPE_I] + h->stat.i_slice_count[SLICE_TYPE_P] + h->stat.i_slice_count[SLICE_TYPE_B] > 0) {
        const int i_count = h->stat.i_slice_count[SLICE_TYPE_I] + h->stat.i_slice_count[SLICE_TYPE_P] + h->stat.i_slice_count[SLICE_TYPE_B];
        float fps = (float)h->param.i_fps_num / h->param.i_fps_den;
#define SUM3(p) (p[SLICE_TYPE_I] + p[SLICE_TYPE_P] + p[SLICE_TYPE_B])
#define SUM3b(p, o) (p[SLICE_TYPE_I][o] + p[SLICE_TYPE_P][o] + p[SLICE_TYPE_B][o])
        float f_bitrate = fps * SUM3(h->stat.i_slice_size) / i_count / 125;

        if (h->pps->b_transform_8x8_mode) {
            int64_t i_i8x8 = SUM3b(h->stat.i_mb_count, I_8x8);
            int64_t i_intra = i_i8x8 + SUM3b(h->stat.i_mb_count, I_4x4) + SUM3b(h->stat.i_mb_count, I_16x16);
            x264_log(h, X264_LOG_INFO, "8x8 transform  intra:%.1f%%  inter:%.1f%%\n", 100. * i_i8x8 / i_intra, 100. * h->stat.i_mb_count_8x8dct[1] / h->stat.i_mb_count_8x8dct[0]);
        }

        if (h->param.analyse.i_direct_mv_pred == X264_DIRECT_PRED_AUTO && h->stat.i_slice_count[SLICE_TYPE_B]) {
            x264_log(h, X264_LOG_INFO, "direct mvs  spatial:%.1f%%  temporal:%.1f%%\n", h->stat.i_direct_frames[1] * 100. / h->stat.i_slice_count[SLICE_TYPE_B], h->stat.i_direct_frames[0] * 100. / h->stat.i_slice_count[SLICE_TYPE_B]);
        }

        for (i_list = 0; i_list < 2; i_list++) {
            int i_slice;
            for (i_slice = 0; i_slice < 2; i_slice++) {
                char *p = buf;
                int64_t i_den = 0;
                int i_max = 0;
                for (i = 0; i < 32; i++) {
                    if (h->stat.i_mb_count_ref[i_slice][i_list][i]) {
                        i_den += h->stat.i_mb_count_ref[i_slice][i_list][i];
                        i_max = i;
                    }
                }
                if (i_max == 0) {
                    continue;
                }
                for (i = 0; i <= i_max; i++) {
                    p += sprintf(p, " %4.1f%%", 100. * h->stat.i_mb_count_ref[i_slice][i_list][i] / i_den);
                }
                x264_log(h, X264_LOG_INFO, "ref %c L%d %s\n", "PB"[i_slice], i_list, buf);
            }
        }

        if (h->param.analyse.b_ssim) {
            x264_log(h, X264_LOG_INFO, "SSIM Mean Y:%.7f\n", SUM3(h->stat.f_ssim_mean_y) / i_count);
        }
        if (h->param.analyse.b_psnr) {
            x264_log(h, X264_LOG_INFO, "PSNR Mean Y:%6.3f U:%6.3f V:%6.3f Avg:%6.3f Global:%6.3f kb/s:%.2f\n", SUM3(h->stat.f_psnr_mean_y) / i_count, SUM3(h->stat.f_psnr_mean_u) / i_count, SUM3(h->stat.f_psnr_mean_v) / i_count, SUM3(h->stat.f_psnr_average) / i_count, x264_psnr(SUM3(h->stat.i_ssd_global), i_count * i_yuv_size), f_bitrate);
        } else {
            x264_log(h, X264_LOG_INFO, "kb/s:%.1f\n", f_bitrate);
        }
    }

    x264_ratecontrol_delete(h);

    if (h->param.rc.psz_stat_out) {
        free(h->param.rc.psz_stat_out);
    }
    if (h->param.rc.psz_stat_in) {
        free(h->param.rc.psz_stat_in);
    }

    x264_cqm_delete(h);

    if (h->param.i_threads > 1) {
        h = h->thread[h->i_thread_phase % h->param.i_threads];
    }

    for (i = 0; h->frames.current[i]; i++) {
        assert(h->frames.current[i]->i_reference_count == 1);
        x264_frame_delete(h->frames.current[i]);
    }
    for (i = 0; h->frames.next[i]; i++) {
        assert(h->frames.next[i]->i_reference_count == 1);
        x264_frame_delete(h->frames.next[i]);
    }
    for (i = 0; h->frames.unused[i]; i++) {
        assert(h->frames.unused[i]->i_reference_count == 0);
        x264_frame_delete(h->frames.unused[i]);
    }

    h = h->thread[0];

    for (i = h->param.i_threads - 1; i >= 0; i--) {
        x264_frame_t **frame;

        for (frame = h->thread[i]->frames.reference; *frame; frame++) {
            assert((*frame)->i_reference_count > 0);
            (*frame)->i_reference_count--;
            if ((*frame)->i_reference_count == 0) {
                x264_frame_delete(*frame);
            }
        }
        frame = &h->thread[i]->fdec;
        assert((*frame)->i_reference_count > 0);
        (*frame)->i_reference_count--;
        if ((*frame)->i_reference_count == 0) {
            x264_frame_delete(*frame);
        }

        x264_macroblock_cache_end(h->thread[i]);
        x264_free(h->thread[i]->out.p_bitstream);
        x264_free(h->thread[i]);
    }
}


#include "common/common.h"
#include "macroblock.h"

#define ZIG(i, y, x) level[i] = dct[x][y];
static inline void zigzag_scan_2x2_dc(int16_t level[4], int16_t dct[2][2]) {
    ZIG(0, 0, 0)
    ZIG(1, 0, 1)
    ZIG(2, 1, 0)
    ZIG(3, 1, 1)
}
#undef ZIG

static inline void idct_dequant_2x2_dc(int16_t dct[2][2], int16_t dct4x4[4][4][4], int dequant_mf[6][4][4], int i_qp) {
    int d0 = dct[0][0] + dct[0][1];
    int d1 = dct[1][0] + dct[1][1];
    int d2 = dct[0][0] - dct[0][1];
    int d3 = dct[1][0] - dct[1][1];
    int dmf = dequant_mf[i_qp % 6][0][0];
    int qbits = i_qp / 6 - 5;
    if (qbits > 0) {
        dmf <<= qbits;
        qbits = 0;
    }
    dct4x4[0][0][0] = (d0 + d1) * dmf >> -qbits;
    dct4x4[1][0][0] = (d0 - d1) * dmf >> -qbits;
    dct4x4[2][0][0] = (d2 + d3) * dmf >> -qbits;
    dct4x4[3][0][0] = (d2 - d3) * dmf >> -qbits;
}

static inline void dct2x2dc(int16_t d[2][2], int16_t dct4x4[4][4][4]) {
    int d0 = dct4x4[0][0][0] + dct4x4[1][0][0];
    int d1 = dct4x4[2][0][0] + dct4x4[3][0][0];
    int d2 = dct4x4[0][0][0] - dct4x4[1][0][0];
    int d3 = dct4x4[2][0][0] - dct4x4[3][0][0];
    d[0][0] = d0 + d1;
    d[1][0] = d2 + d3;
    d[0][1] = d0 - d1;
    d[1][1] = d2 - d3;
    dct4x4[0][0][0] = 0;
    dct4x4[1][0][0] = 0;
    dct4x4[2][0][0] = 0;
    dct4x4[3][0][0] = 0;
}

static ALWAYS_INLINE void x264_quant_4x4(x264_t *h, int16_t dct[4][4], int i_qp, int i_ctxBlockCat, int b_intra, int idx) {
    int i_quant_cat = b_intra ? CQM_4IY : CQM_4PY;
    if (h->mb.b_trellis) {
        x264_quant_4x4_trellis(h, dct, i_quant_cat, i_qp, i_ctxBlockCat, b_intra, idx);
    } else {
        h->quantf.quant_4x4(dct, h->quant4_mf[i_quant_cat][i_qp], h->quant4_bias[i_quant_cat][i_qp]);
    }
}

static ALWAYS_INLINE void x264_quant_8x8(x264_t *h, int16_t dct[8][8], int i_qp, int b_intra, int idx) {
    int i_quant_cat = b_intra ? CQM_8IY : CQM_8PY;
    if (h->mb.b_trellis) {
        x264_quant_8x8_trellis(h, dct, i_quant_cat, i_qp, b_intra, idx);
    } else {
        h->quantf.quant_8x8(dct, h->quant8_mf[i_quant_cat][i_qp], h->quant8_bias[i_quant_cat][i_qp]);
    }
}

void x264_mb_encode_i4x4(x264_t *h, int idx, int i_qp) {
    uint8_t *p_src = &h->mb.pic.p_fenc[0][block_idx_xy_fenc[idx]];
    uint8_t *p_dst = &h->mb.pic.p_fdec[0][block_idx_xy_fdec[idx]];
    DECLARE_ALIGNED_16(int16_t dct4x4[4][4]);

    if (h->mb.b_lossless) {
        h->zigzagf.sub_4x4(h->dct.luma4x4[idx], p_src, p_dst);
        return;
    }

    h->dctf.sub4x4_dct(dct4x4, p_src, p_dst);

    x264_quant_4x4(h, dct4x4, i_qp, DCT_LUMA_4x4, 1, idx);

    if (array_non_zero(dct4x4)) {
        h->zigzagf.scan_4x4(h->dct.luma4x4[idx], dct4x4);
        h->quantf.dequant_4x4(dct4x4, h->dequant4_mf[CQM_4IY], i_qp);

        h->dctf.add4x4_idct(p_dst, dct4x4);
    } else {
        memset(h->dct.luma4x4[idx], 0, sizeof(h->dct.luma4x4[idx]));
    }
}

void x264_mb_encode_i8x8(x264_t *h, int idx, int i_qp) {
    int x = 8 * (idx & 1);
    int y = 8 * (idx >> 1);
    uint8_t *p_src = &h->mb.pic.p_fenc[0][x + y * FENC_STRIDE];
    uint8_t *p_dst = &h->mb.pic.p_fdec[0][x + y * FDEC_STRIDE];
    DECLARE_ALIGNED_16(int16_t dct8x8[8][8]);

    if (h->mb.b_lossless) {
        h->zigzagf.sub_8x8(h->dct.luma8x8[idx], p_src, p_dst);
        return;
    }

    h->dctf.sub8x8_dct8(dct8x8, p_src, p_dst);

    x264_quant_8x8(h, dct8x8, i_qp, 1, idx);

    h->zigzagf.scan_8x8(h->dct.luma8x8[idx], dct8x8);
    h->quantf.dequant_8x8(dct8x8, h->dequant8_mf[CQM_8IY], i_qp);
    h->dctf.add8x8_idct8(p_dst, dct8x8);
}

static void x264_mb_encode_i16x16(x264_t *h, int i_qp) {
    uint8_t *p_src = h->mb.pic.p_fenc[0];
    uint8_t *p_dst = h->mb.pic.p_fdec[0];

    DECLARE_ALIGNED_16(int16_t dct4x4[16][4][4]);
    DECLARE_ALIGNED_16(int16_t dct_dc4x4[4][4]);

    int i;

    if (h->mb.b_lossless) {
        for (i = 0; i < 16; i++) {
            int oe = block_idx_xy_fenc[i];
            int od = block_idx_xy_fdec[i];
            h->zigzagf.sub_4x4(h->dct.luma4x4[i], p_src + oe, p_dst + od);
            dct_dc4x4[0][block_idx_yx_1d[i]] = h->dct.luma4x4[i][0];
            h->dct.luma4x4[i][0] = 0;
        }
        h->zigzagf.scan_4x4(h->dct.luma16x16_dc, dct_dc4x4);
        return;
    }

    h->dctf.sub16x16_dct(dct4x4, p_src, p_dst);
    for (i = 0; i < 16; i++) {

        dct_dc4x4[0][block_idx_xy_1d[i]] = dct4x4[i][0][0];
        dct4x4[i][0][0] = 0;

        x264_quant_4x4(h, dct4x4[i], i_qp, DCT_LUMA_AC, 1, i);

        h->zigzagf.scan_4x4(h->dct.luma4x4[i], dct4x4[i]);
        h->quantf.dequant_4x4(dct4x4[i], h->dequant4_mf[CQM_4IY], i_qp);
    }

    h->dctf.dct4x4dc(dct_dc4x4);
    if (h->mb.b_trellis) {
        x264_quant_dc_trellis(h, (int16_t *)dct_dc4x4, CQM_4IY, i_qp, DCT_LUMA_DC, 1);
    } else {
        h->quantf.quant_4x4_dc(dct_dc4x4, h->quant4_mf[CQM_4IY][i_qp][0] >> 1, h->quant4_bias[CQM_4IY][i_qp][0] << 1);
    }
    h->zigzagf.scan_4x4(h->dct.luma16x16_dc, dct_dc4x4);

    h->dctf.idct4x4dc(dct_dc4x4);
    h->quantf.dequant_4x4_dc(dct_dc4x4, h->dequant4_mf[CQM_4IY], i_qp);

    for (i = 0; i < 16; i++) {

        dct4x4[i][0][0] = dct_dc4x4[0][block_idx_xy_1d[i]];
    }

    h->dctf.add16x16_idct(p_dst, dct4x4);
}

void x264_mb_encode_8x8_chroma(x264_t *h, int b_inter, int i_qp) {
    int i, ch;
    int b_decimate = b_inter && (h->sh.i_type == SLICE_TYPE_B || h->param.analyse.b_dct_decimate);

    for (ch = 0; ch < 2; ch++) {
        uint8_t *p_src = h->mb.pic.p_fenc[1 + ch];
        uint8_t *p_dst = h->mb.pic.p_fdec[1 + ch];
        int i_decimate_score = 0;

        DECLARE_ALIGNED_16(int16_t dct2x2[2][2]);
        DECLARE_ALIGNED_16(int16_t dct4x4[4][4][4]);

        if (h->mb.b_lossless) {
            for (i = 0; i < 4; i++) {
                int oe = block_idx_x[i] * 4 + block_idx_y[i] * 4 * FENC_STRIDE;
                int od = block_idx_x[i] * 4 + block_idx_y[i] * 4 * FDEC_STRIDE;
                h->zigzagf.sub_4x4(h->dct.luma4x4[16 + i + ch * 4], p_src + oe, p_dst + od);
                h->dct.chroma_dc[ch][i] = h->dct.luma4x4[16 + i + ch * 4][0];
                h->dct.luma4x4[16 + i + ch * 4][0] = 0;
            }
            continue;
        }

        h->dctf.sub8x8_dct(dct4x4, p_src, p_dst);
        dct2x2dc(dct2x2, dct4x4);

        for (i = 0; i < 4; i++) {
            if (h->mb.b_trellis) {
                x264_quant_4x4_trellis(h, dct4x4[i], CQM_4IC + b_inter, i_qp, DCT_CHROMA_AC, !b_inter, 0);
            } else {
                h->quantf.quant_4x4(dct4x4[i], h->quant4_mf[CQM_4IC + b_inter][i_qp], h->quant4_bias[CQM_4IC + b_inter][i_qp]);
            }
            h->zigzagf.scan_4x4(h->dct.luma4x4[16 + i + ch * 4], dct4x4[i]);

            if (b_decimate) {
                i_decimate_score += h->quantf.decimate_score15(h->dct.luma4x4[16 + i + ch * 4]);
            }
        }

        if (h->mb.b_trellis) {
            x264_quant_dc_trellis(h, (int16_t *)dct2x2, CQM_4IC + b_inter, i_qp, DCT_CHROMA_DC, !b_inter);
        } else {
            h->quantf.quant_2x2_dc(dct2x2, h->quant4_mf[CQM_4IC + b_inter][i_qp][0] >> 1, h->quant4_bias[CQM_4IC + b_inter][i_qp][0] << 1);
        }

        if (b_decimate && i_decimate_score < 7) {

            memset(&h->dct.luma4x4[16 + ch * 4], 0, 4 * sizeof(*h->dct.luma4x4));
            if (!array_non_zero(dct2x2)) {
                memset(h->dct.chroma_dc[ch], 0, sizeof(h->dct.chroma_dc[ch]));
                continue;
            }
            memset(dct4x4, 0, sizeof(dct4x4));
        } else {
            for (i = 0; i < 4; i++) {
                h->quantf.dequant_4x4(dct4x4[i], h->dequant4_mf[CQM_4IC + b_inter], i_qp);
            }
        }

        zigzag_scan_2x2_dc(h->dct.chroma_dc[ch], dct2x2);
        idct_dequant_2x2_dc(dct2x2, dct4x4, h->dequant4_mf[CQM_4IC + b_inter], i_qp);
        h->dctf.add8x8_idct(p_dst, dct4x4);
    }

    h->mb.i_cbp_chroma = 0;
    for (i = 0; i < 8; i++) {
        int nz = array_non_zero(h->dct.luma4x4[16 + i]);
        h->mb.cache.non_zero_count[x264_scan8[16 + i]] = nz;
        h->mb.i_cbp_chroma |= nz;
    }
    h->mb.cache.non_zero_count[x264_scan8[25]] = array_non_zero(h->dct.chroma_dc[0]);
    h->mb.cache.non_zero_count[x264_scan8[26]] = array_non_zero(h->dct.chroma_dc[1]);
    if (h->mb.i_cbp_chroma) {
        h->mb.i_cbp_chroma = 2;
    } else if (h->mb.cache.non_zero_count[x264_scan8[25]] | h->mb.cache.non_zero_count[x264_scan8[26]]) {
        h->mb.i_cbp_chroma = 1;
    }
}

static void x264_macroblock_encode_skip(x264_t *h) {
    h->mb.i_cbp_luma = 0x00;
    h->mb.i_cbp_chroma = 0x00;
    memset(h->mb.cache.non_zero_count, 0, X264_SCAN8_SIZE);

    h->mb.cbp[h->mb.i_mb_xy] = 0;
}

static void x264_macroblock_encode_pskip(x264_t *h) {
    const int mvx = x264_clip3(h->mb.cache.mv[0][x264_scan8[0]][0], h->mb.mv_min[0], h->mb.mv_max[0]);
    const int mvy = x264_clip3(h->mb.cache.mv[0][x264_scan8[0]][1], h->mb.mv_min[1], h->mb.mv_max[1]);

    if (!h->mb.b_skip_mc) {
        h->mc.mc_luma(h->mb.pic.p_fdec[0], FDEC_STRIDE, h->mb.pic.p_fref[0][0], h->mb.pic.i_stride[0], mvx, mvy, 16, 16);

        h->mc.mc_chroma(h->mb.pic.p_fdec[1], FDEC_STRIDE, h->mb.pic.p_fref[0][0][4], h->mb.pic.i_stride[1], mvx, mvy, 8, 8);

        h->mc.mc_chroma(h->mb.pic.p_fdec[2], FDEC_STRIDE, h->mb.pic.p_fref[0][0][5], h->mb.pic.i_stride[2], mvx, mvy, 8, 8);
    }

    x264_macroblock_encode_skip(h);
}

void x264_predict_lossless_8x8_chroma(x264_t *h, int i_mode) {
    int stride = h->fenc->i_stride[1] << h->mb.b_interlaced;
    if (i_mode == I_PRED_CHROMA_V) {
        h->mc.copy[PIXEL_8x8](h->mb.pic.p_fdec[1], FDEC_STRIDE, h->mb.pic.p_fenc_plane[1] - stride, stride, 8);
        h->mc.copy[PIXEL_8x8](h->mb.pic.p_fdec[2], FDEC_STRIDE, h->mb.pic.p_fenc_plane[2] - stride, stride, 8);
    } else if (i_mode == I_PRED_CHROMA_H) {
        h->mc.copy[PIXEL_8x8](h->mb.pic.p_fdec[1], FDEC_STRIDE, h->mb.pic.p_fenc_plane[1] - 1, stride, 8);
        h->mc.copy[PIXEL_8x8](h->mb.pic.p_fdec[2], FDEC_STRIDE, h->mb.pic.p_fenc_plane[2] - 1, stride, 8);
    } else {
        h->predict_8x8c[i_mode](h->mb.pic.p_fdec[1]);
        h->predict_8x8c[i_mode](h->mb.pic.p_fdec[2]);
    }
}

void x264_predict_lossless_4x4(x264_t *h, uint8_t *p_dst, int idx, int i_mode) {
    int stride = h->fenc->i_stride[0] << h->mb.b_interlaced;
    uint8_t *p_src = h->mb.pic.p_fenc_plane[0] + block_idx_x[idx] * 4 + block_idx_y[idx] * 4 * stride;

    if (i_mode == I_PRED_4x4_V) {
        h->mc.copy[PIXEL_4x4](p_dst, FDEC_STRIDE, p_src - stride, stride, 4);
    } else if (i_mode == I_PRED_4x4_H) {
        h->mc.copy[PIXEL_4x4](p_dst, FDEC_STRIDE, p_src - 1, stride, 4);
    } else {
        h->predict_4x4[i_mode](p_dst);
    }
}

void x264_predict_lossless_8x8(x264_t *h, uint8_t *p_dst, int idx, int i_mode, uint8_t edge[33]) {
    int stride = h->fenc->i_stride[0] << h->mb.b_interlaced;
    uint8_t *p_src = h->mb.pic.p_fenc_plane[0] + (idx & 1) * 8 + (idx >> 1) * 8 * stride;

    if (i_mode == I_PRED_8x8_V) {
        h->mc.copy[PIXEL_8x8](p_dst, FDEC_STRIDE, p_src - stride, stride, 8);
    } else if (i_mode == I_PRED_8x8_H) {
        h->mc.copy[PIXEL_8x8](p_dst, FDEC_STRIDE, p_src - 1, stride, 8);
    } else {
        h->predict_8x8[i_mode](p_dst, edge);
    }
}

void x264_predict_lossless_16x16(x264_t *h, int i_mode) {
    int stride = h->fenc->i_stride[0] << h->mb.b_interlaced;
    if (i_mode == I_PRED_16x16_V) {
        h->mc.copy[PIXEL_16x16](h->mb.pic.p_fdec[0], FDEC_STRIDE, h->mb.pic.p_fenc_plane[0] - stride, stride, 16);
    } else if (i_mode == I_PRED_16x16_H) {
        h->mc.copy_16x16_unaligned(h->mb.pic.p_fdec[0], FDEC_STRIDE, h->mb.pic.p_fenc_plane[0] - 1, stride, 16);
    } else {
        h->predict_16x16[i_mode](h->mb.pic.p_fdec[0]);
    }
}

void x264_macroblock_encode(x264_t *h) {
    int i_cbp_dc = 0;
    int i_qp = h->mb.i_qp;
    int b_decimate = h->sh.i_type == SLICE_TYPE_B || h->param.analyse.b_dct_decimate;
    int b_force_no_skip = 0;
    int i, j, idx;
    uint8_t nnz8x8[4] = {1, 1, 1, 1};

    if (h->sh.b_mbaff && h->mb.i_mb_xy == h->sh.i_first_mb + h->mb.i_mb_stride && IS_SKIP(h->mb.type[h->sh.i_first_mb])) {

        b_force_no_skip = 1;
        if (IS_SKIP(h->mb.i_type)) {
            if (h->mb.i_type == P_SKIP) {
                h->mb.i_type = P_L0;
            } else if (h->mb.i_type == B_SKIP) {
                h->mb.i_type = B_DIRECT;
            }
        }
    }

    if (h->mb.i_type == P_SKIP) {

        x264_macroblock_encode_pskip(h);
        return;
    }
    if (h->mb.i_type == B_SKIP) {

        if (!h->mb.b_skip_mc) {
            x264_mb_mc(h);
        }
        x264_macroblock_encode_skip(h);
        return;
    }

    if (h->mb.i_type == I_16x16) {
        const int i_mode = h->mb.i_intra16x16_pred_mode;
        h->mb.b_transform_8x8 = 0;

        if (h->mb.b_lossless) {
            x264_predict_lossless_16x16(h, i_mode);
        } else {
            h->predict_16x16[i_mode](h->mb.pic.p_fdec[0]);
        }

        x264_mb_encode_i16x16(h, i_qp);
    } else if (h->mb.i_type == I_8x8) {
        DECLARE_ALIGNED_16(uint8_t edge[33]);
        h->mb.b_transform_8x8 = 1;

        if (h->mb.i_skip_intra) {
            h->mc.copy[PIXEL_16x16](h->mb.pic.p_fdec[0], FDEC_STRIDE, h->mb.pic.i8x8_fdec_buf, 16, 16);

            if (h->mb.i_skip_intra == 2) {
                h->mc.memcpy_aligned(h->dct.luma8x8, h->mb.pic.i8x8_dct_buf, sizeof(h->mb.pic.i8x8_dct_buf));
            }
        }
        for (i = h->mb.i_skip_intra ? 3 : 0; i < 4; i++) {
            uint8_t *p_dst = &h->mb.pic.p_fdec[0][8 * (i & 1) + 8 * (i >> 1) * FDEC_STRIDE];
            int i_mode = h->mb.cache.intra4x4_pred_mode[x264_scan8[4 * i]];
            x264_predict_8x8_filter(p_dst, edge, h->mb.i_neighbour8[i], x264_pred_i4x4_neighbors[i_mode]);

            if (h->mb.b_lossless) {
                x264_predict_lossless_8x8(h, p_dst, i, i_mode, edge);
            } else {
                h->predict_8x8[i_mode](p_dst, edge);
            }

            x264_mb_encode_i8x8(h, i, i_qp);
        }
        for (i = 0; i < 4; i++) {
            nnz8x8[i] = array_non_zero(h->dct.luma8x8[i]);
        }
    } else if (h->mb.i_type == I_4x4) {
        h->mb.b_transform_8x8 = 0;

        if (h->mb.i_skip_intra) {
            h->mc.copy[PIXEL_16x16](h->mb.pic.p_fdec[0], FDEC_STRIDE, h->mb.pic.i4x4_fdec_buf, 16, 16);

            if (h->mb.i_skip_intra == 2) {
                h->mc.memcpy_aligned(h->dct.luma4x4, h->mb.pic.i4x4_dct_buf, sizeof(h->mb.pic.i4x4_dct_buf));
            }
        }
        for (i = h->mb.i_skip_intra ? 15 : 0; i < 16; i++) {
            uint8_t *p_dst = &h->mb.pic.p_fdec[0][block_idx_xy_fdec[i]];
            int i_mode = h->mb.cache.intra4x4_pred_mode[x264_scan8[i]];

            if ((h->mb.i_neighbour4[i] & (MB_TOPRIGHT | MB_TOP)) == MB_TOP) {

                *(uint32_t *)&p_dst[4 - FDEC_STRIDE] = p_dst[3 - FDEC_STRIDE] * 0x01010101U;
            }

            if (h->mb.b_lossless) {
                x264_predict_lossless_4x4(h, p_dst, i, i_mode);
            } else {
                h->predict_4x4[i_mode](p_dst);
            }
            x264_mb_encode_i4x4(h, i, i_qp);
        }
    } else {
        int i8x8, i4x4;
        int i_decimate_mb = 0;

        if (!h->mb.b_skip_mc) {
            x264_mb_mc(h);
        }

        if (h->mb.b_lossless) {
            if (h->mb.b_transform_8x8) {
                for (i8x8 = 0; i8x8 < 4; i8x8++) {
                    int x = 8 * (i8x8 & 1);
                    int y = 8 * (i8x8 >> 1);
                    h->zigzagf.sub_8x8(h->dct.luma8x8[i8x8], h->mb.pic.p_fenc[0] + x + y * FENC_STRIDE, h->mb.pic.p_fdec[0] + x + y * FDEC_STRIDE);
                    nnz8x8[i8x8] = array_non_zero(h->dct.luma8x8[i8x8]);
                }
            } else {
                for (i4x4 = 0; i4x4 < 16; i4x4++) {
                    h->zigzagf.sub_4x4(h->dct.luma4x4[i4x4], h->mb.pic.p_fenc[0] + block_idx_xy_fenc[i4x4], h->mb.pic.p_fdec[0] + block_idx_xy_fdec[i4x4]);
                }
            }
        } else if (h->mb.b_transform_8x8) {
            DECLARE_ALIGNED_16(int16_t dct8x8[4][8][8]);
            b_decimate &= !h->mb.b_trellis;
            h->dctf.sub16x16_dct8(dct8x8, h->mb.pic.p_fenc[0], h->mb.pic.p_fdec[0]);
            h->nr_count[1] += h->mb.b_noise_reduction * 4;

            for (idx = 0; idx < 4; idx++) {
                if (h->mb.b_noise_reduction) {
                    h->quantf.denoise_dct(*dct8x8[idx], h->nr_residual_sum[1], h->nr_offset[1], 64);
                }
                x264_quant_8x8(h, dct8x8[idx], i_qp, 0, idx);

                h->zigzagf.scan_8x8(h->dct.luma8x8[idx], dct8x8[idx]);

                if (b_decimate) {
                    int i_decimate_8x8 = h->quantf.decimate_score64(h->dct.luma8x8[idx]);
                    i_decimate_mb += i_decimate_8x8;
                    if (i_decimate_8x8 < 4) {
                        nnz8x8[idx] = 0;
                    }
                } else {
                    nnz8x8[idx] = array_non_zero(dct8x8[idx]);
                }
            }

            if (i_decimate_mb < 6 && b_decimate) {
                *(uint32_t *)nnz8x8 = 0;
            } else {
                for (idx = 0; idx < 4; idx++) {
                    if (nnz8x8[idx]) {
                        h->quantf.dequant_8x8(dct8x8[idx], h->dequant8_mf[CQM_8PY], i_qp);
                        h->dctf.add8x8_idct8(&h->mb.pic.p_fdec[0][(idx & 1) * 8 + (idx >> 1) * 8 * FDEC_STRIDE], dct8x8[idx]);
                    }
                }
            }
        } else {
            DECLARE_ALIGNED_16(int16_t dct4x4[16][4][4]);
            h->dctf.sub16x16_dct(dct4x4, h->mb.pic.p_fenc[0], h->mb.pic.p_fdec[0]);
            h->nr_count[0] += h->mb.b_noise_reduction * 16;

            for (i8x8 = 0; i8x8 < 4; i8x8++) {
                int i_decimate_8x8;

                i_decimate_8x8 = 0;
                for (i4x4 = 0; i4x4 < 4; i4x4++) {
                    idx = i8x8 * 4 + i4x4;

                    if (h->mb.b_noise_reduction) {
                        h->quantf.denoise_dct(*dct4x4[idx], h->nr_residual_sum[0], h->nr_offset[0], 16);
                    }
                    x264_quant_4x4(h, dct4x4[idx], i_qp, DCT_LUMA_4x4, 0, idx);

                    h->zigzagf.scan_4x4(h->dct.luma4x4[idx], dct4x4[idx]);

                    if (b_decimate && i_decimate_8x8 <= 6) {
                        i_decimate_8x8 += h->quantf.decimate_score16(h->dct.luma4x4[idx]);
                    }
                }

                i_decimate_mb += i_decimate_8x8;
                if (i_decimate_8x8 < 4 && b_decimate) {
                    nnz8x8[i8x8] = 0;
                }
            }

            if (i_decimate_mb < 6 && b_decimate) {
                *(uint32_t *)nnz8x8 = 0;
            } else {
                for (i8x8 = 0; i8x8 < 4; i8x8++) {
                    if (nnz8x8[i8x8]) {
                        for (i = 0; i < 4; i++) {
                            h->quantf.dequant_4x4(dct4x4[i8x8 * 4 + i], h->dequant4_mf[CQM_4PY], i_qp);
                        }
                        h->dctf.add8x8_idct(&h->mb.pic.p_fdec[0][(i8x8 & 1) * 8 + (i8x8 >> 1) * 8 * FDEC_STRIDE], &dct4x4[i8x8 * 4]);
                    }
                }
            }
        }
    }

    if (IS_INTRA(h->mb.i_type)) {
        const int i_mode = h->mb.i_chroma_pred_mode;
        if (h->mb.b_lossless) {
            x264_predict_lossless_8x8_chroma(h, i_mode);
        } else {
            h->predict_8x8c[i_mode](h->mb.pic.p_fdec[1]);
            h->predict_8x8c[i_mode](h->mb.pic.p_fdec[2]);
        }
    }

    x264_mb_encode_8x8_chroma(h, !IS_INTRA(h->mb.i_type), h->mb.i_chroma_qp);

    h->mb.i_cbp_luma = 0x00;
    if (h->mb.i_type == I_16x16) {
        for (i = 0; i < 16; i++) {
            int nz = array_non_zero(h->dct.luma4x4[i]);
            h->mb.cache.non_zero_count[x264_scan8[i]] = nz;
            h->mb.i_cbp_luma |= nz;
        }
        h->mb.i_cbp_luma *= 0xf;
        h->mb.cache.non_zero_count[x264_scan8[24]] = array_non_zero(h->dct.luma16x16_dc);
    } else {
        for (i = 0; i < 4; i++) {
            if (!nnz8x8[i]) {
                *(uint16_t *)&h->mb.cache.non_zero_count[x264_scan8[0 + i * 4]] = 0;
                *(uint16_t *)&h->mb.cache.non_zero_count[x264_scan8[2 + i * 4]] = 0;
            } else if (h->mb.b_transform_8x8) {
                *(uint16_t *)&h->mb.cache.non_zero_count[x264_scan8[0 + 4 * i]] = nnz8x8[i] * 0x0101;
                *(uint16_t *)&h->mb.cache.non_zero_count[x264_scan8[2 + 4 * i]] = nnz8x8[i] * 0x0101;
                h->mb.i_cbp_luma |= nnz8x8[i] << i;
            } else {
                int nz, cbp = 0;
                for (j = 0; j < 4; j++) {
                    nz = array_non_zero(h->dct.luma4x4[j + 4 * i]);
                    h->mb.cache.non_zero_count[x264_scan8[j + 4 * i]] = nz;
                    cbp |= nz;
                }
                h->mb.i_cbp_luma |= cbp << i;
            }
        }
        h->mb.cache.non_zero_count[x264_scan8[24]] = 0;
    }

    if (h->param.b_cabac) {
        i_cbp_dc = h->mb.cache.non_zero_count[x264_scan8[24]] | h->mb.cache.non_zero_count[x264_scan8[25]] << 1 | h->mb.cache.non_zero_count[x264_scan8[26]] << 2;
    }

    h->mb.cbp[h->mb.i_mb_xy] = (i_cbp_dc << 8) | (h->mb.i_cbp_chroma << 4) | h->mb.i_cbp_luma;

    if (!b_force_no_skip) {
        if (h->mb.i_type == P_L0 && h->mb.i_partition == D_16x16 && !(h->mb.i_cbp_luma | h->mb.i_cbp_chroma) && *(uint32_t *)h->mb.cache.mv[0][x264_scan8[0]] == *(uint32_t *)h->mb.cache.pskip_mv && h->mb.cache.ref[0][x264_scan8[0]] == 0) {
            h->mb.i_type = P_SKIP;
        }

        if (h->mb.i_type == B_DIRECT && !(h->mb.i_cbp_luma | h->mb.i_cbp_chroma)) {
            h->mb.i_type = B_SKIP;
        }
    }
}

int x264_macroblock_probe_skip(x264_t *h, int b_bidir) {
    DECLARE_ALIGNED_16(int16_t dct4x4[4][4][4]);
    DECLARE_ALIGNED_16(int16_t dct2x2[2][2]);
    DECLARE_ALIGNED_16(int16_t dctscan[16]);

    int i_qp = h->mb.i_qp;
    int mvp[2];
    int ch, thresh;

    int i8x8, i4x4;
    int i_decimate_mb;

    if (!b_bidir) {

        mvp[0] = x264_clip3(h->mb.cache.pskip_mv[0], h->mb.mv_min[0], h->mb.mv_max[0]);
        mvp[1] = x264_clip3(h->mb.cache.pskip_mv[1], h->mb.mv_min[1], h->mb.mv_max[1]);

        h->mc.mc_luma(h->mb.pic.p_fdec[0], FDEC_STRIDE, h->mb.pic.p_fref[0][0], h->mb.pic.i_stride[0], mvp[0], mvp[1], 16, 16);
    }

    for (i8x8 = 0, i_decimate_mb = 0; i8x8 < 4; i8x8++) {
        int fenc_offset = (i8x8 & 1) * 8 + (i8x8 >> 1) * FENC_STRIDE * 8;
        int fdec_offset = (i8x8 & 1) * 8 + (i8x8 >> 1) * FDEC_STRIDE * 8;

        h->dctf.sub8x8_dct(dct4x4, h->mb.pic.p_fenc[0] + fenc_offset, h->mb.pic.p_fdec[0] + fdec_offset);

        for (i4x4 = 0; i4x4 < 4; i4x4++) {
            h->quantf.quant_4x4(dct4x4[i4x4], h->quant4_mf[CQM_4PY][i_qp], h->quant4_bias[CQM_4PY][i_qp]);
            if (!array_non_zero(dct4x4[i4x4])) {
                continue;
            }
            h->zigzagf.scan_4x4(dctscan, dct4x4[i4x4]);
            i_decimate_mb += h->quantf.decimate_score16(dctscan);
            if (i_decimate_mb >= 6) {
                return 0;
            }
        }
    }

    i_qp = h->mb.i_chroma_qp;
    thresh = (x264_lambda2_tab[i_qp] + 32) >> 6;

    for (ch = 0; ch < 2; ch++) {
        uint8_t *p_src = h->mb.pic.p_fenc[1 + ch];
        uint8_t *p_dst = h->mb.pic.p_fdec[1 + ch];

        if (!b_bidir) {
            h->mc.mc_chroma(h->mb.pic.p_fdec[1 + ch], FDEC_STRIDE, h->mb.pic.p_fref[0][0][4 + ch], h->mb.pic.i_stride[1 + ch], mvp[0], mvp[1], 8, 8);
        }

        if (h->pixf.ssd[PIXEL_8x8](p_dst, FDEC_STRIDE, p_src, FENC_STRIDE) < thresh) {
            continue;
        }

        h->dctf.sub8x8_dct(dct4x4, p_src, p_dst);

        dct2x2dc(dct2x2, dct4x4);
        h->quantf.quant_2x2_dc(dct2x2, h->quant4_mf[CQM_4PC][i_qp][0] >> 1, h->quant4_bias[CQM_4PC][i_qp][0] << 1);
        if (array_non_zero(dct2x2)) {
            return 0;
        }

        for (i4x4 = 0, i_decimate_mb = 0; i4x4 < 4; i4x4++) {
            h->quantf.quant_4x4(dct4x4[i4x4], h->quant4_mf[CQM_4PC][i_qp], h->quant4_bias[CQM_4PC][i_qp]);
            if (!array_non_zero(dct4x4[i4x4])) {
                continue;
            }
            h->zigzagf.scan_4x4(dctscan, dct4x4[i4x4]);
            i_decimate_mb += h->quantf.decimate_score15(dctscan);
            if (i_decimate_mb >= 7) {
                return 0;
            }
        }
    }

    h->mb.b_skip_mc = 1;
    return 1;
}

void x264_noise_reduction_update(x264_t *h) {
    int cat, i;
    for (cat = 0; cat < 2; cat++) {
        int size = cat ? 64 : 16;
        const uint16_t *weight = cat ? x264_dct8_weight2_tab : x264_dct4_weight2_tab;

        if (h->nr_count[cat] > (cat ? (1 << 16) : (1 << 18))) {
            for (i = 0; i < size; i++) {
                h->nr_residual_sum[cat][i] >>= 1;
            }
            h->nr_count[cat] >>= 1;
        }

        for (i = 0; i < size; i++) {
            h->nr_offset[cat][i] = ((uint64_t)h->param.analyse.i_noise_reduction * h->nr_count[cat] + h->nr_residual_sum[cat][i] / 2) / ((uint64_t)h->nr_residual_sum[cat][i] * weight[i] / 256 + 1);
        }
    }
}

void x264_macroblock_encode_p8x8(x264_t *h, int i8) {
    int i_qp = h->mb.i_qp;
    uint8_t *p_fenc = h->mb.pic.p_fenc[0] + (i8 & 1) * 8 + (i8 >> 1) * 8 * FENC_STRIDE;
    uint8_t *p_fdec = h->mb.pic.p_fdec[0] + (i8 & 1) * 8 + (i8 >> 1) * 8 * FDEC_STRIDE;
    int b_decimate = h->sh.i_type == SLICE_TYPE_B || h->param.analyse.b_dct_decimate;
    int nnz8x8 = 0;
    int ch;

    x264_mb_mc_8x8(h, i8);

    if (h->mb.b_lossless) {
        int i4;
        if (h->mb.b_transform_8x8) {
            h->zigzagf.sub_8x8(h->dct.luma8x8[i8], p_fenc, p_fdec);
            nnz8x8 = array_non_zero(h->dct.luma8x8[i8]);
        } else {
            for (i4 = i8 * 4; i4 < i8 * 4 + 4; i4++) {
                h->zigzagf.sub_4x4(h->dct.luma4x4[i4], h->mb.pic.p_fenc[0] + block_idx_xy_fenc[i4], h->mb.pic.p_fdec[0] + block_idx_xy_fdec[i4]);
                nnz8x8 |= array_non_zero(h->dct.luma4x4[i4]);
            }
        }
        for (ch = 0; ch < 2; ch++) {
            p_fenc = h->mb.pic.p_fenc[1 + ch] + (i8 & 1) * 4 + (i8 >> 1) * 4 * FENC_STRIDE;
            p_fdec = h->mb.pic.p_fdec[1 + ch] + (i8 & 1) * 4 + (i8 >> 1) * 4 * FDEC_STRIDE;
            h->zigzagf.sub_4x4(h->dct.luma4x4[16 + i8 + ch * 4], p_fenc, p_fdec);
            h->dct.luma4x4[16 + i8 + ch * 4][0] = 0;
        }
    } else {
        if (h->mb.b_transform_8x8) {
            DECLARE_ALIGNED_16(int16_t dct8x8[8][8]);
            h->dctf.sub8x8_dct8(dct8x8, p_fenc, p_fdec);
            x264_quant_8x8(h, dct8x8, i_qp, 0, i8);
            h->zigzagf.scan_8x8(h->dct.luma8x8[i8], dct8x8);

            if (b_decimate && !h->mb.b_trellis) {
                nnz8x8 = 4 <= h->quantf.decimate_score64(h->dct.luma8x8[i8]);
            } else {
                nnz8x8 = array_non_zero(dct8x8);
            }

            if (nnz8x8) {
                h->quantf.dequant_8x8(dct8x8, h->dequant8_mf[CQM_8PY], i_qp);
                h->dctf.add8x8_idct8(p_fdec, dct8x8);
            }
        } else {
            int i4;
            DECLARE_ALIGNED_16(int16_t dct4x4[4][4][4]);
            h->dctf.sub8x8_dct(dct4x4, p_fenc, p_fdec);
            for (i4 = 0; i4 < 4; i4++) {
                x264_quant_4x4(h, dct4x4[i4], i_qp, DCT_LUMA_4x4, 0, i8 * 4 + i4);
            }

            for (i4 = 0; i4 < 4; i4++) {
                h->zigzagf.scan_4x4(h->dct.luma4x4[i8 * 4 + i4], dct4x4[i4]);
            }

            if (b_decimate) {
                int i_decimate_8x8 = 0;
                for (i4 = 0; i4 < 4 && i_decimate_8x8 < 4; i4++) {
                    i_decimate_8x8 += h->quantf.decimate_score16(h->dct.luma4x4[i8 * 4 + i4]);
                }
                nnz8x8 = 4 <= i_decimate_8x8;
            } else {
                nnz8x8 = array_non_zero(dct4x4);
            }

            if (nnz8x8) {
                for (i4 = 0; i4 < 4; i4++) {
                    h->quantf.dequant_4x4(dct4x4[i4], h->dequant4_mf[CQM_4PY], i_qp);
                }
                h->dctf.add8x8_idct(p_fdec, dct4x4);
            }
        }

        i_qp = h->mb.i_chroma_qp;

        for (ch = 0; ch < 2; ch++) {
            DECLARE_ALIGNED_16(int16_t dct4x4[4][4]);
            p_fenc = h->mb.pic.p_fenc[1 + ch] + (i8 & 1) * 4 + (i8 >> 1) * 4 * FENC_STRIDE;
            p_fdec = h->mb.pic.p_fdec[1 + ch] + (i8 & 1) * 4 + (i8 >> 1) * 4 * FDEC_STRIDE;

            h->dctf.sub4x4_dct(dct4x4, p_fenc, p_fdec);
            dct4x4[0][0] = 0;

            if (h->mb.b_trellis) {
                x264_quant_4x4_trellis(h, dct4x4, CQM_4PC, i_qp, DCT_CHROMA_AC, 0, 0);
            } else {
                h->quantf.quant_4x4(dct4x4, h->quant4_mf[CQM_4PC][i_qp], h->quant4_bias[CQM_4PC][i_qp]);
            }

            h->zigzagf.scan_4x4(h->dct.luma4x4[16 + i8 + ch * 4], dct4x4);
            if (array_non_zero(dct4x4)) {
                h->quantf.dequant_4x4(dct4x4, h->dequant4_mf[CQM_4PC], i_qp);
                h->dctf.add4x4_idct(p_fdec, dct4x4);
            }
        }
    }
    h->mb.i_cbp_luma &= ~(1 << i8);
    h->mb.i_cbp_luma |= nnz8x8 << i8;
    h->mb.i_cbp_chroma = 0x02;
}

void x264_macroblock_encode_p4x4(x264_t *h, int i4) {
    int i_qp = h->mb.i_qp;
    uint8_t *p_fenc = &h->mb.pic.p_fenc[0][block_idx_xy_fenc[i4]];
    uint8_t *p_fdec = &h->mb.pic.p_fdec[0][block_idx_xy_fdec[i4]];
    const int i_ref = h->mb.cache.ref[0][x264_scan8[i4]];
    const int mvx = x264_clip3(h->mb.cache.mv[0][x264_scan8[i4]][0], h->mb.mv_min[0], h->mb.mv_max[0]);
    const int mvy = x264_clip3(h->mb.cache.mv[0][x264_scan8[i4]][1], h->mb.mv_min[1], h->mb.mv_max[1]);

    h->mc.mc_luma(p_fdec, FDEC_STRIDE, h->mb.pic.p_fref[0][i_ref], h->mb.pic.i_stride[0], mvx + 4 * 4 * block_idx_x[i4], mvy + 4 * 4 * block_idx_y[i4], 4, 4);

    if (h->mb.b_lossless) {
        h->zigzagf.sub_4x4(h->dct.luma4x4[i4], p_fenc, p_fdec);
    } else {
        DECLARE_ALIGNED_16(int16_t dct4x4[4][4]);
        h->dctf.sub4x4_dct(dct4x4, p_fenc, p_fdec);
        x264_quant_4x4(h, dct4x4, i_qp, DCT_LUMA_4x4, 0, i4);
        h->zigzagf.scan_4x4(h->dct.luma4x4[i4], dct4x4);
        if (array_non_zero(dct4x4)) {
            h->quantf.dequant_4x4(dct4x4, h->dequant4_mf[CQM_4PY], i_qp);
            h->dctf.add4x4_idct(p_fdec, dct4x4);
        }
    }
}

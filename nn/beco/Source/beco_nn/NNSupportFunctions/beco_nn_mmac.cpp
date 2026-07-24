/***************************************************************************
 *
 * Copyright 2015-2025 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "beco_nn/beco_nnsupportfunctions.h"

void beco_matmul_impl(const beco_vec64_in_t *beco_mat_l,
                      const beco_vec64_in_t *beco_mat_r,
                      const uint16_t beco_mat_l_stride,
                      const uint16_t beco_mat_r_stride,
                      const int32_t mat_l_cols)
{
    uint16_t col_cnt = mat_l_cols;
    while (col_cnt) {
        beco_write_reg(BECO_REG0, *beco_mat_l);
        beco_write_reg(BECO_REG1, *beco_mat_r);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_mat_l += beco_mat_l_stride;
        beco_mat_r += beco_mat_r_stride;

        col_cnt--;
    }
}

static void beco_fully_connected_shift(int i)
{
    switch (i) {
    case 1:
        beco_shift_block5(BECO_REG0, 1);
        break;
    case 2:
        beco_shift_block5(BECO_REG0, 2);
        break;
    case 3:
        beco_shift_block5(BECO_REG0, 3);
        break;
    case 4:
        beco_shift_block5(BECO_REG0, 4);
        break;
    case 5:
        beco_shift_block5(BECO_REG0, 5);
        break;
    case 6:
        beco_shift_block5(BECO_REG0, 6);
        break;
    case 7:
        beco_shift_block5(BECO_REG0, 7);
        break;
    default:
        break;
    }
}

void beco_fully_connected_q7_impl(const int8_t *p_v,
                                  const beco_vec64_in_t *beco_m,
                                  const uint16_t dim_vec,
                                  const uint32_t beco_m_cols)
{
    uint16_t vecCnt = dim_vec >> 2;
    const uint32_t beco_m_stride = beco_m_cols - 4;
    while (vecCnt) {
        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;

        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;

        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;

        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;

        vecCnt--;
    }

    vecCnt = dim_vec & 3;
    while (vecCnt) {
        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;
        vecCnt--;
    }
}

void beco_fully_connected_norm_q7_impl(const int8_t *p_v,
                                       const beco_vec64_in_t *beco_w,
                                       const uint16_t dim_vec,
                                       const uint32_t beco_cols,
                                       const uint32_t remainder)
{
    int shift_num, offset_num, index;
    for (int i = 0; i < dim_vec; i++) {
        shift_num = (remainder*i) >> 3;
        offset_num = (remainder*i) & 7;

        index = beco_cols*i + shift_num;
        if (offset_num == 0) {
            beco_write_reg(BECO_REG0, beco_w[index]);
            beco_write_reg(BECO_REG1, beco_w[index+1]);
            beco_write_reg(BECO_REG2, beco_w[index+2]);
            beco_write_reg(BECO_REG3, beco_w[index+3]);
            beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        } else {
            beco_write_reg(BECO_REG0, beco_w[index]);
            beco_write_reg(BECO_REG1, beco_w[index+1]);
            beco_write_reg(BECO_REG2, beco_w[index+2]);
            beco_write_reg(BECO_REG3, beco_w[index+3]);
            beco_write_reg(BECO_REG4, beco_w[index+4]);
            beco_fully_connected_shift(offset_num);
            beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        }
    }
}

void beco_fully_connected_q15_impl(const int16_t *p_v,
                                   const beco_vec64_in_t *beco_m,
                                   const uint16_t dim_vec,
                                   const uint32_t beco_m_cols)
{
    uint16_t vecCnt = dim_vec >> 2;
    const uint32_t beco_m_stride = beco_m_cols - 4;
    while (vecCnt) {
        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;

        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;

        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;

        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;

        vecCnt--;
    }

    vecCnt = dim_vec & 3;
    while (vecCnt) {
        beco_write_reg(BECO_REG0, *beco_m++);
        beco_write_reg(BECO_REG1, *beco_m++);
        beco_write_reg(BECO_REG2, *beco_m++);
        beco_write_reg(BECO_REG3, *beco_m++);
        beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        beco_m += beco_m_stride;
        vecCnt--;
    }
}

void beco_fully_connected_norm_q15_impl(const int16_t *p_v,
                                        const beco_vec64_in_t *beco_w,
                                        const uint16_t dim_vec,
                                        const uint32_t beco_cols,
                                        const uint32_t remainder)
{
    int shift_num, offset_num, index;
    for (int i = 0; i < dim_vec; i++) {
        shift_num = (remainder*i) >> 3;
        offset_num = (remainder*i) & 7;

        index = beco_cols*i + shift_num;
        if (offset_num == 0) {
            beco_write_reg(BECO_REG0, beco_w[index]);
            beco_write_reg(BECO_REG1, beco_w[index+1]);
            beco_write_reg(BECO_REG2, beco_w[index+2]);
            beco_write_reg(BECO_REG3, beco_w[index+3]);
            beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        } else {
            beco_write_reg(BECO_REG0, beco_w[index]);
            beco_write_reg(BECO_REG1, beco_w[index+1]);
            beco_write_reg(BECO_REG2, beco_w[index+2]);
            beco_write_reg(BECO_REG3, beco_w[index+3]);
            beco_write_reg(BECO_REG4, beco_w[index+4]);
            beco_fully_connected_shift(offset_num);
            beco_mmacgr4(((beco_vec32_in_t){.i16 = {*p_v++, 0}}), BECO_REG0);
        }
    }
}

void beco_convolve_impl(beco_vec64_in_t *beco_in,
                        beco_vec64_in_t *beco_wt,
                        const uint16_t ch_im_in,
                        const uint16_t dim_kernel_x,
                        const uint32_t beco_och_stride,
                        const uint32_t beco_wt_stride,
                        const uint32_t grouped_index,
                        const uint32_t beco_in_stride)
{
    uint16_t kernelCnt = dim_kernel_x;
    while (kernelCnt) {
        beco_vec64_in_t *cur_beco_in = beco_in + grouped_index;
        beco_vec64_in_t *cur_beco_wt = beco_wt;
        uint16_t ichCnt = ch_im_in >> 2;
        while (ichCnt) {
            beco_write_reg(BECO_REG0, *cur_beco_in++);
            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            beco_write_reg(BECO_REG0, *cur_beco_in++);
            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            beco_write_reg(BECO_REG0, *cur_beco_in++);
            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            beco_write_reg(BECO_REG0, *cur_beco_in++);
            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            ichCnt--;
        }

        ichCnt = ch_im_in & 3;
        while (ichCnt) {
            beco_write_reg(BECO_REG0, *cur_beco_in++);
            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            ichCnt--;
        }

        beco_in += beco_in_stride;
        beco_wt += beco_och_stride;
        kernelCnt--;
    }
}

void beco_convolve_offset_impl(beco_vec64_in_t beco_in,
                               beco_vec64_in_t *beco_wt,
                               const uint16_t ch_im_in,
                               const uint16_t dim_kernel_x,
                               const uint32_t beco_och_stride,
                               const uint32_t beco_wt_stride)
{
    uint16_t kernelCnt = dim_kernel_x;
    beco_write_reg(BECO_REG0, beco_in);
    while (kernelCnt) {
        beco_vec64_in_t *cur_beco_wt = beco_wt;
        uint16_t ichCnt = ch_im_in >> 2;
        while (ichCnt) {
            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            ichCnt--;
        }

        ichCnt = ch_im_in & 3;
        while (ichCnt) {
            beco_write_reg(BECO_REG1, *cur_beco_wt);
            beco_mmacrr(BECO_REG0, BECO_REG1);
            cur_beco_wt += beco_wt_stride;

            ichCnt--;
        }

        beco_wt += beco_och_stride;
        kernelCnt--;
    }
}

void beco_convolve_1x1_impl(const beco_vec64_in_t *beco_in,
                            const beco_vec64_in_t *beco_wt,
                            const uint16_t ch_im_in,
                            const int beco_in_stride,
                            const int beco_wt_stride)
{
    uint16_t ichCnt = ch_im_in >> 2;
    while(ichCnt) {
        beco_write_reg(BECO_REG0, *beco_in);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_in += beco_in_stride;
        beco_wt += beco_wt_stride;

        beco_write_reg(BECO_REG0, *beco_in);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_in += beco_in_stride;
        beco_wt += beco_wt_stride;

        beco_write_reg(BECO_REG0, *beco_in);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_in += beco_in_stride;
        beco_wt += beco_wt_stride;

        beco_write_reg(BECO_REG0, *beco_in);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_in += beco_in_stride;
        beco_wt += beco_wt_stride;

        ichCnt--;
    }

    ichCnt = ch_im_in & 3;
    while(ichCnt) {
        beco_write_reg(BECO_REG0, *beco_in);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_in += beco_in_stride;
        beco_wt += beco_wt_stride;

        ichCnt--;
    }
}

void beco_convolve_1x1_norm_impl(const beco_vec64_in_t *beco_in,
                                 const beco_vec64_in_t *beco_wt,
                                 const uint16_t ch_im_in,
                                 const int beco_wt_stride)
{
    uint16_t ichCnt = ch_im_in >> 2;
    while(ichCnt) {
        beco_write_reg(BECO_REG0, *beco_in++);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_wt += beco_wt_stride;

        beco_write_reg(BECO_REG0, *beco_in++);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_wt += beco_wt_stride;

        beco_write_reg(BECO_REG0, *beco_in++);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_wt += beco_wt_stride;

        beco_write_reg(BECO_REG0, *beco_in++);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_wt += beco_wt_stride;

        ichCnt--;
    }

    ichCnt = ch_im_in & 3;
    while(ichCnt) {
        beco_write_reg(BECO_REG0, *beco_in++);
        beco_write_reg(BECO_REG1, *beco_wt);
        beco_mmacrr(BECO_REG0, BECO_REG1);
        beco_wt += beco_wt_stride;

        ichCnt--;
    }
}

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

#include "beco_dsp/beco_dspfunctions.h"


void beco_mat_mult_q7_readout(beco_vec32_out_t *out,
                              const uint16_t stride)
{
    beco_vec32_out_t *out1 = out + 4*stride;
    beco_read_acc(BECO_ACC0, -4);
    for (int i = 0; i < 4; i++) {
        out[0] = beco_read_next_acc(BECO_ACC0, 4);
        out[1] = beco_read_next_acc(BECO_ACC1, 0);
        out1[0] = beco_read_next_acc(BECO_ACC2, 0);
        out1[1] = beco_read_next_acc(BECO_ACC3, 0);
        out += stride;
        out1 += stride;
    }
}


static void beco_mat_mult_q7_impl(const beco_vec64_in_t *beco_mat_l,
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

void beco_mat_mult_q7_process(const int8_t *mat_l,
                              const int8_t *mat_r,
                              int8_t *out,
                              const int32_t mat_l_rows,
                              const int32_t mat_r_cols,
                              const int32_t mat_l_cols)
{
    const uint16_t beco_rows_num = mat_l_rows >> 3;
    const uint16_t beco_cols_num = mat_r_cols >> 3;

    const uint16_t beco_mat_l_stride = mat_l_rows >> 3;
    const uint16_t beco_mat_r_stride = mat_r_cols >> 3;

    const beco_vec64_in_t *beco_mat_l = (const beco_vec64_in_t *)mat_l;
    const beco_vec64_in_t *beco_mat_r = (const beco_vec64_in_t *)mat_r;
    beco_vec32_out_t *beco_out = (beco_vec32_out_t *)out;
    const uint16_t beco_out_stride = mat_r_cols >> 2;
    const uint16_t beco_out_7ch_stride = beco_out_stride * 7;

    for (int i = 0; i < beco_rows_num; i++) {
        for (int j = 0; j < beco_cols_num; j++) {
            beco_clear_acc(BECO_ACC0);
            beco_clear_acc(BECO_ACC1);
            beco_clear_acc(BECO_ACC2);
            beco_clear_acc(BECO_ACC3);

            beco_mat_mult_q7_impl(beco_mat_l + i, beco_mat_r + j,
                                  beco_mat_l_stride, beco_mat_r_stride, mat_l_cols);

            beco_mat_mult_q7_readout(beco_out, beco_out_stride);
            beco_out += 2;
        }
        beco_out += beco_out_7ch_stride;
    }
}

arm_status beco_mat_mult_q7(const arm_matrix_instance_q7 *pSrcA,
                            const arm_matrix_instance_q7 *pSrcB,
                            arm_matrix_instance_q7 *pDst,
                            q7_t *pState)
{
    (void)pState;
    q7_t *pInA = pSrcA->pData;
    q7_t *pInB = pSrcB->pData;
    q7_t *pOut = pDst->pData;
    uint16_t numColsB = pSrcB->numCols;
    uint16_t numColsA = pSrcA->numCols;
    uint16_t numRowsA = pSrcA->numRows;

    if (numRowsA % 8 != 0 || numColsB % 8 != 0) {
        return ARM_MATH_ARGUMENT_ERROR;
    }

    uint32_t config =
        BECO_CONF_AMODE_REP32 | BECO_CONF_BMODE_REP32 |
        BECO_CONF_ATYPE_INT8  | BECO_CONF_BTYPE_INT8  |
        BECO_CONF_RSHIFT(7) |
        BECO_CONF_PACK_INT8  | BECO_CONF_RD_8x8_ROT90;
    beco_write_config(config);

    beco_mat_mult_q7_process(pInA, pInB, pOut,
                             numRowsA, numColsB, numColsA);

    return ARM_MATH_SUCCESS;
}

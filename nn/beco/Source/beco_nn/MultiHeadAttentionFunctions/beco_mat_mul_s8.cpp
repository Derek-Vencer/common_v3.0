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

#include "beco_nn/beco_nnfunctions.h"
#include "beco_nn/beco_nnsupportfunctions.h"


int32_t beco_mat_mul_s8_get_buffer_size(void)
{
#if defined(ARM_MATH_MVEI)
    return 64 * sizeof(int32_t);
#else
    return 0;
#endif
}

void beco_mat_mul_q7_process(const int8_t *mat_l,
                             const int8_t *mat_r,
                             const int32_t *bias,
                             int8_t *out,
                             const int32_t multiplier,
                             const int32_t shift,
                             const int32_t mat_l_rows,
                             const int32_t mat_r_cols,
                             const int32_t mat_l_cols,
                             const int32_t activation_min,
                             const int32_t activation_max,
                             void *buffer)
{
    const uint16_t beco_rows_num = mat_l_rows >> 3;
    const uint16_t beco_cols_num = mat_r_cols >> 3;

    const uint16_t beco_mat_l_stride = mat_l_rows >> 3;
    const uint16_t beco_mat_r_stride = mat_r_cols >> 3;

    const beco_vec64_in_t *beco_mat_l = (const beco_vec64_in_t *)mat_l;
    const beco_vec64_in_t *beco_mat_r = (const beco_vec64_in_t *)mat_r;
    const beco_vec64_in_t *beco_bias = (const beco_vec64_in_t *)bias;
#if defined(ARM_MATH_MVEI)
    beco_vec32_out_t *beco_out =  (beco_vec32_out_t *)buffer;
#else
    beco_vec32_out_t *beco_out = (beco_vec32_out_t *)out;
    const uint16_t beco_out_stride = mat_r_cols >> 2;
    const uint16_t beco_out_7ch_stride = beco_out_stride * 7;
#endif

    for (int i = 0; i < beco_rows_num; i++) {
        for (int j = 0; j < beco_cols_num; j++) {
            if (bias) {
                beco_convolve_q7_set_bias(&beco_bias[4*j]);
            } else {
                beco_clear_acc(BECO_ACC0);
                beco_clear_acc(BECO_ACC1);
                beco_clear_acc(BECO_ACC2);
                beco_clear_acc(BECO_ACC3);
            }
            beco_matmul_impl(beco_mat_l + i, beco_mat_r + j,
                             beco_mat_l_stride, beco_mat_r_stride, mat_l_cols);

#if defined(ARM_MATH_MVEI)
            beco_mat_mul_q7_readout32(beco_out);
            beco_mat_mul_q7_requant((int32_t *)beco_out, multiplier, shift,
                                    activation_min, activation_max, mat_r_cols, out);
            out += 8;
        }
        out += 7 * mat_r_cols;
#else
            beco_mat_mul_q7_readout(beco_out, beco_out_stride, multiplier, shift,
                                    activation_min, activation_max);
            beco_out += 2;
        }
        beco_out += beco_out_7ch_stride;
#endif
    }
}

arm_cmsis_nn_status beco_mat_mul_s8(const int8_t *mat_l,
                                    const int8_t *mat_r,
                                    const int32_t *bias,
                                    int8_t *out,
                                    const int32_t multiplier,
                                    const int32_t shift,
                                    const int32_t mat_l_rows,
                                    const int32_t mat_r_cols,
                                    const int32_t mat_l_cols,
                                    const int32_t activation_min,
                                    const int32_t activation_max,
                                    void *buffer)
{
    if (mat_l_rows % 8 != 0 || mat_r_cols % 8 != 0) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    uint32_t config =
        BECO_CONF_AMODE_REP32 | BECO_CONF_BMODE_REP32 |
        BECO_CONF_ATYPE_INT8  | BECO_CONF_BTYPE_INT8  |
        BECO_CONF_RSHIFT(0) |
        BECO_CONF_PACK_INT32  | BECO_CONF_RD_8x8_ROT90;
    beco_write_config(config);

    beco_mat_mul_q7_process(mat_l, mat_r, bias, out,
                            multiplier, shift,
                            mat_l_rows, mat_r_cols, mat_l_cols,
                            activation_min, activation_max, buffer);

    return ARM_CMSIS_NN_SUCCESS;
}

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

int32_t beco_mat_mul_s16_get_buffer_size(void)
{
#if defined(ARM_MATH_MVEI)
    return 16 * sizeof(int32_t);
#else
    return 0;
#endif
}

#if defined(CHIP_BEST1603) || defined(CHIP_BEST1306)

#include "arm_math.h"

__STATIC_FORCEINLINE int16_t beco_requantize_s16(int64_t val, int32_t mult,
                                                 int32_t shift, int16_t min, int16_t max)
{
    int32_t in, out;

    in = (int32_t)(val >> -shift);
    mult = REDUCE_MULTIPLIER(mult);
    out = ((int64_t)(in * mult)) >> 14;
    out = (out + 1) >> 1;

    out = MAX(out, min);
    out = MIN(out, max);
    return (int16_t)out;
}

arm_cmsis_nn_status beco_mat_mul_s16(const int16_t *mat_l,
                                     const int16_t *mat_r,
                                     const int32_t *bias,
                                     int16_t *out,
                                     const int32_t multiplier,
                                     const int32_t shift,
                                     const int32_t mat_l_rows,
                                     const int32_t mat_r_cols,
                                     const int32_t mat_l_cols,
                                     const int32_t activation_min,
                                     const int32_t activation_max,
                                     void *buffer)
{
    // mat_l: not transposed, mat_r: transposed
    (void)buffer;
    int32_t inA1, inB1, inA2, inB2;
    int32_t sum;
    int64_t sum64;

    uint16_t row_cnt = mat_l_rows;
    while (row_cnt) {

        const int16_t *ip_b = mat_r;
        const int32_t *ip_bias = bias;
        uint16_t col_cnt = mat_r_cols;
        while (col_cnt) {

            const int16_t *ip_a = mat_l;
            sum64 = bias ? *ip_bias++ : 0;

            uint16_t k_cnt = mat_l_cols >> 2;
            while (k_cnt) {
                inA1 = read_q15x2_ia((int16_t **)&ip_a);
                inB1 = read_q15x2_ia((int16_t **)&ip_b);
                inA2 = read_q15x2_ia((int16_t **)&ip_a);
                inB2 = read_q15x2_ia((int16_t **)&ip_b);

                sum = __SMLALD(inA1, inB1, 0);
                sum = __SMLALD(inA2, inB2, sum);
                sum64 += sum;

                k_cnt--;
            }
            k_cnt = mat_l_cols & 3;
            while (k_cnt) {
                sum64 += *ip_a++ * *ip_b++;

                k_cnt--;
            }

            *out++ = beco_requantize_s16(sum64, multiplier, shift, activation_min, activation_max);
            col_cnt--;
        }

        mat_l += mat_l_cols;
        row_cnt--;
    }

    return ARM_CMSIS_NN_SUCCESS;
}

#else

static void beco_mat_mul_q15_set_bias(const int32_t* bias)
{
    beco_write_reg(BECO_REG0, (beco_vec64_in_t){.i32 = {*bias++, 0}});
    beco_write_reg(BECO_REG1, (beco_vec64_in_t){.i32 = {*bias++, 0}});
    beco_write_reg(BECO_REG2, (beco_vec64_in_t){.i32 = {*bias++, 0}});
    beco_write_reg(BECO_REG3, (beco_vec64_in_t){.i32 = {*bias++, 0}});

    beco_set_acc_bias(BECO_ACC0_0, BECO_REG0, BECO_REG7);
    beco_set_acc_bias(BECO_ACC0_8, BECO_REG1, BECO_REG7);
    beco_set_acc_bias(BECO_ACC0_4, BECO_REG0, BECO_REG7);
    beco_set_acc_bias(BECO_ACC0_12, BECO_REG1, BECO_REG7);

    beco_set_acc_bias(BECO_ACC1_0, BECO_REG2, BECO_REG7);
    beco_set_acc_bias(BECO_ACC1_8, BECO_REG3, BECO_REG7);
    beco_set_acc_bias(BECO_ACC1_4, BECO_REG2, BECO_REG7);
    beco_set_acc_bias(BECO_ACC1_12, BECO_REG3, BECO_REG7);

    beco_set_acc_bias(BECO_ACC2_0, BECO_REG0, BECO_REG7);
    beco_set_acc_bias(BECO_ACC2_8, BECO_REG1, BECO_REG7);
    beco_set_acc_bias(BECO_ACC2_4, BECO_REG0, BECO_REG7);
    beco_set_acc_bias(BECO_ACC2_12, BECO_REG1, BECO_REG7);

    beco_set_acc_bias(BECO_ACC3_0, BECO_REG2, BECO_REG7);
    beco_set_acc_bias(BECO_ACC3_8, BECO_REG3, BECO_REG7);
    beco_set_acc_bias(BECO_ACC3_4, BECO_REG2, BECO_REG7);
    beco_set_acc_bias(BECO_ACC3_12, BECO_REG3, BECO_REG7);
}

void beco_mat_mul_q15_process(const int16_t *mat_l,
                              const int16_t *mat_r,
                              const int32_t *bias,
                              int16_t *out,
                              const int32_t multiplier,
                              const int32_t shift,
                              const int32_t mat_l_rows,
                              const int32_t mat_r_cols,
                              const int32_t mat_l_cols,
                              const int32_t activation_min,
                              const int32_t activation_max,
                              void *buffer)
{
    const uint16_t beco_rows_num = mat_l_rows >> 2;
    const uint16_t beco_cols_num = mat_r_cols >> 2;

    const uint16_t beco_mat_l_stride = mat_l_rows >> 2;
    const uint16_t beco_mat_r_stride = mat_r_cols >> 2;

    const beco_vec64_in_t *beco_mat_l = (const beco_vec64_in_t *)mat_l;
    const beco_vec64_in_t *beco_mat_r = (const beco_vec64_in_t *)mat_r;
#if defined(ARM_MATH_MVEI)
    beco_vec32_out_t *beco_out =  (beco_vec32_out_t *)buffer;
#else
    beco_vec32_out_t *beco_out = (beco_vec32_out_t *)out;
    const uint16_t beco_out_stride = mat_r_cols >> 1;
    const uint16_t beco_out_3ch_stride = beco_out_stride * 3;
#endif

    for (int i = 0; i < beco_rows_num; i++) {
        for (int j = 0; j < beco_cols_num; j++) {
            if (bias) {
                beco_mat_mul_q15_set_bias(&bias[4 * j]);
            } else {
                beco_clear_acc(BECO_ACC0);
                beco_clear_acc(BECO_ACC1);
                beco_clear_acc(BECO_ACC2);
                beco_clear_acc(BECO_ACC3);
            }
            beco_matmul_impl(beco_mat_l + i, beco_mat_r + j,
                             beco_mat_l_stride, beco_mat_r_stride, mat_l_cols);

#if defined(ARM_MATH_MVEI)
            beco_mat_mul_q15_readout32(beco_out);
            beco_mat_mul_q15_requant((int32_t *)beco_out, multiplier, shift,
                                     activation_min, activation_max, mat_r_cols, out);
            out += 4;
        }
        out += 3 * mat_r_cols;
#else
            beco_mat_mul_q15_readout(beco_out, beco_out_stride, multiplier, shift,
                                     activation_min, activation_max);
            beco_out += 2;
        }
        beco_out += beco_out_3ch_stride;
#endif
    }
}

arm_cmsis_nn_status beco_mat_mul_s16(const int16_t *mat_l,
                                     const int16_t *mat_r,
                                     const int32_t *bias,
                                     int16_t *out,
                                     const int32_t multiplier,
                                     const int32_t shift,
                                     const int32_t mat_l_rows,
                                     const int32_t mat_r_cols,
                                     const int32_t mat_l_cols,
                                     const int32_t activation_min,
                                     const int32_t activation_max,
                                     void *buffer)
{
    // mat_l: transposed, mat_r: not transposed
    if (mat_l_rows % 4 != 0 || mat_r_cols % 4 != 0) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }
    if (shift > -8) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    uint32_t config =
        BECO_CONF_AMODE_REP32 | BECO_CONF_BMODE_REP32 |
        BECO_CONF_ATYPE_INT16 | BECO_CONF_BTYPE_INT16 |
        BECO_CONF_RSHIFT(8) |
        BECO_CONF_PACK_INT32  | BECO_CONF_RD_16x16_ROT90;
    beco_write_config(config);

    beco_mat_mul_q15_process(mat_l, mat_r, bias, out,
                             multiplier, shift+8,
                             mat_l_rows, mat_r_cols, mat_l_cols,
                             activation_min, activation_max, buffer);

    return ARM_CMSIS_NN_SUCCESS;
}
#endif

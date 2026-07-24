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

#include "beco_bias.hpp"
#include "beco_nn/beco_nnfunctions.h"
#include "beco_nn/beco_nnsupportfunctions.h"


int32_t beco_batch_matmul_s8_get_buffer_size(const cmsis_nn_dims *output_dims)
{
#if defined(ARM_MATH_MVEI)
    return (64 + output_dims->c) * sizeof(int32_t);
#else
    return output_dims->c * sizeof(int32_t);
#endif
}

static void beco_batch_matmul_offset_s8_impl(const beco_vec64_in_t *beco_mat_l,
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
        beco_mmacrr(BECO_REG0, BECO_REG5);
        beco_mat_l += beco_mat_l_stride;
        beco_mat_r += beco_mat_r_stride;

        col_cnt--;
    }
}

void beco_vector_sum_s8(int32_t *vector_sum_buf,
                        const int32_t vector_rows,
                        const int32_t vector_cols,
                        const int8_t *vector_data,
                        const int32_t lhs_offset,
                        const int32_t rhs_offset,
                        const int32_t *bias_data)
{
    if (lhs_offset) {
        uint32_t config =
            BECO_CONF_AMODE_REP16 | BECO_CONF_BMODE_REP64 |
            BECO_CONF_ATYPE_INT16 | BECO_CONF_BTYPE_INT8 |
            BECO_CONF_RSHIFT(0) |
            BECO_CONF_PACK_INT32 | BECO_CONF_RD_16x8_ROT90;
        beco_write_config(config);

        int32_t beco_cols_cnt = vector_cols >> 3;
        beco_vec64_in_t *beco_w = (beco_vec64_in_t *)vector_data;
        beco_vec32_out_t *beco_out = (beco_vec32_out_t *)vector_sum_buf;
        int32_t rhs_sum = vector_rows * rhs_offset * lhs_offset;
        for (int i = 0; i < beco_cols_cnt; i++) {
            beco_vec64_in_t *beco_m = beco_w + i;
            if (rhs_offset) {
                beco_set_preload_16x8(rhs_sum);
            } else {
                beco_clear_acc(BECO_ACC0);
            }

            int32_t beco_rows_cnt = vector_rows;
            while(beco_rows_cnt) {
                beco_write_reg(BECO_REG0, *beco_m);
                beco_mmacgr(BECO_ACC0, ((beco_vec32_in_t){.i16 = {(int16_t)lhs_offset, 0}}), BECO_REG0);
                beco_m += beco_cols_cnt;
                beco_rows_cnt--;
            }

            *beco_out++ = beco_read_acc(BECO_ACC0, 0);
            *beco_out++ = beco_read_acc(BECO_ACC0, 2);
            *beco_out++ = beco_read_acc(BECO_ACC0, 8);
            *beco_out++ = beco_read_acc(BECO_ACC0, 10);
            *beco_out++ = beco_read_acc(BECO_ACC0, 4);
            *beco_out++ = beco_read_acc(BECO_ACC0, 6);
            *beco_out++ = beco_read_acc(BECO_ACC0, 12);
            *beco_out++ = beco_read_acc(BECO_ACC0, 14);
        }
    } else {
        memset(vector_sum_buf, 0, vector_cols * sizeof(int32_t));
    }
}

void beco_batch_matmul_s8_process(const int8_t *mat_l,
                                  const int8_t *mat_r,
                                  const int32_t *bias,
                                  int8_t *out,
                                  const int32_t multiplier,
                                  const int32_t shift,
                                  const int32_t filter_offset,
                                  const int32_t out_offset,
                                  const int32_t mat_l_rows,
                                  const int32_t mat_r_cols,
                                  const int32_t mat_l_cols,
                                  const int32_t activation_min,
                                  const int32_t activation_max,
                                  void *buffer)
{
    uint32_t config =
        BECO_CONF_AMODE_REP32 | BECO_CONF_BMODE_REP32 |
        BECO_CONF_ATYPE_INT8  | BECO_CONF_BTYPE_INT8  |
        BECO_CONF_RSHIFT(0) |
        BECO_CONF_PACK_INT32  | BECO_CONF_RD_8x8_ROT90;
    beco_write_config(config);

    const uint16_t beco_rows_num = mat_l_rows >> 3;
    const uint16_t beco_cols_num = mat_r_cols >> 3;

    const uint16_t beco_mat_l_stride = mat_l_rows >> 3;
    const uint16_t beco_mat_r_stride = mat_r_cols >> 3;

    const beco_vec64_in_t *beco_mat_l = (const beco_vec64_in_t *)mat_l;
    const beco_vec64_in_t *beco_mat_r = (const beco_vec64_in_t *)mat_r;
    const beco_vec64_in_t *beco_bias = (const beco_vec64_in_t *)bias;
    int8_t fo = (int8_t)filter_offset;
    beco_vec64_in_t beco_input_offset = {.i8 = {fo, fo, fo, fo, fo, fo, fo, fo}};
    beco_write_reg(BECO_REG5, beco_input_offset);

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
            if (filter_offset) {
                beco_batch_matmul_offset_s8_impl(beco_mat_l + i, beco_mat_r + j,
                                                 beco_mat_l_stride, beco_mat_r_stride, mat_l_cols);
            } else {
                beco_matmul_impl(beco_mat_l + i, beco_mat_r + j,
                                 beco_mat_l_stride, beco_mat_r_stride, mat_l_cols);
            }

#if defined(ARM_MATH_MVEI)
            beco_batch_matmul_s8_readout32(beco_out);
            beco_batch_matmul_s8_requant((int32_t *)beco_out, multiplier, shift, out_offset,
                                         activation_min, activation_max, mat_r_cols, out);
            out += 8;
        }
        out += 7 * mat_r_cols;
#else
            beco_batch_matmul_s8_readout(beco_out, beco_out_stride, multiplier, shift,
                                         out_offset, activation_min, activation_max);
            beco_out += 2;
        }
        beco_out += beco_out_7ch_stride;
#endif
    }
}

arm_cmsis_nn_status beco_batch_matmul_s8(const cmsis_nn_context *ctx,
                                         const cmsis_nn_bmm_params *bmm_params,
                                         const cmsis_nn_per_tensor_quant_params *quant_params,
                                         const cmsis_nn_dims *input_lhs_dims,
                                         const int8_t *input_lhs,
                                         const cmsis_nn_dims *input_rhs_dims,
                                         const int8_t *input_rhs,
                                         const cmsis_nn_dims *output_dims,
                                         int8_t *output)
{
    const int32_t output_batch = output_dims->n;
    const int32_t output_height = output_dims->h;
    const int32_t lhs_rows = input_lhs_dims->w;
    const int32_t rhs_rows = input_rhs_dims->w;
    const int32_t rhs_cols = input_rhs_dims->c;

    if (lhs_rows % 8 != 0 || rhs_rows % 8 != 0) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    const int32_t inner_lhs_diff = input_lhs_dims->h >= input_rhs_dims->h ? 0 : lhs_rows * rhs_cols;
    const int32_t inner_rhs_diff = input_rhs_dims->h >= input_lhs_dims->h ? rhs_rows * rhs_cols : 0;
    const int32_t outer_lhs_diff = input_lhs_dims->n >= input_rhs_dims->n
                                   ? inner_lhs_diff
                                   : -((lhs_rows * rhs_cols) - inner_lhs_diff) * input_lhs_dims->h;
    const int32_t outer_rhs_diff = input_rhs_dims->n >= input_lhs_dims->n
                                   ? (rhs_rows * rhs_cols) - inner_rhs_diff
                                   : -inner_rhs_diff * input_rhs_dims->h;
    int32_t *vector_sum_buf = (int32_t *)ctx->buf;
    void *buffer = (void *)(vector_sum_buf + rhs_rows);

    for (int i_out_batch = 0; i_out_batch < output_batch; i_out_batch++) {
        for (int i_out_height = 0; i_out_height < output_height; i_out_height++) {

            beco_vector_sum_s8(vector_sum_buf,
                               rhs_cols,
                               rhs_rows,
                               input_rhs,
                               bmm_params->fc_params.input_offset,
                               bmm_params->fc_params.filter_offset,
                               NULL);

            beco_batch_matmul_s8_process(input_lhs, input_rhs, vector_sum_buf, output,
                                         quant_params->multiplier, quant_params->shift,
                                         bmm_params->fc_params.filter_offset,
                                         bmm_params->fc_params.output_offset,
                                         lhs_rows, rhs_rows, rhs_cols,
                                         bmm_params->fc_params.activation.min,
                                         bmm_params->fc_params.activation.max, buffer);

            input_lhs += lhs_rows * rhs_cols;
            input_lhs -= inner_lhs_diff;
            input_rhs += inner_rhs_diff;
            output += lhs_rows * rhs_rows;
        }

        input_lhs += outer_lhs_diff;
        input_rhs += outer_rhs_diff;
    }

    return ARM_CMSIS_NN_SUCCESS;
}

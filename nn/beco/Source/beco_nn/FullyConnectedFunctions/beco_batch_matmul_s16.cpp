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


int32_t beco_batch_matmul_s16_get_buffer_size()
{
#if defined(ARM_MATH_MVEI)
    return 16 * sizeof(int32_t);
#else
    return 0;
#endif
}

void beco_batch_matmul_s16_process(const int16_t *mat_l,
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
    uint32_t config =
        BECO_CONF_AMODE_REP32 | BECO_CONF_BMODE_REP32 |
        BECO_CONF_ATYPE_INT16 | BECO_CONF_BTYPE_INT16 |
        BECO_CONF_RSHIFT(8) |
        BECO_CONF_PACK_INT32  | BECO_CONF_RD_16x16_ROT90;
    beco_write_config(config);

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

            beco_set_preload_16x16_4ACC(128);

            beco_matmul_impl(beco_mat_l + i, beco_mat_r + j,
                             beco_mat_l_stride, beco_mat_r_stride, mat_l_cols);

#if defined(ARM_MATH_MVEI)
            beco_mat_mul_q15_readout32(beco_out);
            beco_mat_mul_q15_requant((int32_t *)beco_out, multiplier, shift+8,
                                     activation_min, activation_max, mat_r_cols, out);
            out += 4;
        }
        out += 3 * mat_r_cols;
#else
            beco_mat_mul_q15_readout(beco_out, beco_out_stride, multiplier, shift+8,
                                     activation_min, activation_max);
            beco_out += 2;
        }
        beco_out += beco_out_3ch_stride;
#endif
    }
}

arm_cmsis_nn_status beco_batch_matmul_s16(const cmsis_nn_context *ctx,
                                          const cmsis_nn_bmm_params *bmm_params,
                                          const cmsis_nn_per_tensor_quant_params *quant_params,
                                          const cmsis_nn_dims *input_lhs_dims,
                                          const int16_t *input_lhs,
                                          const cmsis_nn_dims *input_rhs_dims,
                                          const int16_t *input_rhs,
                                          const cmsis_nn_dims *output_dims,
                                          int16_t *output)
{
    const int32_t output_batch = output_dims->n;
    const int32_t output_height = output_dims->h;
    const int32_t lhs_rows = input_lhs_dims->w;
    const int32_t rhs_rows = input_rhs_dims->w;
    const int32_t rhs_cols = input_rhs_dims->c;

    if (lhs_rows % 4 != 0 || rhs_rows % 4 != 0) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }
    if (quant_params->shift > -8) {
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

    for (int i_out_batch = 0; i_out_batch < output_batch; i_out_batch++) {
        for (int i_out_height = 0; i_out_height < output_height; i_out_height++) {

            beco_batch_matmul_s16_process(input_lhs, input_rhs, NULL, output,
                                          quant_params->multiplier, quant_params->shift,
                                          lhs_rows, rhs_rows, rhs_cols,
                                          bmm_params->fc_params.activation.min,
                                          bmm_params->fc_params.activation.max, ctx->buf);

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

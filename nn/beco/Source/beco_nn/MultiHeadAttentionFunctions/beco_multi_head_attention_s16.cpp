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
#include "dsp/matrix_functions.h"


int32_t beco_multi_head_attention_s16_get_buffer_size(const int32_t num_sequence,
                                                      const int32_t num_feature)
{
    int32_t buffer_len = 0;
    if (num_feature % 32 != 0) {
        buffer_len = 32 * sizeof(int16_t);
    }
#if defined(ARM_MATH_MVEI)
    buffer_len += 32 * sizeof(int32_t);
#endif

    // output_q, output_k, output_v, output_q_t, output_k_t, output_v_t
    buffer_len += 6 * num_sequence * num_feature * sizeof(int16_t);
    // attn_weights_t
    buffer_len += num_sequence * num_sequence * sizeof(int16_t);

    return buffer_len;
}


arm_cmsis_nn_status beco_multi_head_attention_s16(const int16_t* input_q,
                                                  const int16_t* input_k,
                                                  const int16_t* input_v,
                                                  const int8_t* in_proj_q_weights,
                                                  const int8_t* in_proj_k_weights,
                                                  const int8_t* in_proj_v_weights,
                                                  const int8_t* out_proj_weights,
                                                  const int32_t* in_proj_q_bias,
                                                  const int32_t* in_proj_k_bias,
                                                  const int32_t* in_proj_v_bias,
                                                  const int32_t* out_proj_bias,
                                                  const int32_t num_batch,
                                                  const int32_t num_sequence,
                                                  const int32_t num_feature,
                                                  const int32_t num_heads,
                                                  const cmsis_nn_scaling* in_proj_q_requant_params,
                                                  const cmsis_nn_scaling* in_proj_k_requant_params,
                                                  const cmsis_nn_scaling* in_proj_v_requant_params,
                                                  const cmsis_nn_scaling* out_qk_requant_params,
                                                  const cmsis_nn_scaling* out_attn_weights_requant_params,
                                                  const cmsis_nn_scaling* out_wv_requant_params,
                                                  const cmsis_nn_scaling* out_requant_params,
                                                  int16_t* output,
                                                  int16_t* attn_weights,
                                                  void* buffer)
{
    if (num_feature % num_heads != 0) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    int32_t mid_dim = num_feature / num_heads;
    if (num_sequence % 4 != 0 || mid_dim % 4 != 0) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    cmsis_nn_fc_params fc_params;
    fc_params.input_offset = 0;
    fc_params.filter_offset = 0;
    fc_params.output_offset = 0;
    fc_params.activation.min = NN_Q15_MIN;
    fc_params.activation.max = NN_Q15_MAX;
    cmsis_nn_per_tensor_quant_params quant_params;
    cmsis_nn_dims input_dims = {1, 1, 1, num_feature};
    cmsis_nn_dims filter_dims = {num_feature, 1, 1, num_feature};
    cmsis_nn_dims bias_dims = {1, 1, 1, num_feature};
    cmsis_nn_dims output_dims = {1, 1, 1, num_feature};
    cmsis_nn_context ctx;
    ctx.buf = (void *)buffer;
    ctx.size = beco_fully_connected_s16_get_buffer_size(&output_dims);

    int32_t batch_cnt = num_batch;
    while (batch_cnt) {

        // run in_proj q,k,v
        int16_t* output_q = (int16_t*)((int8_t*)buffer + ctx.size);
        int16_t* output_k = output_q + num_sequence * num_feature;
        int16_t* output_v = output_k + num_sequence * num_feature;
        for (int i = 0; i < num_sequence; i++) {
            quant_params.multiplier = in_proj_q_requant_params->multiplier;
            quant_params.shift = in_proj_q_requant_params->shift;
            beco_fully_connected_s16(&ctx, &fc_params, &quant_params, &input_dims,
                                     &input_q[i*num_feature], &filter_dims, in_proj_q_weights,
                                     &bias_dims, in_proj_q_bias, &output_dims,
                                     &output_q[i*num_feature]);

            quant_params.multiplier = in_proj_k_requant_params->multiplier;
            quant_params.shift = in_proj_k_requant_params->shift;
            beco_fully_connected_s16(&ctx, &fc_params, &quant_params, &input_dims,
                                     &input_k[i*num_feature], &filter_dims, in_proj_k_weights,
                                     &bias_dims, in_proj_k_bias, &output_dims,
                                     &output_k[i*num_feature]);

            quant_params.multiplier = in_proj_v_requant_params->multiplier;
            quant_params.shift = in_proj_v_requant_params->shift;
            beco_fully_connected_s16(&ctx, &fc_params, &quant_params, &input_dims,
                                     &input_v[i*num_feature], &filter_dims, in_proj_v_weights,
                                     &bias_dims, in_proj_v_bias, &output_dims,
                                     &output_v[i*num_feature]);
        }

        // transpose
        int16_t* output_q_t = output_v + num_sequence * num_feature;
        int16_t* output_k_t = output_q_t + num_sequence * num_feature;
        int16_t* output_v_t = output_k_t + num_sequence * num_feature;
#if defined(CHIP_BEST1603) || defined(CHIP_BEST1306)
        // q,k: [sequence, num_heads, mid_dim] -> [num_heads, sequence, mid_dim]
        // v: [sequence, num_heads, mid_dim] -> [num_heads, mid_dim, sequence]
        if (num_heads == 1) {
            memcpy(output_q_t, output_q, num_sequence * num_feature * sizeof(int16_t));
            memcpy(output_k_t, output_k, num_sequence * num_feature * sizeof(int16_t));
        } else {
            for (int n_head = 0; n_head < num_heads; n_head++) {
                for (int n_seq = 0; n_seq < num_sequence; n_seq++) {
                    memcpy(&output_q_t[n_head*num_sequence*mid_dim + n_seq*mid_dim],
                           &output_q[n_seq*num_heads*mid_dim + n_head*mid_dim],
                           mid_dim * sizeof(int16_t));
                    memcpy(&output_k_t[n_head*num_sequence*mid_dim + n_seq*mid_dim],
                           &output_k[n_seq*num_heads*mid_dim + n_head*mid_dim],
                           mid_dim * sizeof(int16_t));
                }
            }
        }
        arm_matrix_instance_q15 pISrc;
        pISrc.numRows = num_sequence;
        pISrc.numCols = num_heads*mid_dim;
        pISrc.pData = output_v;
        arm_matrix_instance_q15 pIDst;
        pIDst.numRows = num_heads*mid_dim;
        pIDst.numCols = num_sequence;
        pIDst.pData = output_v_t;
        arm_mat_trans_q15(&pISrc, &pIDst);
#else
        // q,k: [sequence, num_heads, mid_dim] -> [num_heads, mid_dim, sequence]
        // v: [sequence, num_heads, mid_dim] -> [num_heads, sequence, mid_dim]
        arm_matrix_instance_q15 pISrc;
        pISrc.numRows = num_sequence;
        pISrc.numCols = num_heads*mid_dim;
        pISrc.pData = output_q;
        arm_matrix_instance_q15 pIDst;
        pIDst.numRows = num_heads*mid_dim;
        pIDst.numCols = num_sequence;
        pIDst.pData = output_q_t;
        arm_mat_trans_q15(&pISrc, &pIDst);
        pISrc.pData = output_k;
        pIDst.pData = output_k_t;
        arm_mat_trans_q15(&pISrc, &pIDst);
        if (num_heads == 1) {
            memcpy(output_v_t, output_v, num_sequence * num_feature * sizeof(int16_t));
        } else {
            for (int n_head = 0; n_head < num_heads; n_head++) {
                for (int n_seq = 0; n_seq < num_sequence; n_seq++) {
                    memcpy(&output_v_t[n_head*num_sequence*mid_dim + n_seq*mid_dim],
                           &output_v[n_seq*num_heads*mid_dim + n_head*mid_dim],
                           mid_dim * sizeof(int16_t));
                }
            }
        }
#endif

        // run q @ k
        for (int j = 0; j < num_heads; j++) {
            beco_mat_mul_s16(output_q_t + j * mid_dim * num_sequence,
                             output_k_t + j * mid_dim * num_sequence,
                             NULL,
                             attn_weights + j * num_sequence * num_sequence,
                             out_qk_requant_params->multiplier,
                             out_qk_requant_params->shift,
                             num_sequence, num_sequence, mid_dim,
                             NN_Q15_MIN, NN_Q15_MAX, buffer);
        }

        // run softmax
        for (int j = 0; j < num_heads; j++) {
            cmsis_nn_softmax_lut_s16 softmax_params;
            softmax_params.exp_lut = softmax_s16_exp_lut;
            softmax_params.one_by_one_lut = softmax_s16_one_by_one_lut;
            arm_softmax_s16(attn_weights + j * num_sequence * num_sequence,
                            num_sequence, num_sequence,
                            out_attn_weights_requant_params->multiplier,
                            out_attn_weights_requant_params->shift,
                            &softmax_params,
                            attn_weights + j * num_sequence * num_sequence);
        }

        // run attn_weights @ v
        int16_t *wv = (int16_t*)((int8_t*)buffer + ctx.size);
        for (int j = 0; j < num_heads; j++) {
#if defined(CHIP_BEST1603) || defined(CHIP_BEST1306)
            int16_t* attn_weights_t = attn_weights + j * num_sequence * num_sequence;
#else
            int16_t* attn_weights_t = output_v_t + num_sequence * num_feature;
            pISrc.numRows = num_sequence;
            pISrc.numCols = num_sequence;
            pISrc.pData = attn_weights + j * num_sequence * num_sequence;
            pIDst.numRows = num_sequence;
            pIDst.numCols = num_sequence;
            pIDst.pData = attn_weights_t;
            arm_mat_trans_q15(&pISrc, &pIDst);
#endif
            beco_mat_mul_s16(attn_weights_t,
                             output_v_t + j * mid_dim * num_sequence,
                             NULL,
                             wv + j * num_sequence * mid_dim,
                             out_wv_requant_params->multiplier,
                             out_wv_requant_params->shift,
                             num_sequence, mid_dim, num_sequence,
                             NN_Q15_MIN, NN_Q15_MAX, buffer);
        }

        // transpose
        // wv: [num_heads, sequence, mid_dim] -> [sequence, num_heads, mid_dim]
        int16_t *wv_t = wv + num_sequence * num_feature;
        if (num_heads == 1) {
            memcpy(wv_t, wv, num_sequence * num_feature * sizeof(int16_t));
        } else {
            for (int n_seq = 0; n_seq < num_sequence; n_seq++) {
                for (int n_head = 0; n_head < num_heads; n_head++) {
                    memcpy(&wv_t[n_seq*num_heads*mid_dim + n_head*mid_dim],
                           &wv[n_head*num_sequence*mid_dim + n_seq*mid_dim],
                           mid_dim * sizeof(int16_t));
                }
            }
        }

        // run out_proj
        for (int i = 0; i < num_sequence; i++) {
            quant_params.multiplier = out_requant_params->multiplier;
            quant_params.shift = out_requant_params->shift;
            beco_fully_connected_s16(&ctx, &fc_params, &quant_params, &input_dims,
                                     &wv_t[i*num_feature], &filter_dims, out_proj_weights,
                                     &bias_dims, out_proj_bias, &output_dims,
                                     &output[i*num_feature]);
        }

        input_q += num_sequence * num_feature;
        input_k += num_sequence * num_feature;
        input_v += num_sequence * num_feature;
        output += num_sequence * num_feature;
        attn_weights += num_heads * num_sequence * num_sequence;
        batch_cnt--;
    }

    return ARM_CMSIS_NN_SUCCESS;
}

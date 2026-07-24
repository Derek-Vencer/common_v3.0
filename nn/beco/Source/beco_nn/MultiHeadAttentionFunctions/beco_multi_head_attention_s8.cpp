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
#include "dsp/support_functions.h"
#include "arm_nnsupportfunctions.h"


int32_t beco_multi_head_attention_s8_get_buffer_size(const int32_t num_sequence,
                                                     const int32_t num_feature)
{
    int32_t buffer_len = 0;
    if (num_feature % 32 != 0) {
        buffer_len = 32 * sizeof(int8_t);
    }
#if defined(ARM_MATH_MVEI)
    buffer_len += 64 * sizeof(int32_t);
#endif

    // output_q, output_k, output_v, output_q_t, output_k_t, output_v_t
    buffer_len += 7 * num_sequence * num_feature * sizeof(int8_t);
    // attn_weights_t
    buffer_len += num_sequence * num_sequence * sizeof(int16_t);

    return buffer_len;
}


arm_cmsis_nn_status beco_multi_head_attention_s8(const int8_t* input_q,
                                                 const int8_t* input_k,
                                                 const int8_t* input_v,
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
                                                 int8_t* output,
                                                 int8_t* attn_weights,
                                                 void* buffer)
{
    if (num_feature % num_heads != 0) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    int32_t mid_dim = num_feature / num_heads;
    if (num_sequence % 8 != 0 || mid_dim % 8 != 0) {
        return ARM_CMSIS_NN_ARG_ERROR;
    }

    int32_t batch_cnt = num_batch;
    while (batch_cnt) {
        int32_t ctx_buffer_len = beco_mat_mul_s8_get_buffer_size();
        // run in_proj q,k,v
        int8_t* input_trans = (int8_t*)((int8_t*)buffer + ctx_buffer_len);
        int8_t* output_q = input_trans + num_sequence * num_feature;
        int8_t* output_k = output_q + num_sequence * num_feature;
        int8_t* output_v = output_k + num_sequence * num_feature;

        arm_matrix_instance_q7 pISrc;
        pISrc.numRows = num_sequence;
        pISrc.numCols = num_feature;
        pISrc.pData = (int8_t *)input_q;
        arm_matrix_instance_q7 pIDst;
        pIDst.numRows = num_feature;
        pIDst.numCols = num_sequence;
        pIDst.pData = input_trans;
        arm_mat_trans_q7(&pISrc, &pIDst);
        beco_mat_mul_s8(input_trans,
                        in_proj_q_weights,
                        in_proj_q_bias,
                        output_q,
                        in_proj_q_requant_params->multiplier,
                        in_proj_q_requant_params->shift,
                        num_sequence, num_feature, num_feature,
                        NN_Q7_MIN, NN_Q7_MAX, buffer);
        if (input_q != input_k) {
            pISrc.pData = (int8_t *)input_k;
            pIDst.pData = input_trans;
            arm_mat_trans_q7(&pISrc, &pIDst);
        }
        beco_mat_mul_s8(input_trans,
                        in_proj_k_weights,
                        in_proj_k_bias,
                        output_k,
                        in_proj_k_requant_params->multiplier,
                        in_proj_k_requant_params->shift,
                        num_sequence, num_feature, num_feature,
                        NN_Q7_MIN, NN_Q7_MAX, buffer);
        if (input_k != input_v) {
            pISrc.pData = (int8_t *)input_v;
            pIDst.pData = input_trans;
            arm_mat_trans_q7(&pISrc, &pIDst);
        }
        beco_mat_mul_s8(input_trans,
                        in_proj_v_weights,
                        in_proj_v_bias,
                        output_v,
                        in_proj_v_requant_params->multiplier,
                        in_proj_v_requant_params->shift,
                        num_sequence, num_feature, num_feature,
                        NN_Q7_MIN, NN_Q7_MAX, buffer);

        // transpose
        int8_t* output_q_t = output_v + num_sequence * num_feature;
        int8_t* output_k_t = output_q_t + num_sequence * num_feature;
        int8_t* output_v_t = output_k_t + num_sequence * num_feature;

        // q,k: [sequence, num_heads, mid_dim] -> [num_heads, mid_dim, sequence]
        // v: [sequence, num_heads, mid_dim] -> [num_heads, sequence, mid_dim]
        pISrc.numRows = num_sequence;
        pISrc.numCols = num_heads*mid_dim;
        pISrc.pData = output_q;
        pIDst.numRows = num_heads*mid_dim;
        pIDst.numCols = num_sequence;
        pIDst.pData = output_q_t;
        arm_mat_trans_q7(&pISrc, &pIDst);
        pISrc.pData = output_k;
        pIDst.pData = output_k_t;
        arm_mat_trans_q7(&pISrc, &pIDst);
        if (num_heads == 1) {
            memcpy(output_v_t, output_v, num_sequence * num_feature * sizeof(int8_t));
        } else {
            for (int n_head = 0; n_head < num_heads; n_head++) {
                for (int n_seq = 0; n_seq < num_sequence; n_seq++) {
                    memcpy(&output_v_t[n_head*num_sequence*mid_dim + n_seq*mid_dim],
                           &output_v[n_seq*num_heads*mid_dim + n_head*mid_dim],
                           mid_dim * sizeof(int8_t));
                }
            }
        }

        // run q @ k
        for (int j = 0; j < num_heads; j++) {
            beco_mat_mul_s8(output_q_t + j * mid_dim * num_sequence,
                            output_k_t + j * mid_dim * num_sequence,
                            NULL,
                            attn_weights + j * num_sequence * num_sequence,
                            out_qk_requant_params->multiplier,
                            out_qk_requant_params->shift,
                            num_sequence, num_sequence, mid_dim,
                            NN_Q7_MIN, NN_Q7_MAX, buffer);
        }

        // run softmax
        for (int j = 0; j < num_heads; j++) {
            int16_t *attn_weights_i16 = (int16_t *)(output_v_t + num_sequence * num_feature);
            arm_q7_to_q15(attn_weights + j * num_sequence * num_sequence,
                          attn_weights_i16, num_sequence * num_sequence);
            cmsis_nn_softmax_lut_s16 softmax_params;
            softmax_params.exp_lut = softmax_s16_exp_lut;
            softmax_params.one_by_one_lut = softmax_s16_one_by_one_lut;
            arm_softmax_s16(attn_weights_i16,
                            num_sequence, num_sequence,
                            out_attn_weights_requant_params->multiplier,
                            out_attn_weights_requant_params->shift,
                            &softmax_params,
                            attn_weights_i16);
            arm_q15_to_q7(attn_weights_i16,
                          attn_weights + j * num_sequence * num_sequence,
                          num_sequence * num_sequence);
        }

        // run attn_weights @ v
        int8_t *wv = (int8_t*)((int8_t*)buffer + ctx_buffer_len);
        for (int j = 0; j < num_heads; j++) {
            int8_t* attn_weights_t = output_v_t + num_sequence * num_feature;
            pISrc.numRows = num_sequence;
            pISrc.numCols = num_sequence;
            pISrc.pData = attn_weights + j * num_sequence * num_sequence;
            pIDst.numRows = num_sequence;
            pIDst.numCols = num_sequence;
            pIDst.pData = attn_weights_t;
            arm_mat_trans_q7(&pISrc, &pIDst);
            beco_mat_mul_s8(attn_weights_t,
                            output_v_t + j * mid_dim * num_sequence,
                            NULL,
                            wv + j * num_sequence * mid_dim,
                            out_wv_requant_params->multiplier,
                            out_wv_requant_params->shift,
                            num_sequence, mid_dim, num_sequence,
                            NN_Q7_MIN, NN_Q7_MAX, buffer);
        }

        // transpose
        // wv: [num_heads, sequence, mid_dim] -> [num_heads, mid_dim, sequence]
        int8_t *wv_t = wv + num_sequence * num_feature;
        pISrc.numRows = num_sequence;
        pISrc.numCols = mid_dim;
        pIDst.numRows = mid_dim;
        pIDst.numCols = num_sequence;
        for (int n_head = 0; n_head < num_heads; n_head++) {
            pISrc.pData = wv + n_head * mid_dim * num_sequence;
            pIDst.pData = wv_t + n_head * mid_dim * num_sequence;
            arm_mat_trans_q7(&pISrc, &pIDst);
        }

        // run out_proj
        beco_mat_mul_s8(wv_t,
                        out_proj_weights,
                        out_proj_bias,
                        output,
                        out_requant_params->multiplier,
                        out_requant_params->shift,
                        num_sequence, num_feature, num_feature,
                        NN_Q7_MIN, NN_Q7_MAX, buffer);

        input_q += num_sequence * num_feature;
        input_k += num_sequence * num_feature;
        input_v += num_sequence * num_feature;
        output += num_sequence * num_feature;
        attn_weights += num_heads * num_sequence * num_sequence;
        batch_cnt--;
    }

    return ARM_CMSIS_NN_SUCCESS;
}

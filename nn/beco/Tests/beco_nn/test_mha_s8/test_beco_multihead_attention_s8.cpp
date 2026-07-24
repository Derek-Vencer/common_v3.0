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

#include "beco_test.h"
#include "beco_print_matrix.hpp"

#include "multihead_attention_s8_test01.h"

// #define PRINT_RESULTS


void beco_multihead_attention_s8_test01(void)
{
    int8_t output[BATCH_SIZE*SEQ_DIM*FEATURE_DIM];
    int8_t attn_weights[BATCH_SIZE*NUM_HEADS*SEQ_DIM*SEQ_DIM];

    cmsis_nn_scaling in_proj_q_requant_params = { MULTIPLIER_Q, SHIFT_Q };
    cmsis_nn_scaling in_proj_k_requant_params = { MULTIPLIER_K, SHIFT_K };
    cmsis_nn_scaling in_proj_v_requant_params = { MULTIPLIER_V, SHIFT_V };
    cmsis_nn_scaling out_qk_requant_params = { MULTIPLIER_QK, SHIFT_QK };
    cmsis_nn_scaling out_attn_weights_requant_params = { MULTIPLIER_ATTN_WEIGHTS, SHIFT_ATTN_WEIGHTS };
    cmsis_nn_scaling out_wv_requant_params = { MULTIPLIER_WV, SHIFT_WV };
    cmsis_nn_scaling out_requant_params = { MULTIPLIER_OUT, SHIFT_OUT };

    int32_t buf_size = beco_multi_head_attention_s8_get_buffer_size(SEQ_DIM, FEATURE_DIM);
    void *buffer = malloc(buf_size);

    BECO_GET_TIME_ENTER(1);
    beco_multi_head_attention_s8(input_q, input_k, input_v,
                                 in_proj_q_weights,
                                 in_proj_k_weights,
                                 in_proj_v_weights,
                                 out_proj_weights,
                                 in_proj_q_bias,
                                 in_proj_k_bias,
                                 in_proj_v_bias,
                                 out_proj_bias,
                                 BATCH_SIZE, SEQ_DIM, FEATURE_DIM, NUM_HEADS,
                                 &in_proj_q_requant_params,
                                 &in_proj_k_requant_params,
                                 &in_proj_v_requant_params,
                                 &out_qk_requant_params,
                                 &out_attn_weights_requant_params,
                                 &out_wv_requant_params,
                                 &out_requant_params,
                                 output, attn_weights, buffer);
    BECO_GET_TIME_EXIT(1, "test01 beco_multi_head_attention_s8");
    free(buffer);

#ifdef PRINT_RESULTS
    TRACE(3, "beco results:%s", "");
    print_matrix(output, SEQ_DIM*FEATURE_DIM,
                 BATCH_SIZE,
                 SEQ_DIM*FEATURE_DIM);
#endif
}

void test_beco_multihead_attention_s8(void)
{
    TRACE(3, "===== test_beco_multihead_attention_s8 START =====");
    beco_multihead_attention_s8_test01();
    TRACE(3, "===== test_beco_multihead_attention_s8 END =====");
}


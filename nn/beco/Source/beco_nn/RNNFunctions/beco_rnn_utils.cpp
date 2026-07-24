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

void beco_fully_connected_pt_q15(const int16_t *pV,
                                 const int8_t *pM,
                                 const uint16_t dim_vec,
                                 const uint16_t num_of_cols,
                                 const int32_t multiplier,
                                 const int32_t out_shift,
                                 const int32_t *bias,
                                 int16_t *pOut,
                                 int16_t *buffer)
{
    if (num_of_cols % 32 == 0) {
        beco_fully_connected_q15(pV, pM, dim_vec, num_of_cols,
                                 multiplier, out_shift, bias, pOut,
                                 -32768, 32767, buffer);
    } else {
        beco_fully_connected_norm_q15(pV, pM, dim_vec, num_of_cols,
                                      multiplier, out_shift, bias, pOut,
                                      -32768, 32767, buffer);
    }
}

void beco_fully_connected_pc_q15(const int16_t *pV,
                                 const int8_t *pM,
                                 const uint16_t dim_vec,
                                 const uint16_t num_of_cols,
                                 const int32_t *multiplier,
                                 const int32_t *out_shift,
                                 const int32_t *bias,
                                 int16_t *pOut,
                                 int16_t *buffer)
{
    if (num_of_cols % 32 == 0) {
        beco_fully_connected_q15_pc(pV, pM, dim_vec, num_of_cols,
                                    multiplier, out_shift, bias, pOut,
                                    -32768, 32767, buffer);
    } else {
        beco_fully_connected_norm_q15_pc(pV, pM, dim_vec, num_of_cols,
                                        multiplier, out_shift, bias, pOut,
                                        -32768, 32767, buffer);
    }
}


int32_t beco_gru_s16_get_buffer_size(const uint16_t num_outputs)
{
    int32_t buffer_len = 6 * num_outputs * sizeof(int16_t);
#if defined(ARM_MATH_MVEI)
    buffer_len += 32 * sizeof(int32_t);
#endif
    if ((3 * num_outputs) % 32 != 0) {
        buffer_len += 32 * sizeof(int16_t);
    }

    return buffer_len;
}

int32_t beco_lstm_s16_get_buffer_size(const uint16_t num_outputs)
{
    int32_t buffer_len = 8 * num_outputs * sizeof(int16_t);
#if defined(ARM_MATH_MVEI)
    buffer_len += 32 * sizeof(int32_t);
#endif
    if ((4 * num_outputs) % 32 != 0) {
        buffer_len += 32 * sizeof(int16_t);
    }

    return buffer_len;
}

int32_t beco_bi_gru_s16_get_buffer_size(const uint16_t num_times,
                                        const uint16_t num_batches,
                                        const uint16_t num_outputs)
{
    int32_t buffer_len = 6 * num_outputs * sizeof(int16_t);
#if defined(ARM_MATH_MVEI)
    buffer_len += 32 * sizeof(int32_t);
#endif
    if ((3 * num_outputs) % 32 != 0) {
        buffer_len += 32 * sizeof(int16_t);
    }
    buffer_len += num_times * num_batches * 2 * num_outputs * sizeof(int16_t);

    return buffer_len;
}

int32_t beco_bi_lstm_s16_get_buffer_size(const uint16_t num_times,
                                         const uint16_t num_batches,
                                         const uint16_t num_outputs)
{
    int32_t buffer_len = 8 * num_outputs * sizeof(int16_t);
#if defined(ARM_MATH_MVEI)
    buffer_len += 32 * sizeof(int32_t);
#endif
    if ((4 * num_outputs) % 32 != 0) {
        buffer_len += 32 * sizeof(int16_t);
    }
    buffer_len += num_times * num_batches * 2 * num_outputs * sizeof(int16_t);

    return buffer_len;
}


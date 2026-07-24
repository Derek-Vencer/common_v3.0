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
#include "beco_bias.hpp"

void beco_fully_connected_norm_q7_process(const int8_t *pV,
                                          const int8_t *pM,
                                          const uint16_t dim_vec,
                                          const uint16_t num_of_cols,
                                          const int32_t multiplier,
                                          const int32_t out_shift,
                                          const int32_t *bias,
                                          int8_t *pOut,
                                          const int8_t out_offset,
                                          const int8_t min,
                                          const int8_t max,
                                          int8_t *buffer)
{
    const uint32_t beco_num_of_cols = num_of_cols >> 5;
    const uint32_t remainder = num_of_cols & 31;
    const uint32_t beco_cols = (num_of_cols - remainder) >> 3;

#if defined(ARM_MATH_MVEI)
    beco_vec32_out_t *beco_out = (beco_vec32_out_t *) buffer;
#else
    beco_vec32_out_t *beco_out = (beco_vec32_out_t *) pOut;
#endif

    const beco_vec64_in_t *beco_m = (const beco_vec64_in_t *) pM;
    const beco_vec64_in_t *beco_bias = (const beco_vec64_in_t *) bias;

    unsigned i;
    for (i = 0; i < beco_num_of_cols; i++) {
        if (bias) {
            beco_fully_connected_set_bias(beco_bias);
            beco_bias += 16;
        }
        else {
            beco_clear_acc(BECO_ACC0);
            beco_clear_acc(BECO_ACC1);
            beco_clear_acc(BECO_ACC2);
            beco_clear_acc(BECO_ACC3);
        }

        beco_fully_connected_norm_q7_impl(pV, &beco_m[4*i], dim_vec, beco_cols, remainder);
#if defined(ARM_MATH_MVEI)
        beco_fully_connected_readout32(beco_out);
        beco_fully_connected_q7_requant((int32_t *)beco_out, multiplier, out_shift, out_offset, min, max, pOut);
        pOut += 32;
#else
        beco_fully_connected_q7_readout(beco_out, multiplier, out_shift, out_offset, min, max);
        beco_out += 8;
#endif
    }

    if (remainder) {
        if (bias) {
            beco_fully_connected_set_bias(beco_bias);
        }
        else {
            beco_clear_acc(BECO_ACC0);
            beco_clear_acc(BECO_ACC1);
            beco_clear_acc(BECO_ACC2);
            beco_clear_acc(BECO_ACC3);
        }

        beco_fully_connected_norm_q7_impl(pV, &beco_m[beco_cols], dim_vec, beco_cols, remainder);
#if defined(ARM_MATH_MVEI)
        int8_t *beco_out_temp = (int8_t *)(buffer + 32 * sizeof(int32_t));
        beco_fully_connected_readout32(beco_out);
        beco_fully_connected_q7_requant((int32_t *)beco_out, multiplier, out_shift, out_offset, min, max, beco_out_temp);
        memcpy(pOut, beco_out_temp, remainder);
#else
        beco_vec32_out_t *beco_out_temp = (beco_vec32_out_t *)buffer;
        beco_fully_connected_q7_readout(beco_out_temp, multiplier, out_shift, out_offset, min, max);
        memcpy(beco_out, beco_out_temp, remainder);
#endif
    }
}

void beco_fully_connected_norm_q7(const int8_t *pV,
                                  const int8_t *pM,
                                  const uint16_t dim_vec,
                                  const uint16_t num_of_cols,
                                  const int32_t multiplier,
                                  const int32_t out_shift,
                                  const int32_t *bias,
                                  int8_t *pOut,
                                  const int8_t out_offset,
                                  const int8_t min,
                                  const int8_t max,
                                  int8_t *buffer)
{
    uint32_t config =
        BECO_CONF_AMODE_REP16 | BECO_CONF_BMODE_REP64 |
        BECO_CONF_ATYPE_INT16 | BECO_CONF_BTYPE_INT8 |
        BECO_CONF_RSHIFT(0) |
        BECO_CONF_PACK_INT32 | BECO_CONF_RD_16x8_ROT90;
    beco_write_config(config);

    beco_fully_connected_norm_q7_process(pV, pM, dim_vec, num_of_cols,
                                         multiplier, out_shift, bias, pOut,
                                         out_offset, min, max, buffer);
}


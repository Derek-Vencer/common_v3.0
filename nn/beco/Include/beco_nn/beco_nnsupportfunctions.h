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


#ifndef _BECO_NNSUPPORTFUNCTIONS_H
#define _BECO_NNSUPPORTFUNCTIONS_H

#include "beco.h"
#include "beco_types.h"
#include "arm_nnsupportfunctions.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Beco nn requantize
 *
 * Beco requantize functions used in readout.
 *
 */

__STATIC_FORCEINLINE int8_t beco_requantize_q7(int32_t val, int32_t mult,
                                               int32_t shift, int8_t out_offset,
                                               int8_t min, int8_t max)
{
    int32_t in, out;
    int64_t long_long = 1 << 30;

#if defined(CHIP_BEST1603) || defined(CHIP_BEST1306)
    val = val > 67108864 ? val + 4160749568 : val;
#endif

    in = val;
    in = ((int64_t) in * mult + long_long) >> 31;
    out = (in + NN_ROUND(-shift)) >> -shift;

    out += out_offset;
    out = MAX(out, min);
    out = MIN(out, max);
    return (int8_t)out;
}


__STATIC_FORCEINLINE int16_t beco_requantize_q15(int32_t val, int32_t mult,
                                                 int32_t shift, int16_t min, int16_t max)
{
    int32_t in, out;

#if defined(CHIP_BEST1603) || defined(CHIP_BEST1306)
    val = val > 67108864 ? val + 4160749568 : val;
#endif

    in = val;
    mult = REDUCE_MULTIPLIER(mult);
    in = ((int64_t) in * mult) >> (14 - shift);
    out = (in + 1) >> 1;

    out = MAX(out, min);
    out = MIN(out, max);
    return (int16_t)out;
}

#define SOFTMAX_S16_TABLE_LEN 513
extern const int16_t softmax_s16_exp_lut[SOFTMAX_S16_TABLE_LEN];
extern const int16_t softmax_s16_one_by_one_lut[SOFTMAX_S16_TABLE_LEN];


/**
 * @defgroup Beco nn set bias
 *
 * Preload bias data in beco ACCs.
 *
 */

void beco_fully_connected_set_bias(const beco_vec64_in_t *bias);


void beco_convolve_q7_set_bias(const beco_vec64_in_t *beco_bias);

void beco_convolve_q15_set_bias(const beco_vec64_in_t *beco_bias);


/**
 * @defgroup Beco nn impl
 *
 * Write beco reg and do mmac.
 *
 */

void beco_matmul_impl(const beco_vec64_in_t *beco_mat_l,
                      const beco_vec64_in_t *beco_mat_r,
                      const uint16_t beco_mat_l_stride,
                      const uint16_t beco_mat_r_stride,
                      const int32_t mat_l_cols);

void beco_fully_connected_q7_impl(const int8_t *p_v,
                                  const beco_vec64_in_t *beco_m,
                                  const uint16_t dim_vec,
                                  const uint32_t beco_m_cols);

void beco_fully_connected_norm_q7_impl(const int8_t *p_v,
                                       const beco_vec64_in_t *beco_w,
                                       const uint16_t dim_vec,
                                       const uint32_t beco_cols,
                                       const uint32_t remainder);

void beco_fully_connected_q15_impl(const int16_t *p_v,
                                   const beco_vec64_in_t *beco_m,
                                   const uint16_t dim_vec,
                                   const uint32_t beco_m_cols);

void beco_fully_connected_norm_q15_impl(const int16_t *p_v,
                                        const beco_vec64_in_t *beco_w,
                                        const uint16_t dim_vec,
                                        const uint32_t beco_cols,
                                        const uint32_t remainder);

void beco_convolve_impl(beco_vec64_in_t *beco_in,
                        beco_vec64_in_t *beco_wt,
                        const uint16_t ch_im_in,
                        const uint16_t dim_kernel_x,
                        const uint32_t beco_och_stride,
                        const uint32_t beco_wt_stride,
                        const uint32_t grouped_index,
                        const uint32_t beco_in_stride);

void beco_convolve_offset_impl(beco_vec64_in_t beco_in,
                               beco_vec64_in_t *beco_wt,
                               const uint16_t ch_im_in,
                               const uint16_t dim_kernel_x,
                               const uint32_t beco_och_stride,
                               const uint32_t beco_wt_stride);

void beco_convolve_1x1_impl(const beco_vec64_in_t *beco_in,
                            const beco_vec64_in_t *beco_wt,
                            const uint16_t ch_im_in,
                            const int beco_in_stride,
                            const int beco_wt_stride);

void beco_convolve_1x1_norm_impl(const beco_vec64_in_t *beco_in,
                                 const beco_vec64_in_t *beco_wt,
                                 const uint16_t ch_im_in,
                                 const int beco_wt_stride);

/**
 * @defgroup Beco nn readout
 *
 * Readout data from beco ACCs to the output pointer.
 *
 */

void beco_fully_connected_q7_readout(beco_vec32_out_t *out,
                                     const int32_t multiplier,
                                     const int32_t shift,
                                     const int8_t offset,
                                     const int8_t min,
                                     const int8_t max);

void beco_fully_connected_q7_readout_pc(beco_vec32_out_t *out,
                                        const int32_t *multiplier,
                                        const int32_t *shift,
                                        const int8_t offset,
                                        const int8_t min,
                                        const int8_t max);

void beco_fully_connected_q15_readout(beco_vec32_out_t *out,
                                      const int32_t multiplier,
                                      const int32_t shift,
                                      const int16_t min,
                                      const int16_t max);

void beco_fully_connected_q15_readout_pc(beco_vec32_out_t *out,
                                         const int32_t *multiplier,
                                         const int32_t *shift,
                                         const int16_t min,
                                         const int16_t max);

void beco_convolve_q7_readout(beco_vec32_out_t *out,
                              uint32_t stride,
                              const int32_t *multiplier,
                              const int32_t *shift,
                              const int8_t offset,
                              const int8_t min,
                              const int8_t max);

void beco_convolve_q15_readout(beco_vec32_out_t *out,
                               uint32_t stride,
                               const int32_t *multiplier,
                               const int32_t *shift,
                               const int16_t min,
                               const int16_t max);


void beco_depthwise_conv_q7_readoutACC0(beco_vec32_out_t *out,
                                        const int32_t multiplier,
                                        const int32_t shift,
                                        const int8_t offset,
                                        const int8_t min,
                                        const int8_t max);

void beco_depthwise_conv_q7_readout4ACC(beco_vec32_out_t *out,
                                        const int32_t multiplier,
                                        const int32_t shift,
                                        const int8_t offset,
                                        const int8_t min,
                                        const int8_t max);

void beco_depthwise_conv_q15_readoutACC0(beco_vec32_out_t *out,
                                         const int32_t multiplier,
                                         const int32_t shift,
                                         const int16_t min,
                                         const int16_t max);

void beco_depthwise_conv_q15_readout4ACC(beco_vec32_out_t *out,
                                         const int32_t multiplier,
                                         const int32_t shift,
                                         const int16_t min,
                                         const int16_t max);


void beco_batch_matmul_s8_readout(beco_vec32_out_t *out,
                                  const uint16_t stride,
                                  const int32_t multiplier,
                                  const int32_t shift,
                                  const int32_t offset,
                                  const int32_t min,
                                  const int32_t max);

void beco_mat_mul_q7_readout(beco_vec32_out_t *out,
                             const uint16_t stride,
                             const int32_t multiplier,
                             const int32_t shift,
                             const int32_t min,
                             const int32_t max);

void beco_mat_mul_q15_readout(beco_vec32_out_t *out,
                              const uint16_t stride,
                              const int32_t multiplier,
                              const int32_t shift,
                              const int32_t min,
                              const int32_t max);

#if defined(ARM_MATH_MVEI)
void beco_fully_connected_readout32(beco_vec32_out_t *out);

void beco_fully_connected_q7_requant(int32_t *val,
                                     const int32_t multiplier,
                                     const int32_t shift,
                                     const int8_t offset,
                                     const int8_t min,
                                     const int8_t max,
                                     int8_t *output);

void beco_fully_connected_q7_pc_requant(int32_t *val,
                                        const int32_t *multiplier,
                                        const int32_t *shift,
                                        const int8_t offset,
                                        const int8_t min,
                                        const int8_t max,
                                        int8_t *output);

void beco_fully_connected_q15_requant(int32_t *val,
                                      const int32_t multiplier,
                                      const int32_t shift,
                                      const int16_t min,
                                      const int16_t max,
                                      int16_t *output);

void beco_fully_connected_q15_pc_requant(int32_t *val,
                                         const int32_t *multiplier,
                                         const int32_t *shift,
                                         const int16_t min,
                                         const int16_t max,
                                         int16_t *output);


void beco_convolve_q7_readout32(beco_vec32_out_t *out);

void beco_convolve_q15_readout32(beco_vec32_out_t *out);

void beco_convolve_q7_requant(int32_t *val,
                              const int32_t *multiplier,
                              const int32_t *shift,
                              const int8_t offset,
                              const int8_t min,
                              const int8_t max,
                              const uint16_t stride,
                              int8_t *output);

void beco_convolve_q15_requant(int32_t *val,
                               const int32_t *multiplier,
                               const int32_t *shift,
                               const int16_t min,
                               const int16_t max,
                               const uint16_t stride,
                               int16_t *output);


void beco_depthwise_conv_q7_readout32_ACC0(beco_vec32_out_t *out);

void beco_depthwise_conv_q7_readout32_4ACC(beco_vec32_out_t *out);

void beco_depthwise_conv_q15_readout32_ACC0(beco_vec32_out_t *out);

void beco_depthwise_conv_q15_readout32_4ACC(beco_vec32_out_t *out);

void beco_depthwise_conv_q7_requant(int32_t *val,
                                    const int32_t multiplier,
                                    const int32_t shift,
                                    const int8_t offset,
                                    const int8_t min,
                                    const int8_t max,
                                    const int8_t accs,
                                    int8_t *output);

void beco_depthwise_conv_q15_requant(int32_t *val,
                                     const int32_t multiplier,
                                     const int32_t shift,
                                     const int16_t min,
                                     const int16_t max,
                                     const int16_t accs,
                                     int16_t *output);


void beco_batch_matmul_s8_readout32(beco_vec32_out_t *out);

void beco_batch_matmul_s8_requant(int32_t *val,
                                  const int32_t multiplier,
                                  const int32_t shift,
                                  const int32_t offset,
                                  const int8_t min,
                                  const int8_t max,
                                  const uint16_t stride,
                                  int8_t *output);

void beco_mat_mul_q7_readout32(beco_vec32_out_t *out);

void beco_mat_mul_q7_requant(int32_t *val,
                             const int32_t multiplier,
                             const int32_t shift,
                             const int8_t min,
                             const int8_t max,
                             const uint16_t stride,
                             int8_t *output);

void beco_mat_mul_q15_readout32(beco_vec32_out_t *out);

void beco_mat_mul_q15_requant(int32_t *val,
                              const int32_t multiplier,
                              const int32_t shift,
                              const int16_t min,
                              const int16_t max,
                              const uint16_t stride,
                              int16_t *output);

#endif

#ifdef __cplusplus
}
#endif

#endif

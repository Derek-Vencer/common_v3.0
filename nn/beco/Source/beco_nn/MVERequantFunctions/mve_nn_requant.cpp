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

#include "beco_nn/beco_nnsupportfunctions.h"

#if defined(ARM_MATH_MVEI)

void beco_fully_connected_q7_requant(int32_t *val,
                                     const int32_t multiplier,
                                     const int32_t shift,
                                     const int8_t offset,
                                     const int8_t min,
                                     const int8_t max,
                                     int8_t *output)
{
    for (int i = 0; i < 4; i++) {
        int32x4_t acc1 = {val[0], val[1], val[4], val[5]};
        int32x4_t acc2 = {val[2], val[3], val[6], val[7]};

        acc1 = arm_requantize_mve(acc1, multiplier, shift);
        acc2 = arm_requantize_mve(acc2, multiplier, shift);

        acc1 = vaddq_s32(acc1, vdupq_n_s32(offset));
        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));
        acc2 = vaddq_s32(acc2, vdupq_n_s32(offset));
        acc2 = vmaxq_s32(acc2, vdupq_n_s32(min));
        acc2 = vminq_s32(acc2, vdupq_n_s32(max));

        vstrbq_s32(output, acc1);
        output += 4;
        vstrbq_s32(output, acc2);
        output += 4;
        val += 8;
    }
}

void beco_fully_connected_q7_pc_requant(int32_t *val,
                                        const int32_t *multiplier,
                                        const int32_t *shift,
                                        const int8_t offset,
                                        const int8_t min,
                                        const int8_t max,
                                        int8_t *output)
{
    for (int i = 0; i < 4; i++) {
        int32x4_t acc1 = {val[0], val[1], val[4], val[5]};
        int32x4_t acc2 = {val[2], val[3], val[6], val[7]};
        int32x4_t multiplier1 = {*multiplier++, *multiplier++, *multiplier++, *multiplier++};
        int32x4_t multiplier2 = {*multiplier++, *multiplier++, *multiplier++, *multiplier++};
        int32x4_t shift1 = {*shift++, *shift++, *shift++, *shift++};
        int32x4_t shift2 = {*shift++, *shift++, *shift++, *shift++};
        int32x4_t right_shift = vminq_s32(vdupq_n_s32(-1), shift1);
        int32x4_t left_shift = vqsubq_s32(shift1, right_shift);
        acc1 = vqdmulhq_s32(vshlq_s32(acc1, left_shift), multiplier1);
        acc1 = vrshlq_s32(acc1, right_shift);
        right_shift = vminq_s32(vdupq_n_s32(-1), shift2);
        left_shift = vqsubq_s32(shift2, right_shift);
        acc2 = vqdmulhq_s32(vshlq_s32(acc2, left_shift), multiplier2);
        acc2 = vrshlq_s32(acc2, right_shift);

        acc1 = vaddq_s32(acc1, vdupq_n_s32(offset));
        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));
        acc2 = vaddq_s32(acc2, vdupq_n_s32(offset));
        acc2 = vmaxq_s32(acc2, vdupq_n_s32(min));
        acc2 = vminq_s32(acc2, vdupq_n_s32(max));

        vstrbq_s32(output, acc1);
        output += 4;
        vstrbq_s32(output, acc2);
        output += 4;
        val += 8;
    }
}

void beco_fully_connected_q15_requant(int32_t *val,
                                      const int32_t multiplier,
                                      const int32_t shift,
                                      const int16_t min,
                                      const int16_t max,
                                      int16_t *output)
{
    const int32x4_t right_shift = vminq_s32(vdupq_n_s32(-1), vdupq_n_s32(shift));
    const int32x4_t left_shift = vqsubq_s32(vdupq_n_s32(shift), right_shift);
    for (int i = 0; i < 4; i++) {
        int32x4_t acc1 = {val[0], val[1], val[4], val[5]};
        int32x4_t acc2 = {val[2], val[3], val[6], val[7]};

        acc1 = vqdmulhq_n_s32(vshlq_s32(acc1, left_shift), multiplier);
        acc1 = vrshlq_s32(acc1, right_shift);
        acc2 = vqdmulhq_n_s32(vshlq_s32(acc2, left_shift), multiplier);
        acc2 = vrshlq_s32(acc2, right_shift);

        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));
        acc2 = vmaxq_s32(acc2, vdupq_n_s32(min));
        acc2 = vminq_s32(acc2, vdupq_n_s32(max));

        vstrhq_s32(output, acc1);
        output += 4;
        vstrhq_s32(output, acc2);
        output += 4;
        val += 8;
    }
}

void beco_fully_connected_q15_pc_requant(int32_t *val,
                                         const int32_t *multiplier,
                                         const int32_t *shift,
                                         const int16_t min,
                                         const int16_t max,
                                         int16_t *output)
{
    for (int i = 0; i < 4; i++) {
        int32x4_t acc1 = {val[0], val[1], val[4], val[5]};
        int32x4_t acc2 = {val[2], val[3], val[6], val[7]};
        int32x4_t multiplier1 = {*multiplier++, *multiplier++, *multiplier++, *multiplier++};
        int32x4_t multiplier2 = {*multiplier++, *multiplier++, *multiplier++, *multiplier++};
        int32x4_t shift1 = {*shift++, *shift++, *shift++, *shift++};
        int32x4_t shift2 = {*shift++, *shift++, *shift++, *shift++};
        int32x4_t right_shift = vminq_s32(vdupq_n_s32(-1), shift1);
        int32x4_t left_shift = vqsubq_s32(shift1, right_shift);
        acc1 = vqdmulhq_s32(vshlq_s32(acc1, left_shift), multiplier1);
        acc1 = vrshlq_s32(acc1, right_shift);
        right_shift = vminq_s32(vdupq_n_s32(-1), shift2);
        left_shift = vqsubq_s32(shift2, right_shift);
        acc2 = vqdmulhq_s32(vshlq_s32(acc2, left_shift), multiplier2);
        acc2 = vrshlq_s32(acc2, right_shift);

        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));
        acc2 = vmaxq_s32(acc2, vdupq_n_s32(min));
        acc2 = vminq_s32(acc2, vdupq_n_s32(max));

        vstrhq_s32(output, acc1);
        output += 4;
        vstrhq_s32(output, acc2);
        output += 4;
        val += 8;
    }
}

void beco_convolve_q7_requant(int32_t *val,
                              const int32_t *multiplier,
                              const int32_t *shift,
                              const int8_t offset,
                              const int8_t min,
                              const int8_t max,
                              const uint16_t stride,
                              int8_t *output)
{
    for (int i = 0; i < 8; i++) {
        int32x4_t acc1 = {*val++, *val++, *val++, *val++};
        int32x4_t acc2 = {*val++, *val++, *val++, *val++};

        acc1 = arm_requantize_mve(acc1, multiplier[i], shift[i]);
        acc2 = arm_requantize_mve(acc2, multiplier[i], shift[i]);

        acc1 = vaddq_s32(acc1, vdupq_n_s32(offset));
        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));
        acc2 = vaddq_s32(acc2, vdupq_n_s32(offset));
        acc2 = vmaxq_s32(acc2, vdupq_n_s32(min));
        acc2 = vminq_s32(acc2, vdupq_n_s32(max));

        vstrbq_s32(&output[i * stride], acc1);
        vstrbq_s32(&output[i * stride + 4], acc2);
    }
}

void beco_convolve_q15_requant(int32_t *val,
                               const int32_t *multiplier,
                               const int32_t *shift,
                               const int16_t min,
                               const int16_t max,
                               const uint16_t stride,
                               int16_t *output)
{
    for (int i = 0; i < 8; i++) {
        int32x4_t right_shift = vminq_s32(vdupq_n_s32(-1), vdupq_n_s32(shift[i]));
        int32x4_t left_shift = vqsubq_s32(vdupq_n_s32(shift[i]), right_shift);
        int32x4_t acc = {*val++, *val++, *val++, *val++};

        acc = vqdmulhq_n_s32(vshlq_s32(acc, left_shift), multiplier[i]);
        acc = vrshlq_s32(acc, right_shift);

        acc = vmaxq_s32(acc, vdupq_n_s32(min));
        acc = vminq_s32(acc, vdupq_n_s32(max));
        vstrhq_s32(&output[i*stride], acc);
    }
}

void beco_depthwise_conv_q7_requant(int32_t *val,
                                    const int32_t multiplier,
                                    const int32_t shift,
                                    const int8_t offset,
                                    const int8_t min,
                                    const int8_t max,
                                    const int8_t accs,
                                    int8_t *output)
{
    for (int i = 0; i < accs; i++) {
        int32x4_t acc1 = {val[0], val[2], val[1], val[3]};
        int32x4_t acc2 = {val[4], val[6], val[5], val[7]};

        acc1 = arm_requantize_mve(acc1, multiplier, shift);
        acc2 = arm_requantize_mve(acc2, multiplier, shift);

        acc1 = vaddq_s32(acc1, vdupq_n_s32(offset));
        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));
        acc2 = vaddq_s32(acc2, vdupq_n_s32(offset));
        acc2 = vmaxq_s32(acc2, vdupq_n_s32(min));
        acc2 = vminq_s32(acc2, vdupq_n_s32(max));

        vstrbq_s32(output, acc1);
        output += 4;
        vstrbq_s32(output, acc2);
        output += 4;
        val += 8;
    }
}

void beco_depthwise_conv_q15_requant(int32_t *val,
                                     const int32_t multiplier,
                                     const int32_t shift,
                                     const int16_t min,
                                     const int16_t max,
                                     const int16_t accs,
                                     int16_t *output)
{
    const int32x4_t right_shift = vminq_s32(vdupq_n_s32(-1), vdupq_n_s32(shift));
    const int32x4_t left_shift = vqsubq_s32(vdupq_n_s32(shift), right_shift);
    for (int i = 0; i < accs; i++) {
        int32x4_t acc1 = {*val++, *val++, *val++, *val++};

        acc1 = vqdmulhq_n_s32(vshlq_s32(acc1, left_shift), multiplier);
        acc1 = vrshlq_s32(acc1, right_shift);

        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));

        vstrhq_s32(output, acc1);
        output += 4;
    }
}

void beco_batch_matmul_s8_requant(int32_t *val,
                                  const int32_t multiplier,
                                  const int32_t shift,
                                  const int32_t offset,
                                  const int8_t min,
                                  const int8_t max,
                                  const uint16_t stride,
                                  int8_t *output)
{
    for (int i = 0; i < 8; i++) {
        int32x4_t acc1 = {*val++, *val++, *val++, *val++};
        int32x4_t acc2 = {*val++, *val++, *val++, *val++};

        acc1 = arm_requantize_mve(acc1, multiplier, shift);
        acc2 = arm_requantize_mve(acc2, multiplier, shift);

        acc1 = vaddq_s32(acc1, vdupq_n_s32(offset));
        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));
        acc2 = vaddq_s32(acc2, vdupq_n_s32(offset));
        acc2 = vmaxq_s32(acc2, vdupq_n_s32(min));
        acc2 = vminq_s32(acc2, vdupq_n_s32(max));

        vstrbq_s32(&output[i * stride], acc1);
        vstrbq_s32(&output[i * stride + 4], acc2);
    }
}

void beco_mat_mul_q7_requant(int32_t *val,
                             const int32_t multiplier,
                             const int32_t shift,
                             const int8_t min,
                             const int8_t max,
                             const uint16_t stride,
                             int8_t *output)
{
    for (int i = 0; i < 8; i++) {
        int32x4_t acc1 = {*val++, *val++, *val++, *val++};
        int32x4_t acc2 = {*val++, *val++, *val++, *val++};

        acc1 = arm_requantize_mve(acc1, multiplier, shift);
        acc2 = arm_requantize_mve(acc2, multiplier, shift);

        acc1 = vmaxq_s32(acc1, vdupq_n_s32(min));
        acc1 = vminq_s32(acc1, vdupq_n_s32(max));
        acc2 = vmaxq_s32(acc2, vdupq_n_s32(min));
        acc2 = vminq_s32(acc2, vdupq_n_s32(max));

        vstrbq_s32(&output[i * stride], acc1);
        vstrbq_s32(&output[i * stride + 4], acc2);
    }
}

void beco_mat_mul_q15_requant(int32_t *val,
                              const int32_t multiplier,
                              const int32_t shift,
                              const int16_t min,
                              const int16_t max,
                              const uint16_t stride,
                              int16_t *output)
{
    int32x4_t right_shift = vminq_s32(vdupq_n_s32(-1), vdupq_n_s32(shift));
    int32x4_t left_shift = vqsubq_s32(vdupq_n_s32(shift), right_shift);
    for (int i = 0; i < 4; i++) {
        int32x4_t acc = {*val++, *val++, *val++, *val++};

        acc = vqdmulhq_n_s32(vshlq_s32(acc, left_shift), multiplier);
        acc = vrshlq_s32(acc, right_shift);

        acc = vmaxq_s32(acc, vdupq_n_s32(min));
        acc = vminq_s32(acc, vdupq_n_s32(max));
        vstrhq_s32(&output[i*stride], acc);
    }
}

#endif

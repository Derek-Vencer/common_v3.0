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

static void swap(int16_t* &a, int16_t* &b) { int16_t* c; c = a; a = b; b = c; }

static void beco_convolve_norm_q15_copy_input(const int16_t *pin,
                                              int16_t *pout,
                                              const uint16_t stride_x,
                                              const uint16_t simd_width,
                                              const uint16_t copy_width,
                                              const uint16_t input_ch,
                                              const uint16_t input_x,
                                              const uint16_t input_y,
                                              const uint16_t kernel_x)
{
    if (stride_x == 1) {
        for (int c = 0; c < input_ch; c++) {
            memcpy(pout, pin, copy_width * sizeof(int16_t));
            pin += input_x * input_y;
            pout += simd_width;
        }
    } else {
        for (int c = 0; c < input_ch; c++) {
            const int16_t *pin1 = pin;
            int16_t *pout1 = pout;
            for (int i = 0; i < copy_width; i++) {
                *pout1++ = *pin1;
                pin1 += stride_x;
            }
            pin += input_x * input_y;
            pout += simd_width;
        }
    }
}

static void data_arrange(int y,
                         int x,
                         const int16_t *pin,
                         int16_t *pout,
                         const uint16_t input_ch,
                         const uint16_t input_x,
                         const uint16_t input_y,
                         const uint16_t kernel_x,
                         const uint16_t padding_x,
                         const uint16_t stride_x,
                         const uint16_t max_step_x,
                         const uint16_t step_x,
                         const uint16_t sw,
                         const uint16_t cw,
                         const uint16_t dilation_x)
{
    int16_t *po = pout;
    pin += input_x * y - padding_x + x;
    for (int kx = 0; kx < kernel_x; kx++) {
        beco_convolve_norm_q15_copy_input(pin, po, stride_x, sw, cw,
                                          input_ch, input_x, input_y, kernel_x);
        pin += dilation_x;
        po += input_ch * sw;
    }

    // set values left-side
    int cur_step_x = step_x * sw * stride_x - padding_x;
    if (cur_step_x < 0) {
        for (int i = 0; i < sw; i++) {
            for (int j = 0; j < kernel_x; j++) {
                int in_col = (sw*step_x + i) * stride_x + j * dilation_x - padding_x;
                if (in_col < 0) {
                    for (int c = 0; c < input_ch; c++) {
                        pout[j*input_ch*sw + c*sw + i] = 0;
                    }
                } else {
                    break;
                }
            }
        }
    }

    // set values right-side
    cur_step_x = (step_x+1) * sw * stride_x + (kernel_x-1) * dilation_x - padding_x;
    if (cur_step_x >= input_x) {
        for (int i = 0; i < sw; i++) {
            for (int j = kernel_x-1; j >= 0; j--) {
                int in_col = (sw*step_x + i) * stride_x + j * dilation_x - padding_x;
                if (in_col >= input_x) {
                    for (int c = 0; c < input_ch; c++) {
                        pout[j*input_ch*sw + c*sw + i] = 0;
                    }
                } else {
                    break;
                }
            }
        }
    }
}

static void update_image_cache(int iy,
                               int ix,
                               const int16_t *input,
                               int16_t **image_cache,
                               bool *image_cache_valid,
                               const uint16_t input_ch,
                               const uint16_t input_x,
                               const uint16_t input_y,
                               const uint16_t kernel_x,
                               const uint16_t kernel_dila_y,
                               const uint16_t padding_x,
                               const uint16_t padding_y,
                               const uint16_t stride_x,
                               const uint16_t stride_y,
                               const uint16_t max_step_x,
                               const uint16_t step_x,
                               const uint16_t simd_width,
                               const uint16_t copy_width,
                               const uint16_t dilation_x,
                               const uint16_t dilation_y)
{
    // Discard expired entries.
    if (iy != -padding_y) {
        for (int i = 0; i < kernel_dila_y - stride_y; i++) {
            swap(image_cache[i], image_cache[i + stride_y]);
            image_cache_valid[i] = image_cache_valid[i + stride_y];
            image_cache_valid[i + stride_y] = false;
        }
        memset(&image_cache_valid[kernel_dila_y - stride_y], false, stride_y * sizeof(bool));
    }

    // Load new entries.
    for (int i = 0; i < kernel_dila_y; i += dilation_y) {
        if (iy + i >= input_y)
            image_cache_valid[i] = false;
        else if (iy + i >= 0 && image_cache_valid[i] == false) {
            data_arrange(iy + i, ix, input, image_cache[i], input_ch, input_x, input_y,
                         kernel_x, padding_x, stride_x, max_step_x,
                         step_x, simd_width, copy_width, dilation_x);
            image_cache_valid[i] = true;
        }
    }
}

static bool get_arranged_input(int iy, int ky,
                               int16_t **image_cache,
                               bool *image_cache_valid,
                               int16_t *&cur_input)
{
    bool valid = iy + ky >= 0 && image_cache_valid[ky];
    if (valid) {
        cur_input = image_cache[ky];
        return true;
    } else {
        return false;
    }
}

static void beco_convolve_norm_q15_copy_out(int16_t *pin,
                                            int16_t *pout,
                                            const int copy_width,
                                            const int stride)
{
    for (int i = 0; i < 8; i++) {
        memcpy(pout, pin, 2 * copy_width);
        pin += 4;
        pout += stride;
    }
}

void beco_convolve_norm_q15_process(const int16_t *Im_in,
                                    const uint16_t dim_im_in_x,
                                    const uint16_t dim_im_in_y,
                                    const uint16_t ch_im_in,
                                    const int8_t *wt,
                                    const uint16_t ch_im_out,
                                    const uint16_t dim_kernel_x,
                                    const uint16_t dim_kernel_y,
                                    const uint16_t padding_x,
                                    const uint16_t padding_y,
                                    const uint16_t stride_x,
                                    const uint16_t stride_y,
                                    const int32_t *bias,
                                    const int32_t *multiplier,
                                    const int32_t *out_shift,
                                    int16_t *Im_out,
                                    const uint16_t dim_im_out_x,
                                    const uint16_t dim_im_out_y,
                                    const uint16_t dilation_x,
                                    const uint16_t dilation_y,
                                    const uint16_t groups,
                                    const int16_t min,
                                    const int16_t max,
                                    int16_t *buffer)
{
    const int dim_kernel_dila_y = dim_kernel_y + (dim_kernel_y-1) * (dilation_y-1);

    const int beco_im_out_x_num = dim_im_out_x >> 2;
    const int remainder = dim_im_out_x & 3;
    const int max_step_x = (beco_im_out_x_num - 1) + (remainder ? 1 : 0);

    const int kernel_och_stride = (ch_im_out * dim_kernel_x * dim_kernel_y) >> 3;
    const int och_stride = ch_im_out >> 3;
    const int out_stride_parchannel = dim_im_out_x * dim_im_out_y;
    const int grouped_in_ch = ch_im_in / groups;
    const int grouped_out_ch = ch_im_out / groups;
    const int is_group = groups == 1 ? 0 : 1;

    uint16_t im2col_buffer_len = 4 * ch_im_in * dim_kernel_dila_y * dim_kernel_x;
    int16_t *im2col_buffer = (int16_t *)buffer;
    int16_t *image_cache[dim_kernel_dila_y];
    bool image_cache_valid[dim_kernel_dila_y];
    // point image_cache to im2col_buffer
    for (int i = 0; i < dim_kernel_dila_y; i++) {
        image_cache[i] = &im2col_buffer[i*4*ch_im_in*dim_kernel_x];
    }

#if defined(ARM_MATH_MVEI)
    beco_vec32_out_t *beco_out = (beco_vec32_out_t *)(buffer + im2col_buffer_len);
#else
    int16_t *out_buffer = (int16_t *)(buffer + im2col_buffer_len);
    beco_vec32_out_t *beco_out = (beco_vec32_out_t *)out_buffer;
#endif
    const beco_vec64_in_t *beco_bias = (const beco_vec64_in_t *)bias;

    int ix=0, iy=0;
    for (int i_out_x = 0; i_out_x < beco_im_out_x_num; i_out_x++) {
        iy = -padding_y;
        int16_t *cur_im_out = Im_out;
        // clear image_cache_valid
        memset(image_cache_valid, false, dim_kernel_dila_y * sizeof(bool));

        for (int i_out_y = 0; i_out_y < dim_im_out_y; i_out_y++, iy += stride_y) {
            // do img2col here
            update_image_cache(iy, ix, Im_in, image_cache, image_cache_valid, ch_im_in,
                               dim_im_in_x, dim_im_in_y,
                               dim_kernel_x, dim_kernel_dila_y, padding_x, padding_y,
                               stride_x, stride_y, max_step_x, i_out_x,
                               4, 4, dilation_x, dilation_y);

            for (int i_ch = 0; i_ch < ch_im_out; i_ch += 8) {
                beco_vec64_in_t *beco_wt = (beco_vec64_in_t *)(wt + i_ch);
                if (bias) {
                    beco_convolve_q15_set_bias(&beco_bias[i_ch/2]);
                }
                else {
                    beco_clear_acc(BECO_ACC0);
                    beco_clear_acc(BECO_ACC1);
                    beco_clear_acc(BECO_ACC2);
                    beco_clear_acc(BECO_ACC3);
                }

                for (int ky = 0; ky < dim_kernel_dila_y; ky += dilation_y) {
                    int16_t *cur_input;
                    if (get_arranged_input(iy, ky, image_cache, image_cache_valid, cur_input)) {
                        beco_vec64_in_t *beco_in = (beco_vec64_in_t *)cur_input;
                        int32_t grouped_index = is_group*(i_ch/grouped_out_ch)*grouped_in_ch;
                        beco_convolve_impl(beco_in, beco_wt, grouped_in_ch, dim_kernel_x,
                                           och_stride, kernel_och_stride, grouped_index, ch_im_in);
                    }
                    beco_wt += (ch_im_out * dim_kernel_x) >> 3;
                }
#if defined(ARM_MATH_MVEI)
                beco_convolve_q15_readout32(beco_out);
                beco_convolve_q15_requant((int32_t *)beco_out, &multiplier[i_ch], &out_shift[i_ch],
                                          min, max, out_stride_parchannel,
                                          &cur_im_out[i_ch * out_stride_parchannel]);
#else
                beco_convolve_q15_readout(beco_out, 2, &multiplier[i_ch], &out_shift[i_ch], min, max);
                beco_convolve_norm_q15_copy_out(out_buffer,
                    &cur_im_out[i_ch*out_stride_parchannel], 4, out_stride_parchannel);
#endif
            }
            cur_im_out += dim_im_out_x;
        }
        Im_out += 4;
        ix += 4 * stride_x;
    }

    // remainder
    if (remainder) {
        iy = -padding_y;
        int16_t *cur_im_out = Im_out;
        // clear image_cache_valid
        memset(image_cache_valid, false, dim_kernel_dila_y * sizeof(bool));

        for (int i_out_y = 0; i_out_y < dim_im_out_y; i_out_y++, iy += stride_y) {
            // first do img2col here
            update_image_cache(iy, ix, Im_in, image_cache, image_cache_valid, ch_im_in,
                               dim_im_in_x, dim_im_in_y,
                               dim_kernel_x, dim_kernel_dila_y, padding_x, padding_y,
                               stride_x, stride_y, max_step_x, beco_im_out_x_num,
                               4, remainder, dilation_x, dilation_y);

            for (int i_ch = 0; i_ch < ch_im_out; i_ch += 8) {
                beco_vec64_in_t *beco_wt = (beco_vec64_in_t *)(wt + i_ch);
                if (bias) {
                    beco_convolve_q15_set_bias(&beco_bias[i_ch/2]);
                }
                else {
                    beco_clear_acc(BECO_ACC0);
                    beco_clear_acc(BECO_ACC1);
                    beco_clear_acc(BECO_ACC2);
                    beco_clear_acc(BECO_ACC3);
                }

                for (int ky = 0; ky < dim_kernel_dila_y; ky += dilation_y) {
                    int16_t *cur_input;
                    if (get_arranged_input(iy, ky, image_cache, image_cache_valid, cur_input)) {
                        beco_vec64_in_t *beco_in = (beco_vec64_in_t *)cur_input;
                        int32_t grouped_index = is_group*(i_ch/grouped_out_ch)*grouped_in_ch;
                        beco_convolve_impl(beco_in, beco_wt, grouped_in_ch, dim_kernel_x,
                                           och_stride, kernel_och_stride, grouped_index, ch_im_in);
                    }
                    beco_wt += (ch_im_out * dim_kernel_x) >> 3;
                }
#if defined(ARM_MATH_MVEI)
                beco_convolve_q15_readout32(beco_out);
                int16_t *out_buffer = (int16_t *)(beco_out + 32);
                beco_convolve_q15_requant((int32_t *)beco_out, &multiplier[i_ch], &out_shift[i_ch],
                                          min, max, 4, out_buffer);
                beco_convolve_norm_q15_copy_out(out_buffer, &cur_im_out[i_ch*out_stride_parchannel],
                                                remainder, out_stride_parchannel);
#else
                beco_convolve_q15_readout(beco_out, 2, &multiplier[i_ch], &out_shift[i_ch], min, max);
                beco_convolve_norm_q15_copy_out(out_buffer, &cur_im_out[i_ch*out_stride_parchannel],
                                                remainder, out_stride_parchannel);
#endif
            }
            cur_im_out += dim_im_out_x;
        }
    }
}

void beco_convolve_norm_q15(const int16_t *Im_in,
                            const uint16_t dim_im_in_x,
                            const uint16_t dim_im_in_y,
                            const uint16_t ch_im_in,
                            const int8_t *wt,
                            const uint16_t ch_im_out,
                            const uint16_t dim_kernel_x,
                            const uint16_t dim_kernel_y,
                            const uint16_t padding_x,
                            const uint16_t padding_y,
                            const uint16_t stride_x,
                            const uint16_t stride_y,
                            const int32_t *bias,
                            const int32_t *multiplier,
                            const int32_t *out_shift,
                            int16_t *Im_out,
                            const uint16_t dim_im_out_x,
                            const uint16_t dim_im_out_y,
                            const uint16_t dilation_x,
                            const uint16_t dilation_y,
                            const uint16_t groups,
                            const int16_t min,
                            const int16_t max,
                            int16_t *buffer)
{
    uint32_t config =
        BECO_CONF_AMODE_REP32 | BECO_CONF_BMODE_REP32 |
        BECO_CONF_ATYPE_INT16 | BECO_CONF_BTYPE_INT8  |
        BECO_CONF_RSHIFT(0) |
        BECO_CONF_PACK_INT32 | BECO_CONF_RD_16x8;
    beco_write_config(config);
    beco_convolve_norm_q15_process(Im_in, dim_im_in_x, dim_im_in_y, ch_im_in, wt, ch_im_out,
                                   dim_kernel_x, dim_kernel_y, padding_x, padding_y,
                                   stride_x, stride_y, bias, multiplier, out_shift, Im_out,
                                   dim_im_out_x, dim_im_out_y, dilation_x, dilation_y,
                                   groups, min, max, buffer);
}


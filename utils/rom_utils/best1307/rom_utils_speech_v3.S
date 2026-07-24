/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#include "asm_macros.h"

def_FuncExt  v3,  speech_dc_filter_create,                        0x00045970
def_FuncExt  v3,  speech_dc_filter_create_with_custom_allocator,  0x000457b4
def_FuncExt  v3,  speech_dc_filter_destroy,                       0x00045b30
def_FuncExt  v3,  speech_dc_filter_set_config,                    0x00045b48
def_FuncExt  v3,  speech_dc_filter_ctl,                           0x00045cbc
def_FuncExt  v3,  speech_dc_filter_process,                       0x00045e78
def_FuncExt  v3,  speech_dc_filter_process_int24,                 0x00045f78
def_FuncExt  v3,  speech_dc_filter_dump,                          0x00045d18
def_FuncExt  v3,  speech_dc_filter_get_required_mips,             0x00045d38
def_FuncExt  v3,  ec2float_create,                                0x000461b8
def_FuncExt  v3,  ec2float_create_with_custom_allocator,          0x00045f80
def_FuncExt  v3,  ec2float_destroy,                               0x000463f0
def_FuncExt  v3,  ec2float_set_config,                            0x00046458
def_FuncExt  v3,  ec2float_process,                               0x00046648
def_FuncExt  v3,  ec2float_set_external_vad,                      0x000467f4
def_FuncExt  v3,  ec2float_get_fir_filter_coeffs,                 0x00046808
def_FuncExt  v3,  ec2float_get_fir_filter_freq_coeffs,            0x00046810
def_FuncExt  v3,  ec2float_get_adaptive_filter_length,            0x00046818
def_FuncExt  v3,  ec2float_get_adaptive_filter_data,              0x00046824
def_FuncExt  v3,  ec2float_get_required_mips,                     0x00046838
def_FuncExt  v3,  ec2float_get_snr,                               0x000468b0
def_FuncExt  v3,  ec2float_get_noise_power,                       0x000468b8
def_FuncExt  v3,  eq_arm_init,                                    0x00046c1c
def_FuncExt  v3,  eq_arm_init_with_custom_allocator,              0x00046bb0
def_FuncExt  v3,  multi_eq_arm_init,                              0x00046c3c
def_FuncExt  v3,  multi_eq_arm_init_with_custom_allocator,        0x00046b50
def_FuncExt  v3,  eq_arm_destroy,                                 0x000468c0
def_FuncExt  v3,  eq_arm_set_config,                              0x00046900
def_FuncExt  v3,  eq_arm_process,                                 0x00046c98
def_FuncExt  v3,  eq_arm_process_int24,                           0x00046d74
def_FuncExt  v3,  eq_arm_process_float,                           0x00046e44
def_FuncExt  v3,  eq_arm_process2,                                0x00046ecc
def_FuncExt  v3,  eq_arm_process2_int24,                          0x00046fd0
def_FuncExt  v3,  eq_arm_process2_float,                          0x000470d0
def_FuncExt  v3,  eq_arm_get_required_mips,                       0x000471cc
def_FuncExt  v3,  speech_ns9_create,                              0x000449cc
def_FuncExt  v3,  speech_ns9_destory,                             0x000438d8
def_FuncExt  v3,  speech_ns9_process,                             0x00043b34
def_FuncExt  v3,  speech_ns9_set_config,                          0x00043f78
def_FuncExt  v3,  compexp_create,                                 0x000474fc
def_FuncExt  v3,  compexp_create_with_custom_allocator,           0x000474b4
def_FuncExt  v3,  compexp_destroy,                                0x00047220
def_FuncExt  v3,  compexp_set_config,                             0x00047238
def_FuncExt  v3,  compexp_process,                                0x0004780c
def_FuncExt  v3,  compexp_process_adj_eq,                         0x00047548
def_FuncExt  v3,  compexp_process_int24,                          0x00047b08
def_FuncExt  v3,  compexp_get_required_mips,                      0x00047dfc
#if defined(SPEECH_ROM_PATCH_10_P_0)
def_FuncExt  v3,  beco_convolve_1x1_q15_q7_q31_out_q15_pc                       0x0004c540
def_FuncExt  v3,  arm_depthwise_separable_conv_2x4s1_q15_q7_q31_out_q15_pc      0x0004d724
def_FuncExt  v3,  arm_depthwise_separable_conv_2x4_q15_q7_q31_out_q15_pc        0x0004d4c0
def_FuncExt  v3,  beco_fully_connected_q15_q7_q31                               0x0004cef8
def_FuncExt  v3,  arm_scale_q15_opt                                             0x0004d9d8
def_FuncExt  v3,  arm_mult_q15_opt                                              0x0004da08
def_FuncExt  v3,  arm_mult_q15_sub_opt                                          0x0004dad4
def_FuncExt  v3,  _ZN6besnet30naive_instent_layer_norm_floatEPfS0_S0_iS0_       0x0004bb04
def_FuncExt  v3,  arm_depthwise_transposed_conv_1x4_q15_q7_q31_out_q15          0x0004dbac
def_FuncExt  v3,  arm_depthwise_separable_conv_1x4s1_q15_q7_q31_out_q15         0x0004d30c
def_FuncExt  v3,  beco_convolve_1x1_q15_q7_q31_group                            0x0004ca9c
def_FuncExt  v3,  beco_convolve_1x1_q15_q7_q31_och1_out_q31                     0x0004d0dc
#endif
#if defined(SPEECH_ROM)
def_FuncExt  v3,  arm_copy_f32                                                  0x00048f54
def_FuncExt  v3,  arm_rfft_fast_init_f32                                        0x00048ea4
def_FuncExt  v3,  arm_rfft_fast_f32                                             0x00048dc4
def_FuncExt  v3,  arm_nn_activations_direct_q15                                 0x00047e1c
def_FuncExt  v3,  arm_cmplx_mag_f32                                             0x00049428
def_FuncExt  v3,  arm_scale_q15                                                 0x0004968c
def_FuncExt  v3,  arm_scale_f32                                                 0x00049600
def_FuncExt  v3,  arm_fill_f32                                                  0x00048fb8
def_FuncExt  v3,  arm_mat_trans_q15                                             0x00049000
def_FuncExt  v3,  arm_add_q15                                                   0x00049508
def_FuncExt  v3,  arm_offset_q15                                                0x00049594
def_FuncExt  v3,  arm_mean_f32                                                  0x00048ed4
#endif

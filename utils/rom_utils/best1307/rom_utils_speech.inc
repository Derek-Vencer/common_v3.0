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

def_RomFunc  speech_dc_filter_create_with_custom_allocator
def_RomFunc  speech_dc_filter_destroy
def_RomFunc  speech_dc_filter_set_config
def_RomFunc  speech_dc_filter_ctl
def_RomFunc  speech_dc_filter_process
def_RomFunc  speech_dc_filter_process_int24
def_RomFunc  speech_dc_filter_dump
def_RomFunc  speech_dc_filter_get_required_mips

def_RomFunc  ec2float_create_with_custom_allocator
def_RomFunc  ec2float_destroy
def_RomFunc  ec2float_set_config
def_RomFunc  ec2float_stereo
def_RomFunc  ec2float_process
def_RomFunc  ec2float_set_external_vad
def_RomFunc  ec2float_get_fir_filter_coeffs
def_RomFunc  ec2float_get_fir_filter_freq_coeffs
def_RomFunc  ec2float_get_adaptive_filter_length
def_RomFunc  ec2float_get_adaptive_filter_data
def_RomFunc  ec2float_get_required_mips
def_RomFunc  ec2float_get_snr
def_RomFunc  ec2float_get_noise_power

#if defined(SPEECH_ROM)
def_RomFunc  eq_arm_init
def_RomFunc  eq_arm_init_with_custom_allocator
def_RomFunc  multi_eq_arm_init
def_RomFunc  multi_eq_arm_init_with_custom_allocator
def_RomFunc  eq_arm_destroy
def_RomFunc  eq_arm_set_config
def_RomFunc  eq_arm_process
def_RomFunc  eq_arm_process_int24
def_RomFunc  eq_arm_process_float
def_RomFunc  eq_arm_process2
def_RomFunc  eq_arm_process2_int24
def_RomFunc  eq_arm_process2_float
def_RomFunc  eq_arm_get_required_mips
#endif

#if !defined(SPEECH_ROM_PATCH_10_P_0)
def_RomFunc  speech_ns9_destory
#endif
def_RomFunc  speech_ns9_set_config
#if defined(SPEECH_ROM_PATCH_10_P_0)
def_RomFunc  beco_convolve_1x1_q15_q7_q31_out_q15_pc
def_RomFunc  arm_depthwise_separable_conv_2x4s1_q15_q7_q31_out_q15_pc
def_RomFunc  arm_depthwise_separable_conv_2x4_q15_q7_q31_out_q15_pc
def_RomFunc  beco_fully_connected_q15_q7_q31
def_RomFunc  arm_scale_q15_opt
def_RomFunc  arm_mult_q15_opt
def_RomFunc  arm_mult_q15_sub_opt
def_RomFunc  _ZN6besnet30naive_instent_layer_norm_floatEPfS0_S0_iS0_
def_RomFunc  arm_depthwise_transposed_conv_1x4_q15_q7_q31_out_q15
def_RomFunc  arm_depthwise_separable_conv_1x4s1_q15_q7_q31_out_q15
def_RomFunc  beco_convolve_1x1_q15_q7_q31_group
def_RomFunc  beco_convolve_1x1_q15_q7_q31_och1_out_q31
#else
def_RomFunc  speech_ns9_create
#if !defined(SPEECH_ROM_PATCH_9_P_1) && !defined(SPEECH_ROM_PATCH_9_P_5)
def_RomFunc  speech_ns9_process
#endif
#endif

#if defined(SPEECH_ROM)
def_RomFunc  arm_copy_f32
def_RomFunc  arm_rfft_fast_init_f32
def_RomFunc  arm_rfft_fast_f32
def_RomFunc  arm_nn_activations_direct_q15
def_RomFunc  arm_cmplx_mag_f32
def_RomFunc  arm_scale_q15
def_RomFunc  arm_scale_f32
def_RomFunc  arm_fill_f32
def_RomFunc  arm_mat_trans_q15
def_RomFunc  arm_add_q15
def_RomFunc  arm_offset_q15
def_RomFunc  arm_mean_f32
#endif

def_RomFunc  compexp_create_with_custom_allocator
def_RomFunc  compexp_destroy
def_RomFunc  compexp_set_config
def_RomFunc  compexp_process
def_RomFunc  compexp_process_adj_eq
def_RomFunc  compexp_process_int24
def_RomFunc  compexp_get_required_mips

def_RomFunc  multi_compexp_create_with_custom_allocator
def_RomFunc  multi_compexp_destroy
#def_RomFunc  multi_compexp_set_config
def_RomFunc  multi_compexp_process
def_RomFunc  multi_compexp_process_int24
def_RomFunc  multi_compexp_get_required_mips

#if defined(SPEECH_TX_1MIC_NS_IN_ROM)
def_RomFunc  speech_1mic_ns_create
def_RomFunc  speech_1mic_ns_destroy
def_RomFunc  speech_1mic_ns_process
def_RomFunc  speech_1mic_ns_get_model_data_94l
def_RomFunc  speech_1mic_ns_get_model_data_95l
def_RomFunc  es_core_init
def_RomFunc  es_core_destroy
def_RomFunc  es_process_spectrum
#endif

#if defined(SPEECH_NS2FLOAT_IN_ROM)
def_RomFunc  speech_ns2float_create
def_RomFunc  speech_ns2float_create_with_custom_allocator
def_RomFunc  speech_ns2float_destroy
def_RomFunc  speech_ns2float_set_config
def_RomFunc  speech_ns2float_process
def_RomFunc  speech_ns2float_process_int24
def_RomFunc  speech_ns2float_set_echo_state
def_RomFunc  speech_ns2float_set_echo_suppress
def_RomFunc  speech_ns2float_set_ringtone_mode
def_RomFunc  speech_ns2float_set_external_vad
def_RomFunc  speech_ns2float_get_required_mips
#endif

#if defined(SPEECH_2MIC_PREAF_IN_ROM)
def_RomFunc  speech_2mic_preaf_create
def_RomFunc  speech_2mic_preaf_destroy
def_RomFunc  speech_2mic_preaf_process
#endif

#if defined(SPEECH_2MIC_NS8_IN_ROM)
def_RomFunc  speech_2mic_ns8_create
def_RomFunc  speech_2mic_ns8_destroy
def_RomFunc  speech_2mic_ns8_process
def_RomFunc  speech_2mic_ns8_get_required_mips
def_RomFunc  speech_2mic_ns8_get_delay
def_RomFunc  speech_2mic_ns8_set_config
def_RomFunc  speech_2mic_ns8_get_model_data_91s
def_RomFunc  speech_2mic_ns8_get_model_data_95s
#endif

def_RomFunc  cmsis_f32_fft
def_RomFunc  cmsis_f32_fft_init_with_custom_allocator
def_RomFunc  cmsis_f32_fft_destroy
def_RomFunc  cmsis_f32_ifft
def_RomFunc  get_conj_win_f32
def_RomFunc  filterbank_create_pflt
def_RomFunc  filterbank_destroy_pflt
def_RomFunc  filterbank_compute_bank_pflt
def_RomFunc  filterbank_compute_psd_pflt
def_RomFunc  spectral_wdrc_create
def_RomFunc  spectral_wdrc_process
def_RomFunc  spectral_wdrc_destroy
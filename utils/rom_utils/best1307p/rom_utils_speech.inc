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

def_RomFunc  Reset_WB_PLC
def_RomFunc  WB_PLC
def_RomFunc  WB_PLC_erasure

def_RomFunc  speech_1mic_ns_create
def_RomFunc  speech_1mic_ns_destroy
def_RomFunc  speech_1mic_ns_process
def_RomFunc  speech_1mic_ns_set_af_state
def_RomFunc  speech_1mic_ns_get_model_data_94l
def_RomFunc  speech_1mic_ns_get_model_data_95l
def_RomFunc  speech_1mic_ns_get_model_data_96l

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

def_RomFunc  speech_1mic_preaf_set_config
def_RomFunc  speech_1mic_preaf_create
def_RomFunc  speech_1mic_preaf_destroy
def_RomFunc  speech_1mic_preaf_process
def_RomFunc  speech_1mic_preaf_reset_coef

def_RomFunc  speech_2mic_preaf_create
def_RomFunc  speech_2mic_preaf_destroy
def_RomFunc  speech_2mic_preaf_process
def_RomFunc  speech_2mic_preaf_reset_coef

def_RomFunc  speech_2mic_ns8_get_model_data_91s
def_RomFunc  speech_2mic_ns8_get_model_data_95s

def_RomFunc  speech_3mic_ns_get_model_data_91s
def_RomFunc  speech_3mic_ns_get_model_data_95s

def_RomFunc  arm_copy_f32
def_RomFunc  arm_fill_f32
def_RomFunc  arm_fill_q31
def_RomFunc  arm_fill_q15
def_RomFunc  arm_scale_q15
def_RomFunc  arm_scale_f32
def_RomFunc  arm_add_q15
def_RomFunc  arm_mean_f32
def_RomFunc  arm_mat_trans_q15
def_RomFunc  arm_nn_activations_direct_q15
def_RomFunc  arm_offset_q15
def_RomFunc  arm_cmplx_mag_f32
def_RomFunc  logmmse_init
def_RomFunc  logmmse_destroy
def_RomFunc  logmmse_analysis
def_RomFunc  logmmse_process
def_RomFunc  lcmmse_init
def_RomFunc  lcmmse_destroy
def_RomFunc  lcmmse_noise_estimate
def_RomFunc  lcmmse_get_noise_power
def_RomFunc  lcmmse_get_snrpost
def_RomFunc  lcmmse_get_speech_prob
def_RomFunc  mdf_time_destroy
def_RomFunc  mdf_time_reset_filter
def_RomFunc  fixed_bf_set_config
def_RomFunc  fixed_bf_create
def_RomFunc  fixed_bf_destroy
def_RomFunc  fixed_bf_process
def_RomFunc  arm_rfft_fast_init_f32
def_RomFunc  arm_rfft_fast_f32
#if !defined(USE_CMSIS_FFT_LEN_1024)
def_RomFunc  cmsis_f32_fft_init_with_custom_allocator
def_RomFunc  cmsis_f32_fft_destroy
def_RomFunc  cmsis_f32_fft
def_RomFunc  cmsis_f32_ifft
#endif
def_RomFunc  stft
def_RomFunc  istft_f32
def_RomFunc  update_auto_psd
def_RomFunc  update_cross_psd
def_RomFunc  calc_coherence
def_RomFunc  spectral_wdrc_create
def_RomFunc  spectral_wdrc_destroy
def_RomFunc  spectral_wdrc_process
def_RomFunc  integer_delay_init
def_RomFunc  integer_delay_destroy
def_RomFunc  integer_delay_process
def_RomFunc  integer_delay_init_with_custom_allocator
def_RomFunc  filterbank_create_pflt
def_RomFunc  filterbank_destroy_pflt
def_RomFunc  filterbank_compute_bank_pflt
def_RomFunc  filterbank_compute_psd_pflt
def_RomFunc  get_conj_win_half
def_RomFunc  get_conj_win_f32
def_RomFunc  GetFloatLimiterSize
def_RomFunc  CreateFloatLimiter
def_RomFunc  DestroyFloatLimiter
def_RomFunc  ApplyFloatLimiter
def_RomFunc  speech_nn_ns9s_create
def_RomFunc  speech_nn_ns9s_run
def_RomFunc  speech_nn_ns9s_destroy
def_RomFunc  speech_nn_ns9s_lcmmse_get_snr_estimate
def_RomFunc  speech_nn_ns9s_lcmmse_get_noise_power
def_RomFunc  speech_nn_ns9s_needed_size
def_RomFunc  speech_nn_ns9l_create
def_RomFunc  speech_nn_ns9l_destroy
def_RomFunc  speech_nn_ns9l_run
def_RomFunc  speech_nn_ns9l_needed_size
def_RomFunc  rls_create_in_3mic
def_RomFunc  rls_destroy_in_3mic
def_RomFunc  rls_process_in_3mic
def_RomFunc  rls_process_in_3mic_new
def_RomFunc  rls_create
def_RomFunc  rls_destroy
def_RomFunc  rls_update
def_RomFunc  rls_process
def_RomFunc  arm_depthwise_separable_conv_2x4s2pc_ns_s16
def_RomFunc  arm_depthwise_separable_conv_2x4s1pc_ns_s16
def_RomFunc  arm_instance_norm_f32
def_RomFunc  arm_depthwise_transposed_conv_1x4s2_ns_s16
def_RomFunc  arm_depthwise_separable_conv_1x4s1_ns_s16
def_RomFunc  _Z36beco_fully_connected_ns_s16_set_biasPK15beco_vec64_in_t
def_RomFunc  beco_convolve_1x1pc_norelu_ns_s16
def_RomFunc  beco_convolve_1x1pc_ns_s16
def_RomFunc  beco_convolve_1x1_ns_s16

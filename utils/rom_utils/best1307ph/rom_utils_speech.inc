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

def_RomFunc  speech_dc_filter_create
def_RomFunc  speech_dc_filter_create_with_custom_allocator
def_RomFunc  speech_dc_filter_destroy
def_RomFunc  speech_dc_filter_set_config
def_RomFunc  speech_dc_filter_ctl
def_RomFunc  speech_dc_filter_process
def_RomFunc  speech_dc_filter_process_int24
def_RomFunc  speech_dc_filter_dump
def_RomFunc  speech_dc_filter_get_required_mips
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
def_RomFunc  lpc_plc_create
def_RomFunc  lpc_plc_destroy
def_RomFunc  lpc_plc_save
def_RomFunc  lpc_plc_generate
def_RomFunc  speech_1mic_preaf_create
def_RomFunc  speech_1mic_preaf_destroy
def_RomFunc  speech_1mic_preaf_process
def_RomFunc  speech_1mic_preaf_reset_coef
def_RomFunc  speech_1mic_preaf_set_config
def_RomFunc  speech_2mic_preaf_create
def_RomFunc  speech_2mic_preaf_destroy
def_RomFunc  speech_2mic_preaf_process
def_RomFunc  speech_2mic_preaf_reset_coef
def_RomFunc  speech_2mic_preaf_set_config
def_RomFunc  speech_3mic_preaf_create
def_RomFunc  speech_3mic_preaf_destroy
def_RomFunc  speech_3mic_preaf_process
def_RomFunc  speech_3mic_preaf_set_config
def_RomFunc  speech_3mic_preaf_reset_coef
def_RomFunc  speech_3mic_preaf_set_wind_status
def_RomFunc  speech_3mic_preaf_reset_config_with_mode
def_RomFunc  get_conj_win_half
def_RomFunc  arm_scale_f32
def_RomFunc  arm_copy_f32
def_RomFunc  arm_rfft_fast_init_f32
def_RomFunc  arm_rfft_fast_f32
def_RomFunc  event_detection_create
def_RomFunc  event_detection_process
def_RomFunc  event_detection_destroy
def_RomFunc  event_detection_reset_controller
def_RomFunc  event_detection_set_fir_flag
def_RomFunc  event_detection_reset_fir_flag
def_RomFunc  event_detection_set_anc_on_flag
def_RomFunc  event_detection_set_ff_fir_enable
def_RomFunc  event_detection_reset
def_RomFunc  event_detection_set_noise_status
def_RomFunc  anc_ff_iir_lms_register_set_fir_freq_handler
def_RomFunc  anc_ff_iir_lms_register_set_iir_handler
def_RomFunc  anc_ff_iir_lms_register_convergence_handler
def_RomFunc  anc_ff_iir_lms_create
def_RomFunc  anc_ff_iir_lms_destroy
def_RomFunc  anc_ff_iir_lms_reset
def_RomFunc  anc_ff_iir_lms_process
def_RomFunc  anc_ff_iir_lms_set_ff_iir_status
def_RomFunc  anc_ff_iir_lms_sync_fir_flag
def_RomFunc  anc_ff_iir_lms_reset_ff_FIR
def_RomFunc  anc_ff_iir_lms_set_ff_iir_calib_gain
def_RomFunc  anc_ff_iir_lms_set_adc_gain
def_RomFunc  anc_ff_iir_lms_update_FB_gain
def_RomFunc  anc_ff_iir_lms_clear_ada_iir_cache
def_RomFunc  speech_1mic_ns_create
def_RomFunc  speech_1mic_ns_destroy
def_RomFunc  speech_1mic_ns_process
def_RomFunc  speech_1mic_ns_set_af_state
def_RomFunc  speech_1mic_ns_set_config
def_RomFunc  speech_2mic_ns8_create
def_RomFunc  speech_2mic_ns8_destroy
def_RomFunc  speech_2mic_ns8_process
def_RomFunc  speech_2mic_ns8_get_required_mips
def_RomFunc  speech_2mic_ns8_get_delay
def_RomFunc  speech_2mic_ns8_set_config
def_RomFunc  speech_2mic_ns8_set_af_state
def_RomFunc  speech_3mic_ns_create
def_RomFunc  speech_3mic_ns_preprocess
def_RomFunc  speech_3mic_ns_process
def_RomFunc  speech_3mic_ns_destroy
def_RomFunc  speech_3mic_ns_set_config
def_RomFunc  speech_3mic_ns_reset_config_with_mode
def_RomFunc  speech_3mic_get_required_mips
def_RomFunc  speech_3mic_ns_set_ns_state
def_RomFunc  speech_3mic_ns_update_snr
def_RomFunc  speech_3mic_ns_get_wind_state
def_RomFunc  speech_3mic_ns_get_wind_status
def_RomFunc  speech_3mic_ns_set_af_state
def_RomFunc  speech_nn_ns_get_model_data_94l
def_RomFunc  speech_nn_ns_get_model_data_95l_rom_v2
def_RomFunc  speech_nn_ns_get_model_data_95s
def_RomFunc  speech_nn_ns_get_model_data_91s
def_RomFunc  arm_fill_q15
def_RomFunc  arm_fill_q31
def_RomFunc  arm_fill_f32

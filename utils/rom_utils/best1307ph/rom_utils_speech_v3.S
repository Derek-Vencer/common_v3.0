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

def_FuncExt  v1,  speech_dc_filter_create,                        0x00188ccc
def_FuncExt  v1,  speech_dc_filter_create_with_custom_allocator,  0x00188bd8
def_FuncExt  v1,  speech_dc_filter_destroy,                       0x00188dc4
def_FuncExt  v1,  speech_dc_filter_set_config,                    0x00188ddc
def_FuncExt  v1,  speech_dc_filter_ctl,                           0x00188e9c
def_FuncExt  v1,  speech_dc_filter_process,                       0x00189038
def_FuncExt  v1,  speech_dc_filter_process_int24,                 0x0018912c
def_FuncExt  v1,  speech_dc_filter_dump,                          0x00188ef8
def_FuncExt  v1,  speech_dc_filter_get_required_mips,             0x00188f18
def_FuncExt  v1,  eq_arm_init,                                    0x0018943c
def_FuncExt  v1,  eq_arm_init_with_custom_allocator,              0x001893d0
def_FuncExt  v1,  multi_eq_arm_init,                              0x0018945c
def_FuncExt  v1,  multi_eq_arm_init_with_custom_allocator,        0x0018936c
def_FuncExt  v1,  eq_arm_destroy,                                 0x00189134
def_FuncExt  v1,  eq_arm_set_config,                              0x00189174
def_FuncExt  v1,  eq_arm_process,                                 0x001894c0
def_FuncExt  v1,  eq_arm_process_int24,                           0x0018959c
def_FuncExt  v1,  eq_arm_process_float,                           0x0018966c
def_FuncExt  v1,  eq_arm_process2,                                0x001896fc
def_FuncExt  v1,  eq_arm_process2_int24,                          0x00189804
def_FuncExt  v1,  eq_arm_process2_float,                          0x00189904
def_FuncExt  v1,  eq_arm_get_required_mips,                       0x00189a00
def_FuncExt  v1,  speech_ns2float_create,                         0x00189f38
def_FuncExt  v1,  speech_ns2float_create_with_custom_allocator,   0x00189d90
def_FuncExt  v1,  speech_ns2float_destroy,                        0x00189a54
def_FuncExt  v1,  speech_ns2float_set_config,                     0x00189ae8
def_FuncExt  v1,  speech_ns2float_process,                        0x0018aae8
def_FuncExt  v1,  speech_ns2float_process_int24,                  0x0018b590
def_FuncExt  v1,  speech_ns2float_set_echo_state,                 0x00189f5c
def_FuncExt  v1,  speech_ns2float_set_echo_suppress,              0x00189f74
def_FuncExt  v1,  speech_ns2float_set_ringtone_mode,              0x0018a010
def_FuncExt  v1,  speech_ns2float_set_external_vad,               0x0018a01c
def_FuncExt  v1,  speech_ns2float_get_required_mips,              0x0018a030
def_FuncExt  v1,  lpc_plc_create,                                 0x00188004
def_FuncExt  v1,  lpc_plc_destroy,                                0x0018807c
def_FuncExt  v1,  lpc_plc_save,                                   0x001880a8
def_FuncExt  v1,  lpc_plc_generate,                               0x001880e8
def_FuncExt  v1,  speech_1mic_preaf_create,                       0x00188138
def_FuncExt  v1,  speech_1mic_preaf_destroy,                      0x001881cc
def_FuncExt  v1,  speech_1mic_preaf_process,                      0x001881f8
def_FuncExt  v1,  speech_1mic_preaf_reset_coef,                   0x0018828c
def_FuncExt  v1,  speech_1mic_preaf_set_config,                   0x0018812c
def_FuncExt  v1,  speech_2mic_preaf_create,                       0x00188348
def_FuncExt  v1,  speech_2mic_preaf_destroy,                      0x001883c4
def_FuncExt  v1,  speech_2mic_preaf_process,                      0x00188418
def_FuncExt  v1,  speech_2mic_preaf_reset_coef,                   0x00188520
def_FuncExt  v1,  speech_2mic_preaf_set_config,                   0x001882a0
def_FuncExt  v1,  speech_3mic_preaf_create,                       0x001885b0
def_FuncExt  v1,  speech_3mic_preaf_destroy,                      0x00188788
def_FuncExt  v1,  speech_3mic_preaf_process,                      0x0018880c
def_FuncExt  v1,  speech_3mic_preaf_set_config,                   0x00188534
def_FuncExt  v1,  speech_3mic_preaf_reset_coef,                   0x00188bb8
def_FuncExt  v1,  speech_3mic_preaf_set_wind_status,              0x00188bcc
def_FuncExt  v1,  speech_3mic_preaf_reset_config_with_mode,       0x00188548
def_FuncExt  v1,  get_conj_win_half,                              0x0019ad14
def_FuncExt  v1,  arm_scale_f32,                                  0x00199fb4
def_FuncExt  v1,  arm_copy_f32,                                   0x001996ac
def_FuncExt  v1,  arm_rfft_fast_init_f32,                         0x001995f4
def_FuncExt  v1,  arm_rfft_fast_f32,                              0x0019951c
def_FuncExt  v1,  event_detection_create,                         0x0018d40c
def_FuncExt  v1,  event_detection_process,                        0x0018e4ec
def_FuncExt  v1,  event_detection_destroy,                        0x0018d7e0
def_FuncExt  v1,  event_detection_reset_controller,               0x0018e848
def_FuncExt  v1,  event_detection_set_fir_flag,                   0x0018e85c
def_FuncExt  v1,  event_detection_reset_fir_flag,                 0x0018e898
def_FuncExt  v1,  event_detection_set_anc_on_flag,                0x0018e8c0
def_FuncExt  v1,  event_detection_set_ff_fir_enable,              0x0018e8f4
def_FuncExt  v1,  event_detection_reset,                          0x0018d37c
def_FuncExt  v1,  event_detection_set_noise_status,               0x0018e8f8
def_FuncExt  v1,  anc_ff_iir_lms_register_set_fir_freq_handler,   0x0018ba2c
def_FuncExt  v1,  anc_ff_iir_lms_register_set_iir_handler,        0x0018ba24
def_FuncExt  v1,  anc_ff_iir_lms_register_convergence_handler,    0x0018ba34
def_FuncExt  v1,  anc_ff_iir_lms_create,                          0x0018bbb0
def_FuncExt  v1,  anc_ff_iir_lms_destroy,                         0x0018c7f8
def_FuncExt  v1,  anc_ff_iir_lms_process,                         0x0018c9dc
def_FuncExt  v1,  anc_ff_iir_lms_set_ff_iir_status,               0x0018d254
def_FuncExt  v1,  anc_ff_iir_lms_sync_fir_flag,                   0x0018d280
def_FuncExt  v1,  anc_ff_iir_lms_reset_ff_FIR,                    0x0018d24c
def_FuncExt  v1,  anc_ff_iir_lms_set_ff_iir_calib_gain,           0x0018d288
def_FuncExt  v1,  anc_ff_iir_lms_set_adc_gain,                    0x0018d2f4
def_FuncExt  v1,  anc_ff_iir_lms_update_FB_gain,                  0x0018ba44
def_FuncExt  v1,  anc_ff_iir_lms_clear_ada_iir_cache,             0x0018d2e4
def_FuncExt  v1,  speech_1mic_ns_create,                          0x0005b20c
def_FuncExt  v1,  speech_1mic_ns_destroy,                         0x0005b5d8
def_FuncExt  v1,  speech_1mic_ns_process,                         0x0005b658
def_FuncExt  v1,  speech_1mic_ns_set_af_state,                    0x0005bb38
def_FuncExt  v1,  speech_1mic_ns_set_config,                      0x0005bb40
def_FuncExt  v1,  speech_2mic_ns8_create,                         0x00059dd8
def_FuncExt  v1,  speech_2mic_ns8_destroy,                        0x0005a214
def_FuncExt  v1,  speech_2mic_ns8_process,                        0x0005b168
def_FuncExt  v1,  speech_2mic_ns8_get_required_mips,              0x0005b1f0
def_FuncExt  v1,  speech_2mic_ns8_get_delay,                      0x0005b1fc
def_FuncExt  v1,  speech_2mic_ns8_set_config,                     0x000599d0
def_FuncExt  v1,  speech_2mic_ns8_set_af_state,                   0x0005b204
def_FuncExt  v1,  speech_3mic_ns_create,                          0x00057660
def_FuncExt  v1,  speech_3mic_ns_process,                         0x00057f98
def_FuncExt  v1,  speech_3mic_ns_destroy,                         0x00057d50
def_FuncExt  v1,  speech_3mic_ns_set_config,                      0x00057004
def_FuncExt  v1,  speech_3mic_ns_reset_config_with_mode,          0x00057500
def_FuncExt  v1,  speech_3mic_ns_get_wind_state,                  0x000599b4
def_FuncExt  v1,  speech_3mic_ns_get_wind_status,                 0x000599bc
def_FuncExt  v1,  speech_3mic_ns_set_af_state,                    0x000599c8
def_FuncExt  v1,  speech_nn_ns_get_model_data_94l,                0x0005d8d0
def_FuncExt  v1,  speech_nn_ns_get_model_data_95l_rom_v2,         0x0005d8c8
def_FuncExt  v1,  speech_nn_ns_get_model_data_95s,                0x0005d8c0
def_FuncExt  v1,  speech_nn_ns_get_model_data_91s,                0x0005d8b8
def_FuncExt  v1,  arm_fill_q15,                                   0x00063d4c
def_FuncExt  v1,  arm_fill_q31,                                   0x00063dac
def_FuncExt  v1,  arm_fill_f32,                                   0x00063d08

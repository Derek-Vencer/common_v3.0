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
#include "asm_macros.h"

def_FuncExt  v1,  speech_dc_filter_create,                        0x00044f18
def_FuncExt  v1,  speech_dc_filter_create_with_custom_allocator,  0x00044d5c
def_FuncExt  v1,  speech_dc_filter_destroy,                       0x000450d8
def_FuncExt  v1,  speech_dc_filter_set_config,                    0x000450f0
def_FuncExt  v1,  speech_dc_filter_ctl,                           0x00045264
def_FuncExt  v1,  speech_dc_filter_process,                       0x00045420
def_FuncExt  v1,  speech_dc_filter_process_int24,                 0x00045520
def_FuncExt  v1,  speech_dc_filter_dump,                          0x000452c0
def_FuncExt  v1,  speech_dc_filter_get_required_mips,             0x000452e0
def_FuncExt  v1,  eq_arm_init,                                    0x00045884
def_FuncExt  v1,  eq_arm_init_with_custom_allocator,              0x00045818
def_FuncExt  v1,  multi_eq_arm_init,                              0x000458a4
def_FuncExt  v1,  multi_eq_arm_init_with_custom_allocator,        0x000457b8
def_FuncExt  v1,  eq_arm_destroy,                                 0x00045528
def_FuncExt  v1,  eq_arm_set_config,                              0x00045568
def_FuncExt  v1,  eq_arm_process,                                 0x00045900
def_FuncExt  v1,  eq_arm_process_int24,                           0x000459dc
def_FuncExt  v1,  eq_arm_process_float,                           0x00045aac
def_FuncExt  v1,  eq_arm_process2,                                0x00045b34
def_FuncExt  v1,  eq_arm_process2_int24,                          0x00045c38
def_FuncExt  v1,  eq_arm_process2_float,                          0x00045d38
def_FuncExt  v1,  eq_arm_get_required_mips,                       0x00045e34
def_FuncExt  v1,  compexp_create,                                 0x00046164
def_FuncExt  v1,  compexp_create_with_custom_allocator,           0x0004611c
def_FuncExt  v1,  compexp_destroy,                                0x00045e88
def_FuncExt  v1,  compexp_set_config,                             0x00045ea0
def_FuncExt  v1,  compexp_process,                                0x00046474
def_FuncExt  v1,  compexp_process_adj_eq,                         0x000461b0
def_FuncExt  v1,  compexp_process_int24,                          0x00046770
def_FuncExt  v1,  compexp_get_required_mips,                      0x00046a64
def_FuncExt  v1,  multi_compexp_create,                           0x00046cbc
def_FuncExt  v1,  multi_compexp_create_with_custom_allocator,     0x00046c60
def_FuncExt  v1,  multi_compexp_destroy,                          0x00046a7c
def_FuncExt  v1,  multi_compexp_set_config,                       0x00046ab8
def_FuncExt  v1,  multi_compexp_process,                          0x00046d1c
def_FuncExt  v1,  multi_compexp_process_int24,                    0x00046d88
def_FuncExt  v1,  multi_compexp_get_required_mips,                0x00046df4
def_FuncExt  v1,  speech_1mic_ns_create,                          0x00044354
def_FuncExt  v1,  speech_1mic_ns_destroy,                         0x00044770
def_FuncExt  v1,  speech_1mic_ns_process,                         0x00044924
def_FuncExt  v1,  speech_1mic_ns_get_model_data_94l,              0x00048908
def_FuncExt  v1,  speech_2mic_preaf_create,                       0x00043010
def_FuncExt  v1,  speech_2mic_preaf_destroy,                      0x000430e8
def_FuncExt  v1,  speech_2mic_preaf_process,                      0x0004313c
def_FuncExt  v1,  speech_2mic_ns8_create,                         0x000435e8
def_FuncExt  v1,  speech_2mic_ns8_destroy,                        0x0004390c
def_FuncExt  v1,  speech_2mic_ns8_process,                        0x00043a24
def_FuncExt  v1,  speech_2mic_ns8_get_required_mips,              0x00044340
def_FuncExt  v1,  speech_2mic_ns8_get_delay,                      0x0004434c
def_FuncExt  v1,  speech_2mic_ns8_set_config,                     0x0004323c
def_FuncExt  v1,  speech_2mic_ns8_get_model_data_91s,             0x000488f8
def_FuncExt  v1,  speech_2mic_ns8_get_model_data_95s,             0x00048900
def_FuncExt  v1,  speech_ns2float_create,                         0x000472a0
def_FuncExt  v1,  speech_ns2float_create_with_custom_allocator,   0x00047124
def_FuncExt  v1,  speech_ns2float_destroy,                        0x00046e00
def_FuncExt  v1,  speech_ns2float_set_config,                     0x00046e94
def_FuncExt  v1,  speech_ns2float_process,                        0x00047e4c
def_FuncExt  v1,  speech_ns2float_process_int24,                  0x000488f0
def_FuncExt  v1,  speech_ns2float_set_echo_state,                 0x000472c4
def_FuncExt  v1,  speech_ns2float_set_echo_suppress,              0x000472dc
def_FuncExt  v1,  speech_ns2float_set_ringtone_mode,              0x00047378
def_FuncExt  v1,  speech_ns2float_set_external_vad,               0x00047384
def_FuncExt  v1,  speech_ns2float_get_required_mips,              0x00047398

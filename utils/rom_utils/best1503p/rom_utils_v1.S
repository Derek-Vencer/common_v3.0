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
    def_FuncExt v1, rom1_entry,                         0x00030000
    def_FuncExt v1, set__impure_ptr,                    0x00030000
    def_FuncExt v1, _btif_sbc_decode_frames_select_channel, 0x000323a8
    def_FuncExt v1, _btif_sbc_decode_frames_parser,      0x00032500
    def_FuncExt v1, _btif_sbc_init_encoder,              0x00032508
    def_FuncExt v1, _btif_sbc_encode_frames,             0x00032510
    def_FuncExt v1, _btif_plc_update_sbc_decoder_state,  0x0003256c
    def_Symbol sbc_crc_tbl,                             0x220273f0

    def_FuncExt v1, aacDecoder_Close_real,              0x000334ec
    def_FuncExt v1, is_aacDecoder_Close_real            0x00033520
    def_FuncExt v1, aacDecoder_GetStreamInfo_real       0x00033530
    def_FuncExt v1, aacDecoder_DecodeFrame_real         0x00033348
    def_FuncExt v1, aacDecoder_Fill_real                0x00033318
    def_FuncExt v1, aacDecoder_DecodeFrame_Config_real  0x00033534
    def_FuncExt v1, aacDecoder_SetParam_real            0x00033250
    def_FuncExt v1, aacDecoder_Open_real                0x00033298
    def_FuncExt v1, aacCreateMuteFrame_real             0x00033538

    def_FuncExt v1, CvsdEncInit                         0x000335b0
    def_FuncExt v1, CvsdDecInit                         0x000335d0
    def_FuncExt v1, CvsdEncode                          0x000335f0
    def_FuncExt v1, CvsdDecode                          0x00033b10

    def_FuncExt v1, lc3_api_decoder_init_sneak_rom      0x00035270
    def_FuncExt v1, lc3_api_encoder_init_sneak_rom      0x00035578

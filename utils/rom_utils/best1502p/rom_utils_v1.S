/***************************************************************************
 *
 * Copyright 2015-2024 BES.
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
    def_FuncExt v1, set__impure_ptr,                    0x00030b14
    def_FuncExt v1, _btif_sbc_decode_frames_select_channel, 0x000324e8
    def_FuncExt v1, _btif_sbc_decode_frames_parser,      0x00032640
    def_FuncExt v1, _btif_sbc_init_encoder,              0x00032648
    def_FuncExt v1, _btif_sbc_encode_frames,             0x00032650
    def_FuncExt v1, _btif_plc_update_sbc_decoder_state,  0x000326ac
    def_Symbol sbc_crc_tbl,                             0x22027200

    def_FuncExt v1, aacDecoder_Close_real,              0x0003362c
    def_FuncExt v1, is_aacDecoder_Close_real            0x00033660
    def_FuncExt v1, aacDecoder_GetStreamInfo_real       0x00033670
    def_FuncExt v1, aacDecoder_DecodeFrame_real         0x00033488
    def_FuncExt v1, aacDecoder_Fill_real                0x00033458
    def_FuncExt v1, aacDecoder_DecodeFrame_Config_real  0x00033674
    def_FuncExt v1, aacDecoder_SetParam_real            0x00033390
    def_FuncExt v1, aacDecoder_Open_real                0x000333d8
    def_FuncExt v1, aacCreateMuteFrame_real             0x00033678

    def_FuncExt v1, CvsdEncInit                         0x000336f0
    def_FuncExt v1, CvsdDecInit                         0x00033710
    def_FuncExt v1, CvsdEncode                          0x00033730
    def_FuncExt v1, CvsdDecode                          0x00033c50

    def_FuncExt v1, lc3_api_decoder_init_sneak_rom      0x000353a8
    def_FuncExt v1, lc3_api_encoder_init_sneak_rom      0x0003555c

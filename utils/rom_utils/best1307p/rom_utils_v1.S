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
    def_FuncExt v1, _btif_sbc_decode_frames_select_channel, 0x0003243c
    def_FuncExt v1, _btif_sbc_decode_frames_parser,      0x00032594
    def_FuncExt v1, _btif_sbc_init_encoder,              0x0003259c
    def_FuncExt v1, _btif_sbc_encode_frames,             0x000325a4
    def_FuncExt v1, _btif_plc_update_sbc_decoder_state,  0x00032600
    def_Symbol sbc_crc_tbl,                             0x22027480

    def_FuncExt v1, aacDecoder_Close_real,              0x0003357c
    def_FuncExt v1, is_aacDecoder_Close_real            0x000335b0
    def_FuncExt v1, aacDecoder_GetStreamInfo_real       0x000335c0
    def_FuncExt v1, aacDecoder_DecodeFrame_real         0x000333d8
    def_FuncExt v1, aacDecoder_Fill_real                0x000333a8
    def_FuncExt v1, aacDecoder_DecodeFrame_Config_real  0x000335c4
    def_FuncExt v1, aacDecoder_SetParam_real            0x000332e0
    def_FuncExt v1, aacDecoder_Open_real                0x00033328
    def_FuncExt v1, aacCreateMuteFrame_real             0x000335c8

    def_FuncExt v1, CvsdEncInit                         0x00033640
    def_FuncExt v1, CvsdDecInit                         0x00033660
    def_FuncExt v1, CvsdEncode                          0x00033680
    def_FuncExt v1, CvsdDecode                          0x00033ba0

    def_FuncExt v1, lc3_api_decoder_init_sneak_rom      0x00035300
    def_FuncExt v1, lc3_api_encoder_init_sneak_rom      0x00035608

    def_FuncExt v1, opus_encoder_create                 0x0016d804
    def_FuncExt v1, opus_encoder_destroy                0x0016d8e8
    def_FuncExt v1, opus_encoder_ctl                    0x0016d904
    def_FuncExt v1, opus_encode                         0x0016df58

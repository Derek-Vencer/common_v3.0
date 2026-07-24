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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "cmsis_os.h"
#include "hal_trace.h"

#include "lc3_adapter.h"
#include "audio_datapath_adc.h"
#include "audio_datapath_encoder.h"
#include "audio_datapath_manager.h"

#include "app_wireless_mic.h"

#ifdef AUDIO_CODEC_ENABLE
typedef int (*AUDIO_ENCODE_PROCESS_CB_T)(void* input, void** output, uint32_t output_len);
static AUDIO_ENCODE_PROCESS_CB_T audio_encode_cb = NULL;
static void* audio_encoder_buffer = NULL;
static uint8_t audio_encode_bitrate_idx = 0;
static uint16_t audio_encode_frame_size = 0;

void audio_datapath_encoder_init(void)
{
#ifdef AUDIO_LC3_ENCODE
    audio_encode_bitrate_idx = AUDIO_LC3_CODEC_BITRATE_COUNT;
    audio_lc3_encoder_init((void**)&audio_encode_cb, AUDIO_ENCODED_CODEC_BITRATE, AUDIO_ENCODED_CODEC_FRAME_US);
#endif
}

void audio_datapath_encode_handler(uint8_t* src, uint32_t src_len, uint8_t** dest, uint32_t* dest_len)
{
    uint16_t frame_size = audio_encode_frame_size;
    if (audio_encode_cb) {
        int ret = audio_encode_cb(src, &audio_encoder_buffer, frame_size);
        if (!ret && audio_encoder_buffer) {
            *dest = (uint8_t*)audio_encoder_buffer;
            *dest_len = frame_size;
        } else {
            LOG_ERR(2, "%s encode err:%d", __func__, ret);
        }
    } else {
        ASSERT(0, "%s invalid encode cb.", __func__);
    }
}

void audio_datapath_encode_frame_size_update(uint8_t bitrate_idx)
{
#ifdef AUDIO_LC3_ENCODE
    if (bitrate_idx > 0 && bitrate_idx <= AUDIO_LC3_CODEC_BITRATE_COUNT) {
        audio_encode_frame_size = lc3_encode_map[bitrate_idx].frame_size;
        LOG_MSG(2, "encode bitrate:%d, frame size:%d", lc3_encode_map[bitrate_idx].bitrate, audio_encode_frame_size);
    } else {
        LOG_ERR(1, "invalid bitrate idx:%d", bitrate_idx);
    }
#endif
}

void audio_datapath_encode_bitrate_idx_set(uint8_t bitrate_idx)
{
#ifdef AUDIO_LC3_ENCODE
    if (bitrate_idx > 0 && bitrate_idx <= AUDIO_LC3_CODEC_BITRATE_COUNT) {
        audio_encode_bitrate_idx = bitrate_idx;
        LOG_MSG(1, "encode bitrate idx:%d", audio_encode_bitrate_idx);
    } else {
        LOG_ERR(1, "invalid bitrate idx:%d", bitrate_idx);
    }
#endif
}

uint8_t audio_datapath_encode_bitrate_idx_get(void)
{
    return audio_encode_bitrate_idx;
}
#endif
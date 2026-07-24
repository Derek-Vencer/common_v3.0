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
#ifndef _AUD_DATAPATH_ENCODER_H
#define _AUD_DATAPATH_ENCODER_H

#ifdef AUDIO_LC3_ENCODE
#include "lc3_adapter.h"
#define AUDIO_ENCODED_CODEC_BITRATE                  (AUDIO_LC3_CODEC_BITRATE)
#define AUDIO_ENCODED_CODEC_FRAME_US                 (AUDIO_LC3_CODEC_FRAME_US)
#define AUDIO_ENCODED_FRAME_SIZE                     (AUDIO_LC3_ENCODED_FRAME_SIZE)
#else
#define AUDIO_ENCODED_CODEC_BITRATE                  (128000)
#define AUDIO_ENCODED_CODEC_FRAME_US                 (5000)
#define AUDIO_ENCODED_FRAME_SIZE                     (AUDIO_ENCODED_CODEC_BITRATE * AUDIO_ENCODED_CODEC_FRAME_US / 1000000 / 8)
#endif

void audio_datapath_encoder_init(void);

void audio_datapath_encode_handler(uint8_t* src, uint32_t src_len, uint8_t** dest, uint32_t* dest_len);

void audio_datapath_encode_frame_size_update(uint8_t bitrate_idx);

void audio_datapath_encode_bitrate_idx_set(uint8_t bitrate_idx);

uint8_t audio_datapath_encode_bitrate_idx_get(void);
#endif
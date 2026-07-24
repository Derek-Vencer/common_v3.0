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
#ifndef __LC3_ADAPTER_H
#define __LC3_ADAPTER_H

#ifdef LC3_CODEC_ON

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_LC3_CODEC_BITRATE_96K                       (96000)
#define AUDIO_LC3_CODEC_BITRATE_128K                      (128000)
#define AUDIO_LC3_CODEC_BITRATE                           (AUDIO_LC3_CODEC_BITRATE_128K)
#define AUDIO_LC3_CODEC_BITRATE_COUNT                     (2)

#define AUDIO_LC3_FRAME_SIZE(B, T)                        (B * T / 1000000 / 8)

#define AUDIO_LC3_CODEC_FRAME_US                          (BLE_AUDIO_FRAME_US)
#define AUDIO_LC3_ENCODED_FRAME_SIZE                      AUDIO_LC3_FRAME_SIZE(AUDIO_LC3_CODEC_BITRATE, AUDIO_LC3_CODEC_FRAME_US)
#define AUDIO_LC3_DECODED_FRAME_SIZE                      (48 * 4 * AUDIO_LC3_CODEC_FRAME_US / 1000)

typedef struct {
    uint32_t bitrate;
    uint16_t frame_size;
} AUDIO_LC3_ENCODED_MAP_T;

extern const AUDIO_LC3_ENCODED_MAP_T lc3_encode_map[];

#ifdef AUDIO_LC3_ENCODE
int audio_lc3_encoder_init(void** encode_cb, uint32_t bitrate, uint32_t frame_us);
#endif

#ifdef AUDIO_LC3_DECODE
int audio_lc3_decoder_init(void** decode_cb, uint32_t* frame_size);
#endif

#ifdef __cplusplus
}
#endif
#endif
#endif
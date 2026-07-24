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
#ifndef __AUD_DATAPATH_MANAGER_H__
#define __AUD_DATAPATH_MANAGER_H__

#ifdef AUD_OUTPUT_BY_DAC
#include "audio_datapath_dac.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_SYNC_INTERVAL                 (300 / AUDIO_OUTPUT_DATA_COOKING_INTERVAL_MS)

#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_TRIGGER_START                 (1 << 0)
#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_TRIGGER_OK                    (1 << 1)
#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_TRIGGER_TIMEOUT               (2000)

#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_START_MUTE_MS                 (10)
#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_TRIGGER_DELAY                 (15 * 1000)
#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_CHECK_INTERVAL                (1500 * 1000)
#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_PACK_LOST_RETRG_THD           (50)
#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_OVERTIME_RETRG_THD            (6)
#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_SYNC_FACTOR_FAST_LIMIT        (5000)
#define AUDIO_MANAGER_SLAVE_CAPTURE_STREAM_SYNC_FACTOR_SLOW_LIMIT        (-5000)

#define AUDIO_OUTPUT_PATH_BIT_DAC                                        (1 << 0)
#define AUDIO_OUTPUT_PATH_BIT_USB                                        (1 << 1)
#define AUDIO_OUTPUT_PATH_BIT_I2S                                        (1 << 2)
#define AUDIO_OUTPUT_PATH_BIT_ALL                                        (AUDIO_OUTPUT_PATH_BIT_DAC | AUDIO_OUTPUT_PATH_BIT_USB | AUDIO_OUTPUT_PATH_BIT_I2S)
#define AUDIO_OUTPUT_PATH_NUM                                            (3)

#define AUDIO_INTPUT_PATH_BIT_ADC                                        (1 << 8)

#define AUDIO_OUTPUT_DATA_COOKING_START                                  (1 << 0)
#define AUDIO_OUTPUT_DATA_COOKING_INTERVAL_MS                            (AUDIO_CODEC_PLAYBACK_STREAM_DMA_INTERVAL_MS) //(2)
#define AUDIO_OUTPUT_DATA_COOKING_INTERVAL_US                            (1000 * AUDIO_OUTPUT_DATA_COOKING_INTERVAL_MS)

typedef enum {
    AUDIO_MANAGER_SESSION_STATUS_CAP_START = 0x10,
    AUDIO_MANAGER_SESSION_STATUS_CAP_STOP = 0x11,
} AUDIO_SESSION_STATUS_T;

void audio_datapath_manager_init(void);

void audio_manager_datapath_set(uint16_t path);

void audio_manager_datapath_clear(uint16_t path);

uint16_t audio_manager_datapath_get(void);

void audio_raw_data_dispatch(uint8_t* buff, uint32_t len, bool encoded, uint64_t rx_tsf);

int audio_decoded_data_dispatch(uint8_t* buff, uint32_t len, uint64_t rx_tsf);

uint8_t audio_ch_to_device_idx(uint8_t ch);

bool audio_manager_trigger_slave_algo_get(void);

bool audio_manager_trigger_slave_encode_get(void);

void audio_manager_codec_playback_stream_start_shot(void);

void audio_manager_codec_playback_stream_stop_shot(void);

void audio_manager_codec_capture_stream_start_shot(void);

void audio_manager_codec_capture_stream_stop_shot(void);

void audio_manager_i2s_playback_stream_start_shot(void);

void audio_manager_i2s_playback_stream_stop_shot(void);

void audio_manager_usb_capture_stream_start_shot(void);

void audio_manager_usb_capture_stream_stop_shot(void);

void audio_manager_data_cooking_start_shot(uint8_t path);

uint64_t audio_manager_data_cooking_ref_tsf(void);

void audio_manager_capture_start(uint8_t session_id);

void audio_manager_capture_stop(uint8_t session_id);

bool audio_manager_is_ltc_output(void);

void audio_manager_audio_session_free(void);

#ifdef __cplusplus
}
#endif
#endif

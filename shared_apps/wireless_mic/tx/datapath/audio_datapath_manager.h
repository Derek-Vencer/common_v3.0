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
#ifndef __AUD_DATAPATH_MANAGER_H__
#define __AUD_DATAPATH_MANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_OUTPUT_PATH_BIT_BLE                                         (1 << 0)
#define AUDIO_OUTPUT_PATH_BIT_SPP                                         (1 << 1)
#define AUDIO_OUTPUT_PATH_BIT_EMMC                                        (1 << 2)
#define AUDIO_OUTPUT_PATH_BIT_HFP                                         (1 << 3)
#define AUDIO_OUTPUT_PATH_BIT_ADC_DUMMY_TEST                              (1 << 4)
#define AUDIO_OUTPUT_PATH_NUM                                             (4)

void audio_datapath_manager_init(void);

void audio_manager_datapath_set(uint16_t path);

void audio_manager_datapath_clear(uint16_t path);

uint16_t audio_manager_datapath_get(void);

void audio_manager_codec_capture_stream_start_pre_handler(void);

void audio_manager_codec_capture_stream_trigger_time_set(uint64_t trigger_time);

uint64_t audio_manager_codec_capture_stream_trigger_time_get(void);

void audio_manager_codec_capture_stream_dsp_algo_start_shot(void);

void audio_manager_codec_capture_stream_dsp_algo_stop_shot(void);

void audio_manager_codec_capture_stream_ns_algo_start_shot(void);

void audio_manager_codec_capture_stream_ns_algo_stop_shot(void);

#ifdef __cplusplus
}
#endif
#endif

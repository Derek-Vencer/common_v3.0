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
#ifndef __AUD_DATAPATH_ADC_H__
#define __AUD_DATAPATH_ADC_H__

#define AUDIO_CODEC_CAPTURE_STREAM_ID                           (AUD_STREAM_ID_2)
#define AUDIO_CODEC_CAPTURE_STREAM_DEVICE                       (AUD_STREAM_USE_INT_CODEC)
#define AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_MS              (BLE_AUDIO_FRAME_US / 1000)
#define AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US              (BLE_AUDIO_FRAME_US)
#define AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM                  (AUD_CHANNEL_NUM_2)
#define AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_MAP                  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BITS                  (AUD_BITS_24)
#define AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BYTES                 (AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BITS >= AUD_BITS_24 ? 4 : 2)
#define AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_RATE                  (AUD_SAMPRATE_48000)
#define AUDIO_CODEC_CAPTURE_STREAM_MUTE_CNT                     (30)

#define AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE                (AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_RATE / 1000 * AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BYTES * \
                                                                 AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM * AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US / 1000 * 2)

#define AUDIO_CODEC_CAPTURE_STREAM_START_COST_SAFE_TIME_US      (3 * 1000)
#define AUDIO_CODEC_CAPTURE_STREAM_ALGO_COST_SAFE_TIME_US       (2 * 1000)
#define AUDIO_CODEC_CAPTURE_STREAM_FIXED_OFFSET_TIME_US         (1000)

typedef void (*AUDIO_DATAPTH_CODEC_CAPTURE_STREAM_DATAPATH_OUTPUT_CALLBACK)(uint8_t* buff, uint32_t len);

typedef struct{
    float proportiongain;
    float integralgain;
    float derivativegain;
    float error[3];
    float result;
} AUDIO_CODEC_PID_T;

#ifdef __cplusplus
extern "C" {
#endif

void audio_datapath_codec_capture_stream_data_replace_set(bool set);

void audio_datapath_codec_capture_stream_data_replace_handler(uint8_t *buff, uint32_t len);

void audio_datapath_codec_capture_stream_mute_set(bool set);

void audio_datapath_codec_capture_stream_with_encode_set(bool set);

bool audio_datapath_codec_capture_stream_with_encode_get(void);

void audio_datapath_codec_capture_stream_output_pre_handler_register(AUDIO_DATAPTH_CODEC_CAPTURE_STREAM_DATAPATH_OUTPUT_CALLBACK cb);

void audio_datapath_codec_capture_stream_output_pre_handler_degister(void);

void audio_datapath_codec_capture_stream_output_post_handler_register(AUDIO_DATAPTH_CODEC_CAPTURE_STREAM_DATAPATH_OUTPUT_CALLBACK cb);

void audio_datapath_codec_capture_stream_output_post_handler_degister(void);

bool audio_datapath_codec_capture_stream_running(void);

void audio_datapath_codec_capture_stream_sync_pid_init(void);

float audio_datapath_codec_capture_stream_sync_pid_calc(AUDIO_CODEC_PID_T *pid, int32_t diff);

void audio_datapath_codec_capture_stream_sync_tune(float ratio);

void audio_datapath_codec_capture_stream_onoff(bool on_off);

void audio_datapath_adc_init(void);

void audio_datapath_codec_capture_stream_dsp_algo_onoff_set(bool set);

bool audio_datapath_codec_capture_stream_dsp_algo_onoff_get(void);

uint8_t audio_datapath_codec_capture_stream_mute_cnt_get(void);

void audio_datapath_codec_capture_stream_ns_algo_onoff_set(bool set);

bool audio_datapath_codec_capture_stream_ns_algo_onoff_get(void);

#ifdef __cplusplus
}
#endif
#endif
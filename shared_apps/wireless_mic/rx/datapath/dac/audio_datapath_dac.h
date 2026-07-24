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
#ifndef __AUD_DATAPATH_DAC_H__
#define __AUD_DATAPATH_DAC_H__

#define AUDIO_CODEC_PLAYBACK_STREAM_TRIGGER_DELAY                (100 * 1000)
#define AUDIO_CODEC_PLAYBACK_STREAM_START_COST_SAFE_TIME_US      (10 * 1000)

#define AUDIO_CODEC_PLAYBACK_STREAM_ID                           (AUD_STREAM_ID_2)
#define AUDIO_CODEC_PLAYBACK_STREAM_DEVICE                       (AUD_STREAM_USE_INT_CODEC2)
#define AUDIO_CODEC_PLAYBACK_STREAM_DMA_INTERVAL_MS              (2)
#define AUDIO_CODEC_PLAYBACK_STREAM_DMA_INTERVAL_US              (1000 * AUDIO_CODEC_PLAYBACK_STREAM_DMA_INTERVAL_MS)
#define AUDIO_CODEC_PLAYBACK_STREAM_CHANNEL_NUM                  (AUD_CHANNEL_NUM_2)
#define AUDIO_CODEC_PLAYBACK_STREAM_CHANNEL_MAP                  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define AUDIO_CODEC_PLAYBACK_STREAM_SAMPLE_BITS                  (AUD_BITS_24)
#define AUDIO_CODEC_PLAYBACK_STREAM_SAMPLE_BYTES                 (AUDIO_CODEC_PLAYBACK_STREAM_SAMPLE_BITS >= AUD_BITS_24 ? 4 : 2)
#define AUDIO_CODEC_PLAYBACK_STREAM_SAMPLE_RATE                  (AUD_SAMPRATE_48000)

#define AUDIO_CODEC_PLAYBACK_STREAM_DMA_BUFF_SIZE                (AUDIO_CODEC_PLAYBACK_STREAM_SAMPLE_RATE * AUDIO_CODEC_PLAYBACK_STREAM_SAMPLE_BYTES * \
                                                                  AUDIO_CODEC_PLAYBACK_STREAM_CHANNEL_NUM * AUDIO_CODEC_PLAYBACK_STREAM_DMA_INTERVAL_MS / 1000 * 2)

#define AUDIO_CODEC_PLAYBACK_STREAM_INPUT_BUFF_SIZE              (48 * 4 * AUDIO_OUTPUT_DATA_COOKING_INTERVAL_MS * 2)

typedef struct{
    float proportiongain;
    float integralgain;
    float derivativegain;
    float error[3];
    float result;
} AUDIO_PLAYBACK_CODEC_PID_T;

void audio_datapath_dac_init(void);

void audio_datapath_dac_playback_stream_onoff(bool on_off);

void audio_send_decoded_data_to_dac(uint8_t session_id, uint8_t* buff, uint32_t len, uint64_t play_time);

uint32_t audio_datapath_dac_ref_tsf_get(void);

void audio_data_codec_playback_stream_mute_onoff(bool on_off);

bool audio_data_codec_playback_stream_running(void);

void audio_data_codec_playback_stream_data_input(uint8_t* data, uint32_t len);

#endif

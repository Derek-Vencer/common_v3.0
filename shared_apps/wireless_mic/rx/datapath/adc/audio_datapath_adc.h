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
#define AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_MS              (2)
#define AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US              (1000 * AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_MS)
#define AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM                  (AUD_CHANNEL_NUM_2)
#define AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_MAP                  (AUD_CHANNEL_MAP_CH0 | AUD_CHANNEL_MAP_CH1)
#define AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BITS                  (AUD_BITS_24)
#define AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BYTES                 (AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BITS >= AUD_BITS_24 ? 4 : 2)
#define AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_RATE                  (AUD_SAMPRATE_48000)

#define AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE                (AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_RATE * AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BYTES * \
                                                                 AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM * AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_MS / 1000 * 2)

bool aud_datapath_adc_capture_stream_running(void);

void audio_datapath_adc_capture_stream_onoff(bool on_off);

void audio_datapath_adc_init(void);

#endif
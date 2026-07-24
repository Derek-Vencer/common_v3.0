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
#include <string.h>
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_aud.h"
#include "audioflinger.h"

#include "app_utils.h"
#include "app_wireless_mic.h"
#include "audio_datapath_adc.h"
#include "audio_datapath_usb.h"
#include "adc_audio_queue.h"

#ifdef AUD_OUTPUT_BY_DAC
#include "audio_datapath_dac.h"
#endif

#ifdef LTC_ENABLED
#include "ltc_adapter.h"
#endif

#ifndef APP_SYSFREQ_USER_APP_MIC
#define APP_SYSFREQ_USER_APP_MIC                        (APP_SYSFREQ_USER_APP_20)
#endif

static bool audio_datapath_adc_capture_stream_started = false;

static uint8_t audio_datapath_adc_capture_stream_dma_buff[AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE];

bool aud_datapath_adc_capture_stream_running(void)
{
    return audio_datapath_adc_capture_stream_started;
}

static uint32_t audio_adc_capture_stream_handler(uint8_t *buff, uint32_t len)
{
#ifdef AUD_OUTPUT_BY_DAC
    audio_data_codec_playback_stream_data_input(buff, len);
    return 0;
#endif

    uint8_t adc1_input_buff[len / 2] = {0};
    uint8_t adc2_input_buff[len / 2] = {0};
    int32_t *buf = (int32_t *)buff;
    int32_t *adc1 = (int32_t *)adc1_input_buff;
    int32_t *adc2 = (int32_t *)adc2_input_buff;
    for (uint32_t i = 0; i < (len / 8); i++) {
        adc1[i] = buf[2 * i];
        adc2[i] = buf[2 * i + 1];
    }

    // audio_adc_data_enqueue(adc1_input_buff, len / 2);

#ifdef LTC_ENABLED
    uint8_t ltc_input_mode = ltc_input_mode_get();
    if (ltc_input_mode == LTC_INPUT_MODE_RIN) {
        ltc_frame_enqueue(adc2_input_buff, len / 2);

        ltc_decoder_process_shot();
    } else if (ltc_input_mode == LTC_INPUT_MODE_FACTORY) {
        if (audio_data_usb_capture_stream_running()) {
            audio_data_usb_capture_stream_data_input(buff, len);
        }
    }
#endif

    return 0;
}

void audio_datapath_adc_capture_stream_onoff(bool on_off)
{
    if (audio_datapath_adc_capture_stream_started == on_off) {
        return;
    }

    LOG_MSG(1, "audio_codec_capture_stream %s", on_off == true ? "on" : "off");

    if (on_off) {
        app_sysfreq_req(APP_SYSFREQ_USER_APP_MIC, APP_SYSFREQ_26M);

        struct AF_STREAM_CONFIG_T stream_cfg;

        memset(&stream_cfg, 0, sizeof(struct AF_STREAM_CONFIG_T));
        stream_cfg.bits = AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_BITS;
        stream_cfg.sample_rate = AUDIO_CODEC_CAPTURE_STREAM_SAMPLE_RATE;
        stream_cfg.channel_num = AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_NUM;
        stream_cfg.channel_map = (enum AUD_CHANNEL_MAP_T)AUDIO_CODEC_CAPTURE_STREAM_CHANNEL_MAP;
        stream_cfg.data_ptr = audio_datapath_adc_capture_stream_dma_buff;
        stream_cfg.data_size = AUDIO_CODEC_CAPTURE_STREAM_DMA_BUFF_SIZE;
        stream_cfg.handler = audio_adc_capture_stream_handler;
        stream_cfg.device = AUDIO_CODEC_CAPTURE_STREAM_DEVICE;
        stream_cfg.io_path = AUD_INPUT_PATH_MAINMIC;

        af_set_priority(AF_USER_DATAPATH_ADC, osPriorityRealtime);
        af_stream_open(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE, &stream_cfg);
        af_stream_start(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
    } else {
        af_stream_stop(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
        af_stream_close(AUDIO_CODEC_CAPTURE_STREAM_ID, AUD_STREAM_CAPTURE);
        af_set_priority(AF_USER_DATAPATH_ADC, osPriorityAboveNormal);

        app_sysfreq_req(APP_SYSFREQ_USER_APP_MIC, APP_SYSFREQ_32K);
    }

    audio_datapath_adc_capture_stream_started = on_off;
}

void audio_datapath_adc_init(void)
{
    audio_adc_data_queue_init();
}
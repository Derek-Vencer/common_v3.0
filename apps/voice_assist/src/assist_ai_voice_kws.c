/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#include "hal_trace.h"
#include "plat_types.h"
#include "audioflinger.h"
#include "speech_ssat.h"
#include "app_anc_assist.h"
#include "app_voice_assist_anc.h"

static uint8_t *g_stream_buf_ptr    = NULL;
static AF_STREAM_HANDLER_T g_stream_handler = NULL;
static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"kws";

// int16_t _get_max_pcm_val(int16_t *pcm_buf, uint32_t pcm_len)
// {
//     int16_t max = 0;
//     for (uint32_t i=0; i<pcm_len; i++) {
//         if (max < pcm_buf[i]) {
//             max = pcm_buf[i];
//         }
//     }

//     return max;
// }

static int32_t voice_assist_kws_process(voice_assist_process_frame_data_t *process_frame_data)
{
    // VOICE_ASSIST_TRACE(0, "[%s] len = %d", __func__, process_frame_data->frame_len);

    if ((g_stream_handler == NULL) || (g_stream_buf_ptr == NULL)) {
        return 1;
    }

    float   *input_pcm_buf = (float *)process_frame_data->talk_mic[0];
    int16_t *out_pcm_buf = (int16_t *)g_stream_buf_ptr;

    for (uint32_t i=0; i < process_frame_data->frame_len; i++) {
        out_pcm_buf[i] = speech_ssat_int16((int32_t)(input_pcm_buf[i] / (256 / 1)));   // 1: 0dB
    }
    g_stream_handler((uint8_t *)out_pcm_buf, process_frame_data->frame_len * sizeof(int16_t));

    // VOICE_ASSIST_TRACE(0, "[%s] Max: %d", __func__, (uint32_t)_get_max_pcm_val(out_pcm_buf, process_frame_data->frame_len));

    return 0;
}

static int32_t voice_assist_kws_get_fs(void)
{
    return 16000;
}

static const voice_assist_algo_callback_t kws_assist_algo_dsp = {
    .cap_process    = voice_assist_kws_process,
    .get_fs         = voice_assist_kws_get_fs,
};

static int32_t voice_assist_kws_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_TALK_MIC_CH_L;

    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->extern_kws_en = true;

    return mic_map;
}

static const voice_assist_stream_callback_t voice_assist_kws_stream = {
    .get_mic_ch_map       = voice_assist_kws_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_kws_stream_cfg = {
    .voice_assist_callback = &voice_assist_kws_stream,
    .assist_algo_dsp = &kws_assist_algo_dsp,
};

int32_t assist_ai_voice_kws_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_kws_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_kws_stream_cfg);

    return 0;
}

int32_t assist_ai_voice_kws_open(struct AF_STREAM_CONFIG_T *stream_cfg)
{
    ASSERT(stream_cfg != NULL, "[%s] stream_cfg = NULL", __func__);
    ASSERT(stream_cfg->bits == AUD_BITS_16, "[%s] bits(%d) is invalid", __func__, stream_cfg->bits);
    ASSERT(stream_cfg->channel_num == AUD_CHANNEL_NUM_1, "[%s] ch(%d) is invalid", __func__, stream_cfg->channel_num);

    VOICE_ASSIST_TRACE(0, "[%s] KWS open stream: fs: %d, ch: %d, bits: %d, size: %d", __func__,
        stream_cfg->sample_rate,
        stream_cfg->channel_num,
        stream_cfg->bits,
        stream_cfg->data_size);

    g_stream_buf_ptr = stream_cfg->data_ptr;
    g_stream_handler = stream_cfg->handler;

    app_anc_assist_open(_index);

    return 0;
}

int32_t assist_ai_voice_kws_close(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] KWS close stream", __func__);

    app_anc_assist_close(_index);
    g_stream_buf_ptr = NULL;
    g_stream_handler = NULL;

    return 0;
}

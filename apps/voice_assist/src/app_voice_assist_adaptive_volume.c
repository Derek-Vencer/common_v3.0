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
#include "app_anc_assist.h"
#include "app_voice_assist_adaptive_volume.h"
#include "adaptive_volume.h"
#include "anc_assist.h"
#include "app_voice_assist_anc.h"
#include "hal_trace.h"
#include "ae_math.h"
#include "af_stream_sw_gain.h"


#ifdef AUDIO_ADAPTIVE_VOLUME

#define HEAP_BUFF_SIZE (3 * 1024)
#include "ext_heap.h"
static uint8_t __attribute__((aligned(4))) ada_volume_heap_buff[HEAP_BUFF_SIZE];

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"ada_volume";

static bool audio_adaptive_volume_is_running = 0;
extern const AdaptiveVolumeConfig audio_adaptive_volume_cfg;
static AdaptiveVolumeState *adaptive_volume_st = NULL;

static int32_t _voice_assist_adaptive_volume_callback(void *buf, uint32_t len, void *other);

static int32_t voice_assist_ada_volume_process(voice_assist_process_frame_data_t *process_frame_data)
{
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    float res[2];
    res[0] = anc_assist_res->ada_volume_noise_energy;
    res[1] = anc_assist_res->ref_detect_energy;
    // VOICE_ASSIST_TRACE(0, "[%s] ff_mic_noise_engery_db(x10): %d", __func__, (int)res[0] * 10);
    // VOICE_ASSIST_TRACE(0, "[%s] ref_mic_noise_engery_db(x10): %d", __func__, (int)res[1] * 10);
    if (anc_assist_res->ada_volume_noise_status) {
        _voice_assist_adaptive_volume_callback(res, 3, NULL);
    }

    return 0;
}

static int32_t voice_assist_ada_volume_get_fs(void)
{
    return 16000;
}

static int32_t voice_assist_ada_volume_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FF_MIC_CH_L | ANC_REF_MIC_CH_L;
#if defined(FREEMAN_ENABLED_STERO)
    mic_map |= ANC_FF_MIC_CH_R;
#endif
    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->adaptive_volume_en = true;

    return mic_map;
}

static const voice_assist_algo_callback_t ada_volume_assist_algo_dsp = {
    .cap_process    = voice_assist_ada_volume_process,
    .get_fs         = voice_assist_ada_volume_get_fs,
};

static const voice_assist_stream_callback_t voice_assist_ada_volume_stream = {
    .get_mic_ch_map       = voice_assist_ada_volume_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_ada_volume_stream_cfg = {
    .voice_assist_callback = &voice_assist_ada_volume_stream,
    .assist_algo_dsp = &ada_volume_assist_algo_dsp,
};

int32_t app_voice_assist_adaptive_volume_init(void)
{
    // app_anc_assist_register(_index, _voice_assist_adaptive_volume_callback);
    _index = app_voice_assist_stream_register(user_name, &voice_assist_ada_volume_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_ada_volume_stream_cfg);

    return 0;
}

int32_t app_voice_assist_adaptive_volume_open(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] adaptive volume start stream", __func__);

    app_voice_assist_open_anc();

    ext_heap_init(ada_volume_heap_buff);
    adaptive_volume_st = adaptive_volume_custom_allocator(&audio_adaptive_volume_cfg, &ext_allocator);

    app_anc_assist_open(_index);
    audio_adaptive_volume_is_running = 1;

    af_stream_sw_gain_set_max_gain_coef(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, 1.0);

    return 0;
}

int32_t app_voice_assist_adaptive_volume_close(void)
{
    if(audio_adaptive_volume_is_running == 1){
        VOICE_ASSIST_TRACE(0, "[%s] adaptive volume close stream", __func__);
        af_stream_sw_gain_set_extra_gain(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, AF_STREAM_SW_GAIN_ADAPTIVE_VOLUME_USER, 1.0);
        audio_adaptive_volume_is_running = 0;
        app_anc_assist_close(_index);
        app_voice_assist_close_anc();
    }

    adaptive_volume_destroy(adaptive_volume_st);

    return 0;
}

static float adaptive_gain = 0.f;
static int32_t _voice_assist_adaptive_volume_callback(void *buf, uint32_t len, void *other)
{
    if(audio_adaptive_volume_is_running != 1){
        return 0;
    }

    // ff mic noise engery
    float *res = (float *)buf;
    float ff_mic_noise_engery_db = res[0];
    // VOICE_ASSIST_TRACE(0, "[%s] ff_mic_noise_engery_db(x10): %d", __func__, (int)(ff_mic_noise_engery_db * 10));

    // ref
    POSSIBLY_UNUSED float ref_engery_db = res[1];
    POSSIBLY_UNUSED float *ref = (float *)other;
    POSSIBLY_UNUSED uint32_t ref_frame_len = len;

    ref_engery_db = ref_engery_db - adaptive_gain;
    // VOICE_ASSIST_TRACE(0, "[%s] ref_engery_db(x10): %d", __func__, (int)(ref_engery_db * 10));

    if(adaptive_volume_st != NULL){ 
        float adaptive_volume_gain_db = adaptive_volume_gain_estimate(adaptive_volume_st, ff_mic_noise_engery_db, ref_engery_db);
        float lin_gain =  DB2LIN(adaptive_volume_gain_db);
        VOICE_ASSIST_TRACE(0, "[%s] gain*10= %d", __func__, (int)(adaptive_volume_gain_db*10));
        af_stream_sw_gain_set_extra_gain(AUD_STREAM_ID_0, AUD_STREAM_PLAYBACK, AF_STREAM_SW_GAIN_ADAPTIVE_VOLUME_USER, lin_gain);
        adaptive_gain = adaptive_volume_gain_db;
    }else{
        VOICE_ASSIST_TRACE(0, "%s warning, adaptive_volume_st is not created",__func__);
    }

    return 0;
}

#endif
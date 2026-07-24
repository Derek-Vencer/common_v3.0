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
#ifdef VOICE_ASSIST_NOISE
#include "hal_trace.h"
#include "app_anc_assist.h"
#include "anc_assist.h"
#include "anc_process.h"
#include "app_voice_assist_noise_adapt_anc.h"
#include "app_voice_assist_anc.h"
#include "app_anc_utils.h"

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"noise_adapt_anc";
static int32_t ambient_cnt = 0;

static int32_t _voice_assist_noise_adapt_anc_callback(void * buf, uint32_t len, void *other)
{
    int32_t *res = (int32_t *)buf;
    noise_status_t noise_status = res[1];

    if (res[2] >= 150) {
        ambient_cnt++;
        if (ambient_cnt >= 10) {
            ambient_cnt = 0;
            VOICE_ASSIST_TRACE(0, "The ambient noise is greater than 50dB!");
        }
    } else {
        ambient_cnt = 0;
    }
    
    // need to do tws sync
    // app_anc_set_gain_f32 0.5 == total gain -6dB, app_anc_set_gain_f32 0.25 == total gain -12dB
    if (res[0]) {
        if (noise_status == NOISE_STATUS_STRONG_ANC) {
        VOICE_ASSIST_TRACE(4, "4444444444444444444444444444");
        } else if (noise_status == NOISE_STATUS_MIDDLE_ANC) {
            VOICE_ASSIST_TRACE(3, "3333333333333333333333333333");
        } else if (noise_status == NOISE_STATUS_LOWER_ANC) {
            VOICE_ASSIST_TRACE(2, "2222222222222222222222222222");
        } else if (noise_status == NOISE_STATUS_QUIET_ANC)  {
            VOICE_ASSIST_TRACE(1, "1111111111111111111111111111");
        } else {
            VOICE_ASSIST_TRACE(1, "0000000000000000000000000000");
        }
    }
    
    return 0;
}

static int32_t voice_assist_noise_adapt_anc_process(voice_assist_process_frame_data_t *process_frame_data)
{
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    for (uint32_t i = 0; i < MAX_FF_CHANNEL_NUM; i++) {
        uint32_t res[3];
        res[0] = anc_assist_res->noise_adapt_changed[i];
        res[1] = anc_assist_res->noise_status[i];
        res[2] = (int32_t)(anc_assist_res->noise_energy[i] * 100);
        if (res[0]) {
            _voice_assist_noise_adapt_anc_callback(res, 3, NULL);
        }
    }

    return 0;
}

static int32_t voice_assist_noise_adapt_anc_get_fs(void)
{
    return 16000;
}

static const voice_assist_algo_callback_t noise_adapt_anc_assist_algo_dsp = {
    .cap_process    = voice_assist_noise_adapt_anc_process,
    .get_fs         = voice_assist_noise_adapt_anc_get_fs,
};

static int32_t voice_assist_noise_adapt_anc_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FF_MIC_CH_L;
#if defined(FREEMAN_ENABLED_STERO)
    mic_map |= ANC_FF_MIC_CH_R;
#endif
    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->noise_en = true;

    return mic_map;
}

static const voice_assist_stream_callback_t voice_assist_noise_adapt_anc_stream = {
    .get_mic_ch_map       = voice_assist_noise_adapt_anc_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_noise_adapt_anc_stream_cfg = {
    .voice_assist_callback = &voice_assist_noise_adapt_anc_stream,
    .assist_algo_dsp = &noise_adapt_anc_assist_algo_dsp,
};

int32_t app_voice_assist_noise_adapt_anc_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_noise_adapt_anc_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_noise_adapt_anc_stream_cfg);

    return 0;
}

int32_t app_voice_assist_noise_adapt_anc_open(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] noise adapt anc start stream", __func__);
    app_voice_assist_open_anc();
    app_anc_assist_open(_index);
    app_anc_set_gain_f32(ANC_GAIN_USER_APP_ANC, ANC_FEEDBACK, 1, 1);
    app_anc_set_gain_f32(ANC_GAIN_USER_ANC_ASSIST_SYNC, ANC_FEEDBACK, 1, 1);

    return 0;
}

int32_t app_voice_assist_noise_adapt_anc_close(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] noise adapt anc close stream", __func__);
    app_anc_assist_close(_index);
    app_voice_assist_close_anc();
    app_anc_set_gain_f32(ANC_GAIN_USER_ANC_ASSIST_SYNC, ANC_FEEDBACK, 1, 1);

    return 0;
}
#endif

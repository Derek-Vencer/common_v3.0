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
#include "anc_assist.h"
#include "app_anc_assist.h"
#include "app_voice_assist_wd.h"
#include "cmsis.h"
#include "app_voice_assist_ultrasound.h"
#include "app_voice_assist_anc.h"

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"wd";
bool infrasound_fadeout_flag;

static uint32_t out_ear_ticks = 0;
static uint32_t in_ear_ticks = 0;

#define MAX_WD_TICKS (133 * 3)

static wd_status_t last_wd_status[2] = {WD_STATUS_OUT_EAR, WD_STATUS_OUT_EAR};

static int32_t _voice_assist_wd_callback(void *buf, uint32_t len, void *other)
{

     uint32_t *res = (uint32_t *)buf;

    anc_assist_algo_status_t wd_changed = res[0];
    wd_status_t wd_status = res[1];

    if (wd_changed == ANC_ASSIST_ALGO_STATUS_CHANGED) {
        if (wd_status == WD_STATUS_IN_EAR) {
            VOICE_ASSIST_TRACE(0, "[%s] Change to WD_STATUS_IN_EAR", __func__);
        }else if(wd_status == WD_STATUS_OUT_EAR) {
            VOICE_ASSIST_TRACE(0, "[%s] Change to WD_STATUS_OUT_EAR", __func__);
        }  else {
            VOICE_ASSIST_TRACE(0, "[%s] Change to WD_STATUS_IDEL", __func__);
        }
        app_voice_assist_wd_close();
    }
    return 0;
}

static int32_t voice_assist_wd_close(void)
{
    app_voice_assist_pilot_set_play_fadeout();
    return 0;
}

static int32_t voice_assist_wd_process(voice_assist_process_frame_data_t *process_frame_data)
{
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    uint32_t res[2];
    res[0] = anc_assist_res->wd_changed[0];
    res[1] = anc_assist_res->wd_status[0];
    _voice_assist_wd_callback(res, 2, NULL);

    return 0;
}

static int32_t voice_assist_wd_get_fs(void)
{
    return 16000;
}

static const voice_assist_algo_callback_t wd_assist_algo_dsp = {
    .cap_process    = voice_assist_wd_process,
    .close          = voice_assist_wd_close,
    .get_fs         = voice_assist_wd_get_fs,
};

static int32_t voice_assist_wd_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FB_MIC_CH_L | ANC_REF_MIC_CH_L;
#if defined(FREEMAN_ENABLED_STERO)
    mic_map |= ANC_FB_MIC_CH_R;
#endif

    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->pilot_en = true;
    cfg->pilot_cfg.adaptive_anc_en = 0;
    cfg->pilot_cfg.wd_en = 1;
    cfg->pilot_cfg.custom_leak_detect_en = 0;

    return mic_map;
}

#ifndef ANC_SPK_CH
#define ANC_SPK_CH              (AUD_CHANNEL_MAP_CH0)
#endif

int32_t voice_assist_wd_get_spk(void)
{
    uint32_t wd_spk_ch = 0;

    wd_spk_ch |= ANC_SPK_CH;;

    return wd_spk_ch;
}

static const voice_assist_stream_callback_t voice_assist_wd_stream = {
    .get_mic_ch_map       = voice_assist_wd_get_mic_map,
    .get_spk_ch_map       = voice_assist_wd_get_spk,
};

static voice_assist_user_register_cfg voice_assist_wd_stream_cfg = {
    .voice_assist_callback = &voice_assist_wd_stream,
    .assist_algo_dsp = &wd_assist_algo_dsp,
};

int32_t app_voice_assist_wd_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_wd_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_wd_stream_cfg);

    return 0;
}

int32_t app_voice_assist_wd_open(void)
{
    app_anc_assist_open(_index);

    out_ear_ticks = 0;
    in_ear_ticks = 0;

    for (uint32_t i = 0; i < ARRAY_SIZE(last_wd_status); i++) {
        last_wd_status[i] = WD_STATUS_OUT_EAR;
    }

    return 0;
}

int32_t app_voice_assist_wd_close(void)
{
    app_anc_assist_close(_index);

    return 0;
}

/***************************************************************************
 *
 * Copyright 2015-2023 BES.
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
#ifdef VOICE_ASSIST_NOISE_CLASSIFY
#include "hal_trace.h"
#include "app_anc_assist.h"
#include "anc_assist.h"
#include "app_voice_assist_noise_classify_adapt_anc.h"
#include "app_voice_assist_anc.h"

static int32_t POSSIBLY_UNUSED noise_classify_status[MAX_FF_CHANNEL_NUM];
static int32_t g_status = 2;
static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"noise_classify_adapt_anc";

static int32_t _voice_assist_noise_classify_adapt_anc_callback(void * buf, uint32_t len, void *other)
{
    int32_t *res = (int32_t *)buf;
    int32_t tmp_status;
#if defined(FREEMAN_ENABLED_STERO)
    for (uint8_t i = 0, j = 0; i < len; i += 2, j++) {
        noise_classify_status[j] = res[i + 1];
        // VOICE_ASSIST_TRACE(0, "mic %d: noise status = %d", j, noise_classify_status[j]);
    }
    tmp_status = MAX(noise_classify_status[0], noise_classify_status[1]);
#else
    tmp_status = res[1];
#endif
    if (g_status != tmp_status) {
        g_status = tmp_status;
        if (g_status == NOISE_STATUS_PLANE_ANC) {
            VOICE_ASSIST_TRACE(4, "NOISE_STATUS_PLANE_ANC");
        } else if (g_status == NOISE_STATUS_TRANSPORT_ANC) {
            VOICE_ASSIST_TRACE(3, "NOISE_STATUS_TRANSPORT_ANC");
        } else if (g_status == NOISE_STATUS_OUTDOOR_ANC) {
            VOICE_ASSIST_TRACE(2, "NOISE_STATUS_OUTDOOR_ANC");
        } else if (g_status == NOISE_STATUS_INDOOR_ANC) {
            VOICE_ASSIST_TRACE(1, "NOISE_STATUS_INDOOR_ANC");
        }
    }
    return 0;
}

static int32_t _voice_assist_noise_classify_adapt_anc_process(voice_assist_process_frame_data_t *process_frame_data)
{
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    int res[4] = {0};
    for (uint8_t i = 0, j = 0; i < 2 * MAX_FB_CHANNEL_NUM; i += 2, j++) {
        res[i + 0] = anc_assist_res->noise_classify_adapt_changed[j];
        res[i + 1] = anc_assist_res->noise_classify_status[j];
    }
    if (res[0] || res[2]) {
        _voice_assist_noise_classify_adapt_anc_callback(res, 4, NULL);
    }

    return 0;
}

static int32_t voice_assist_noise_classify_adapt_anc_get_fs(void)
{
    return 16000;
}

static const voice_assist_algo_callback_t noise_classify_adapt_anc_assist_algo_dsp = {
    .cap_process    = _voice_assist_noise_classify_adapt_anc_process,
    .get_fs         = voice_assist_noise_classify_adapt_anc_get_fs,
};

static int32_t voice_assist_noise_classify_adapt_anc_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FF_MIC_CH_L;
    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->ff_howling_en = true;
    cfg->fb_howling_en = true;
    cfg->wind_en = true;

    return mic_map;
}

static const voice_assist_stream_callback_t voice_assist_noise_classify_adapt_anc_stream = {
    .get_mic_ch_map       = voice_assist_noise_classify_adapt_anc_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_noise_classify_adapt_anc_stream_cfg = {
    .voice_assist_callback = &voice_assist_noise_classify_adapt_anc_stream,
    .assist_algo_dsp = &noise_classify_adapt_anc_assist_algo_dsp,
};

int32_t app_voice_assist_noise_classify_adapt_anc_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_noise_classify_adapt_anc_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_noise_classify_adapt_anc_stream_cfg);

    return 0;
}

int32_t app_voice_assist_noise_classify_adapt_anc_open(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] noise adapt anc start stream", __func__);
    app_voice_assist_open_anc();
    app_anc_assist_open(_index);

    return 0;
}

int32_t app_voice_assist_noise_classify_adapt_anc_close(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] noise adapt anc close stream", __func__);
    app_anc_assist_close(_index);
    app_voice_assist_close_anc();

    return 0;
}
#endif

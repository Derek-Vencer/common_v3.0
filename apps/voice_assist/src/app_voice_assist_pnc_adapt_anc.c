
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
#include "app_anc_assist.h"
#include "anc_assist.h"
#include "app_voice_assist_pnc_adapt_anc.h"
#include "app_voice_assist_anc.h"

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"pnc_adapt_anc";

static int32_t voice_assist_pnc_adapt_anc_process(voice_assist_process_frame_data_t *process_frame_data)
{
    return 0;
}

static int32_t voice_assist_pnc_adapt_anc_get_fs(void)
{
    return 16000;
}

static const voice_assist_algo_callback_t pnc_adapt_anc_assist_algo_dsp = {
    .cap_process    = voice_assist_pnc_adapt_anc_process,
    .get_fs         = voice_assist_pnc_adapt_anc_get_fs,
};

static int32_t voice_assist_pnc_adapt_anc_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FF_MIC_CH_L | ANC_FB_MIC_CH_L|ANC_TALK_MIC_CH_L|ANC_REF_MIC_CH_L;

    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->pnc_en = true;

    return mic_map;
}

static const voice_assist_stream_callback_t voice_assist_pnc_adapt_anc_stream = {
    .get_mic_ch_map       = voice_assist_pnc_adapt_anc_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_pnc_adapt_anc_stream_cfg = {
    .voice_assist_callback = &voice_assist_pnc_adapt_anc_stream,
    .assist_algo_dsp = &pnc_adapt_anc_assist_algo_dsp,
};

int32_t app_voice_assist_pnc_adapt_anc_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_pnc_adapt_anc_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_pnc_adapt_anc_stream_cfg);

    return 0;
}

int32_t app_voice_assist_pnc_adapt_anc_open(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] pnc adapt anc start stream", __func__);
    app_anc_assist_open(_index);
    return 0;
}

int32_t app_voice_assist_pnc_adapt_anc_close(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] pnc adapt anc close stream", __func__);
    app_anc_assist_close(_index);
    return 0;
}

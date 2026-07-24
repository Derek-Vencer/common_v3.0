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
#ifdef VOICE_ASSIST_ADAPTIVE_MODE
#include "hal_trace.h"
#include "anc_assist.h"
#include "app_anc_assist.h"
#include "app_anc.h"
#include "ae_math.h"
#include "app_anc_utils.h"
#include "app_voice_assist_anc.h"
#include "app_voice_assist_adaptive_mode.h"

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"adaptive_mode";
static int32_t _voice_assist_adaptive_mode_callback(voice_assist_process_frame_data_t *process_frame_data);
static bool adaptive_gain_enable = false;

static void adaptive_mode_set_tt_ff_gain(float tt_gain_lin, float ff_gain_lin)
{
    VOICE_ASSIST_TRACE(0, "[%s] tt_gain(x100): %d, ff_gain(x100): %d", __func__, (int)(tt_gain_lin * 100), (int)(ff_gain_lin * 100));

    // TODO: tws sync gain
    app_anc_set_global_gain_f32(ANC_FEEDFORWARD, ff_gain_lin, ff_gain_lin);
    app_anc_set_global_gain_f32(ANC_TALKTHRU, tt_gain_lin, tt_gain_lin);
}

int32_t app_voice_assist_adaptive_mode_open(void)
{
    app_anc_assist_open(_index);
    app_voice_assist_open_anc();

    adaptive_gain_enable = true;

    return 0;
}

int32_t app_voice_assist_adaptive_mode_close(void)
{
    adaptive_gain_enable = false;

    app_anc_assist_close(_index);
    app_voice_assist_close_anc();

    adaptive_mode_set_tt_ff_gain(1.0, 1.0); // reset gain
    return 0;
}

int32_t app_voice_assist_adaptive_mode_set_gain_db(float gain_db)
{
    if(gain_db > 0){
        VOICE_ASSIST_TRACE(0, "[%s] gain_db(x10): (%d) should <= 0", __func__, (int)(gain_db * 10));
        return -1;
    }

    VOICE_ASSIST_TRACE(0, "[%s] fixed gain_db (x10): %d", __func__, (int)(gain_db * 10));
    adaptive_gain_enable = false;

    float gain_lin = DB2LIN(gain_db);

    adaptive_mode_set_tt_ff_gain(gain_lin, 1.0 - gain_lin);

    return 0;
}

static int32_t _voice_assist_adaptive_mode_callback(voice_assist_process_frame_data_t *process_frame_data)
{
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    assist_adaptive_mode_res_t *res = &(anc_assist_res->adaptive_mode_res);

    if(ASSIST_ADAPTIVE_MODE_STATUS_CHANGED == res->status && adaptive_gain_enable){
        float gain_lin = DB2LIN(res->gain_db);
        adaptive_mode_set_tt_ff_gain(gain_lin, 1.0 - gain_lin);
    }

    return 0;
}

static int32_t voice_assist_adaptive_mode_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FF_MIC_CH_L;

   // VOICE_ASSIST_TRACE(0, "[%s] ANC_ASSIST_USER_ADAPTIVE_MODE", __func__);
    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->ai_vad_en = true;
    cfg->adaptive_mode_en = true;

    return mic_map;
}

static voice_assist_algo_callback_t voice_assist_adaptive_mode_algo = {
    .cap_process = _voice_assist_adaptive_mode_callback,
};

static voice_assist_stream_callback_t voice_assist_adaptive_mode_stream = {
    .get_mic_ch_map = voice_assist_adaptive_mode_get_mic_map,
};

voice_assist_user_register_cfg voice_assist_adaptive_mode_cfg = {
    .voice_assist_callback = &voice_assist_adaptive_mode_stream,
    .assist_algo_dsp = &voice_assist_adaptive_mode_algo,
};


int32_t app_voice_assist_adaptive_mode_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_adaptive_mode_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name, &voice_assist_adaptive_mode_cfg);

    return 0;
}
#endif

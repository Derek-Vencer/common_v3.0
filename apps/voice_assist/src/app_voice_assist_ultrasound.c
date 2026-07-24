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
#ifdef VOICE_ASSIST_WD_ENABLED
#include "hal_trace.h"
#include "anc_assist.h"
#include "app_anc_assist.h"
#include "app_voice_assist_ultrasound.h"
#include "app_voice_assist_wd.h"
#include "cmsis_os.h"
#include "assist_ultrasound.h"
#include "app_voice_assist_anc.h"

typedef struct
{
    uint32_t tick;
} ultrasound_state_t;

#define ANC_ASSIST_ULTRASOUND_PEROID_MS (500)
static ultrasound_state_t ultrasound;
static osTimerId app_anc_assist_id = NULL;
static bool ultrasound_reset_needed = true;

extern bool g_opened_flag;
extern AncAssistConfig anc_assist_cfg;

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"ultrasound";

static void app_anc_assist_handler(void const *param);
osTimerDef(app_anc_assist, app_anc_assist_handler);
static void app_anc_assist_handler(void const *param)
{
	VOICE_ASSIST_TRACE(0, "[%s]", __FUNCTION__);

#if 1
	if (g_opened_flag == false) {
		app_voice_assist_ultrasound_open();
	}
#endif
}

void app_anc_assist_checker_start(void)
{
    app_anc_assist_id = osTimerCreate(osTimer(app_anc_assist), osTimerPeriodic, NULL);
    if (app_anc_assist_id != NULL) {
        osTimerStart(app_anc_assist_id, ANC_ASSIST_ULTRASOUND_PEROID_MS);
    }
}

static int32_t voice_assist_ultrasound_process(voice_assist_process_frame_data_t *process_frame_data)
{
    if (ultrasound_reset_needed) {
        assist_ultrasound_reset();
        ultrasound_reset_needed = false;
    }

    // // VOICE_ASSIST_TRACE(0, "[%s] len = %d", __func__, len);

    // uint32_t *res = (uint32_t *)buf;

    // if (res[0] == true) {
    //     VOICE_ASSIST_TRACE(0, "[%s] ultrasound status changed", __func__);
    //     app_voice_assist_wd_open();
    // }

    // ultrasound.tick += 1;

    // if (anc_assist_cfg.pilot_cfg.debug_en) {
    //     VOICE_ASSIST_TRACE(2, "[%s] ultrasound.tick = %d", __func__, ultrasound.tick);
    // }

    // if (ultrasound.tick == anc_assist_cfg.pilot_cfg.ultrasound_stop_tick) {
    //     app_voice_assist_ultrasound_close();
    // }
    return 0;
}

static int32_t voice_assist_ultrasound_get_fs(void)
{
    return 32000;
}

static const voice_assist_algo_callback_t ultrasound_assist_algo_dsp = {
    .cap_process    = voice_assist_ultrasound_process,
    .get_fs         = voice_assist_ultrasound_get_fs,
};

static int32_t voice_assist_ultrasound_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FB_MIC_CH_L | ANC_REF_MIC_CH_L;
#if defined(FREEMAN_ENABLED_STERO)
    mic_map |= ANC_FB_MIC_CH_R;
#endif

    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->ultrasound_en = true;

    return mic_map;
}

#ifndef ANC_SPK_CH
#define ANC_SPK_CH     (AUD_CHANNEL_MAP_CH0)
#endif

static int32_t voice_assist_ultrasound_get_speak_map()
{
    uint32_t speak_map = 0;

    speak_map |= ANC_SPK_CH;

    return speak_map;
}

static const voice_assist_stream_callback_t voice_assist_ultrasound_stream = {
    .get_mic_ch_map       = voice_assist_ultrasound_get_mic_map,
    .get_spk_ch_map       = voice_assist_ultrasound_get_speak_map,
};

static voice_assist_user_register_cfg voice_assist_ultrasound_eqstream_cfg = {
    .voice_assist_callback = &voice_assist_ultrasound_stream,
    .assist_algo_dsp = &ultrasound_assist_algo_dsp,
};

int32_t app_voice_assist_ultrasound_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_ultrasound_eqstream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_ultrasound_eqstream_cfg);

    app_anc_assist_checker_start();

    return 0;
}

int32_t app_voice_assist_ultrasound_open(void)
{
    app_anc_assist_open(_index);
    ultrasound_reset_needed = true;

    ultrasound.tick = 0;

    return 0;
}

int32_t app_voice_assist_ultrasound_close(void)
{
    app_anc_assist_close(_index);

    return 0;
}
#endif

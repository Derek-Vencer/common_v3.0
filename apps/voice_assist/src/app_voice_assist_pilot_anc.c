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
#ifdef VOICE_ASSIST_PILOT_ANC_ENABLED
#include "hal_trace.h"
#include "anc_assist.h"
#include "app_anc.h"
#include "app_anc_assist.h"
#include "app_voice_assist_pilot_anc.h"
#include "cmsis_os.h"
#include "app_voice_assist_anc.h"

#define ANC_ASSIST_TIMING_PEROID_MS (1000 * 20)

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"pilot_anc";

static void pilot_anc_handler(void const *param);
osTimerDef(pilot_anc, pilot_anc_handler);
static osTimerId pilot_anc_id = NULL;
static void pilot_anc_handler(void const *param)
{
	VOICE_ASSIST_TRACE(0, "[%s]", __FUNCTION__);

	if (app_anc_work_status()) {
		app_voice_assist_pilot_anc_open();
	}
}

static void app_voice_assist_pilot_anc_checker_start(void)
{
    pilot_anc_id = osTimerCreate(osTimer(pilot_anc), osTimerPeriodic, NULL);
    if (pilot_anc_id != NULL) {
        osTimerStart(pilot_anc_id, ANC_ASSIST_TIMING_PEROID_MS);
    }
}

static int32_t last_stop_flag = 0;
extern int32_t get_pilot_stop_flag(void);

static int32_t voice_assist_pilot_anc_process(voice_assist_process_frame_data_t *process_frame_data)
{
    // VOICE_ASSIST_TRACE(0, "[%s] len = %d", __func__, len);
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(assist_res != NULL, "[%s] assist_res is NULL", __func__);

    // close pilot anc when pilot stops
    if (last_stop_flag == 0 && get_pilot_stop_flag()) {
        VOICE_ASSIST_TRACE(0, "[%s] pilot finished, %d/%d", __FUNCTION__, last_stop_flag, get_pilot_stop_flag());
        app_voice_assist_pilot_anc_close();
    }

    if (assist_res->curve_changed[0] == 1 && assist_res->curve_id[0] == ANC_ASSIST_ALGO_ID_PILOT) {
        VOICE_ASSIST_TRACE(0,"!!!!!!!!!!!!!!!!!!!!!!!! current state is %d",assist_res->curve_index[0]);
    }

    last_stop_flag = get_pilot_stop_flag();

    return 0;
}

static int32_t voice_assist_pilot_anc_get_fs(void)
{
    return 16000;
}

static const voice_assist_algo_callback_t _pilot_anc_assist_algo_dsp = {
    .cap_process    = voice_assist_pilot_anc_process,
    .get_fs         = voice_assist_pilot_anc_get_fs,
};

static int32_t voice_assist_pilot_anc_get_mic_map()
{
    uint32_t mic_map = 0;

    return mic_map;
}

static const voice_assist_stream_callback_t voice_assist_pilot_anc_stream = {
    .get_mic_ch_map       = voice_assist_pilot_anc_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_pilot_anc_stream_cfg = {
    .voice_assist_callback = &voice_assist_pilot_anc_stream,
    .assist_algo_dsp = &_pilot_anc_assist_algo_dsp,
};

int32_t app_voice_assist_pilot_anc_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_pilot_anc_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_pilot_anc_stream_cfg);
    app_voice_assist_pilot_anc_checker_start();

    return 0;
}

int32_t app_voice_assist_pilot_anc_open(void)
{
    app_anc_assist_open(_index);

    last_stop_flag = 0;

    return 0;
}

int32_t app_voice_assist_pilot_anc_close(void)
{
    app_anc_assist_close(_index);

    return 0;
}
#endif

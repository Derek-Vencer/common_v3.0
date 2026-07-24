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
#ifdef VOICE_ASSIST_VPU_WSD
#include "hal_trace.h"
#include "anc_assist.h"
#include "app_voice_assist_anc.h"
#include "app_anc_assist.h"
#include "app_voice_assist_vpu_wsd.h"
#include "app_anc.h"
#include "cmsis_os.h"
#include "cmsis.h"
#include "tgt_hardware.h"

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"vpu_wsd";
static int32_t _voice_assist_vpu_wsd_callback(voice_assist_process_frame_data_t *process_frame_data);

static bool last_vpu_wsd_flag = 0;
// static app_anc_mode_t last_anc_mode = APP_ANC_MODE_QTY;

static void voice_assist_vpu_wsd_timer_cb(void const *n);
osTimerDef(VOICE_ASSIST_VPU_WSD_TIMER, voice_assist_vpu_wsd_timer_cb);
osTimerId voice_assist_vpu_wsd_timer_id = NULL;

static void voice_assist_vpu_wsd_timer_cb(void const *n)
{
    // app_anc_switch(last_anc_mode);
    VOICE_ASSIST_TRACE(1, "[%s]", __FUNCTION__);
}

int32_t app_voice_assist_vpu_wsd_open(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] vpu wsd open stream", __func__);

    app_anc_assist_open(_index);

    app_voice_assist_open_anc();

    // app_anc_switch(APP_ANC_MODE1);

    last_vpu_wsd_flag = 0;

    return 0;
}

int32_t app_voice_assist_vpu_wsd_close(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] vpu wsd close stream", __func__);

    osTimerStop(voice_assist_vpu_wsd_timer_id);

    app_anc_assist_close(_index);

    app_voice_assist_close_anc();

    // app_anc_switch(APP_ANC_MODE_OFF);
    return 0;
}

static int32_t _voice_assist_vpu_wsd_callback(voice_assist_process_frame_data_t *process_frame_data)
{
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    bool vpu_wsd_flag_changed = anc_assist_res->vpu_wsd_flag_changed;
    bool vpu_wsd_flag = anc_assist_res->vpu_wsd_flag;

    /*
     * when vpu_wsd 0 -> 1, switch to TT
     * when vpu_wsd 1 -> 0, start timer, when timer arrive, switch to ANC. if wsd change to 1 before timer, just stop timer
     */
    if (vpu_wsd_flag_changed) {
        last_vpu_wsd_flag = vpu_wsd_flag;
        VOICE_ASSIST_TRACE(0, "[%s] Change to %d", __func__, vpu_wsd_flag);
        // last_anc_mode = app_anc_get_curr_mode();

        if (vpu_wsd_flag == true) {
            // if (app_anc_get_curr_mode() != APP_ANC_MODE6) {
            //     app_anc_switch(APP_ANC_MODE6);
            // }
            osTimerStop(voice_assist_vpu_wsd_timer_id);
        } else {
            osTimerStart(voice_assist_vpu_wsd_timer_id, 5*1000);
        }
    }

    return 0;
}

static int32_t voice_assist_vpu_wsd_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FF_MIC_CH_L | ANC_REF_MIC_CH_L;

   // VOICE_ASSIST_TRACE(0, "[%s] ANC_ASSIST_USER_VPU_WSD", __func__);
    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->vpu_wsd_en = true;
    cfg->vpu_aec_en = true;

    return mic_map;
}

static voice_assist_algo_callback_t voice_assist_vpu_wsd_algo = {
    .cap_process = _voice_assist_vpu_wsd_callback,
};

static voice_assist_stream_callback_t voice_assist_vpu_wsd_stream = {
    .get_mic_ch_map = voice_assist_vpu_wsd_get_mic_map,
};

voice_assist_user_register_cfg voice_assist_vpu_wsd_cfg = {
    .voice_assist_callback = &voice_assist_vpu_wsd_stream,
    .assist_algo_dsp = &voice_assist_vpu_wsd_algo,
};


int32_t app_voice_assist_vpu_wsd_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_vpu_wsd_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name, &voice_assist_vpu_wsd_cfg);

    if (voice_assist_vpu_wsd_timer_id == NULL){
        voice_assist_vpu_wsd_timer_id = osTimerCreate(osTimer(VOICE_ASSIST_VPU_WSD_TIMER), osTimerOnce, NULL);
    }
    return 0;
}
#endif

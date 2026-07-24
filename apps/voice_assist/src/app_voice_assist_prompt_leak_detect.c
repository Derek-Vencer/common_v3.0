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
#include "app_voice_assist_prompt_leak_detect.h"
#include "arm_math.h"
#include "speech_memory.h"
// #include "anc_process.h"
#include "app_anc.h"
#include "app_voice_assist_anc.h"

extern int32_t assist_prompt_set_working_status(uint32_t status);
extern int32_t assist_prompt_get_mode_index(int *mode_index);

static int is_running= 0;
static int cfg_set_flag = 0;
static int output_mode_num = 0;

POSSIBLY_UNUSED static int32_t mode_l = -1;
POSSIBLY_UNUSED static int32_t mode_r = -1;

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"prompt_leak_detect";

static int32_t _voice_assist_prompt_leak_detect_callback(void *buf, uint32_t len, void *other)
{
    int32_t *res = (int32_t *)buf;
#if defined(FREEMAN_ENABLED_STERO)
    mode_l = res[0];
    mode_r = res[1];
    VOICE_ASSIST_TRACE(0, "[%s] select mode_l = %d , mode_r = %d", __FUNCTION__, mode_l, mode_r);
#else
    mode_l = res[0];
    VOICE_ASSIST_TRACE(0, "[%s] select mode = %d ", __FUNCTION__, mode_l);
#endif

    return 0;
}

static int32_t voice_assist_prompt_leak_detect_process(voice_assist_process_frame_data_t *process_frame_data)
{
    uint32_t res[1 * MAX_FB_CHANNEL_NUM];
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    for (uint32_t i = 0; i < MAX_FB_CHANNEL_NUM; i++) {
        res[0 + i] = anc_assist_res->prompt_leak_detect_result[i];
    }

    if (anc_assist_res->prompt_leak_detect_status[0] == PROMPT_LEAK_DETECT_STATUS_RESULT) {
        _voice_assist_prompt_leak_detect_callback(res, MAX_FB_CHANNEL_NUM, NULL);
    }

    return 0;
}

static int32_t voice_assist_prompt_leak_detect_get_fs(void)
{
    return 16000;
}

static const voice_assist_algo_callback_t prompt_leak_detect_assist_algo_dsp = {
    .cap_process    = voice_assist_prompt_leak_detect_process,
    .get_fs         = voice_assist_prompt_leak_detect_get_fs,
};

static int32_t voice_assist_prompt_leak_detect_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FB_MIC_CH_L | ANC_FF_MIC_CH_L | ANC_REF_MIC_CH_L;
#if defined(FREEMAN_ENABLED_STERO)
    mic_map |= ANC_FB_MIC_CH_R | ANC_FF_MIC_CH_R;
#endif

    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->prompt_adaptive_en = true;

    return mic_map;
}

static const voice_assist_stream_callback_t voice_assist_prompt_leak_detect_stream = {
    .get_mic_ch_map       = voice_assist_prompt_leak_detect_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_prompt_leak_detect_eqstream_cfg = {
    .voice_assist_callback = &voice_assist_prompt_leak_detect_stream,
    .assist_algo_dsp = &prompt_leak_detect_assist_algo_dsp,
};

int32_t app_voice_assist_prompt_leak_detect_init(void)
{
    is_running = 0;
    _index = app_voice_assist_stream_register(user_name, &voice_assist_prompt_leak_detect_eqstream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_prompt_leak_detect_eqstream_cfg);

    return 0;
}

int32_t app_voice_assist_prompt_leak_detect_set_working_status(int32_t status)
{
    assist_prompt_set_working_status(status);
    return 0;
}

int32_t app_voice_assist_prompt_leak_detect_open(void)
{
    is_running = 1;
    output_mode_num = 0;
    VOICE_ASSIST_TRACE(0,"[%s] @@@@@@@ open",__func__);
    VOICE_ASSIST_TRACE(0,"@@@@@@@ before %d",output_mode_num);
    cfg_set_flag = 0;

    app_anc_assist_open(_index);
    return 0;
}

int32_t app_voice_assist_prompt_leak_detect_close(void)
{
    if(is_running == 1) {
        is_running = 0;
        VOICE_ASSIST_TRACE(0,"[%s] @@@@@@@ close",__func__);
        float band1,band2,band3;
        app_voice_assist_get_prompt_anc_index(&output_mode_num,&band1,&band2,&band3,0);
        VOICE_ASSIST_TRACE(0,"[%s] @@@@@@@ mode_index %d band value = %d %d %d",__func__,output_mode_num,(int)(band1*100),(int)(band2*100),(int)(band3*100));
        // app_anc_switch(1);
        app_anc_assist_close(_index);
    }
    return 0;
}

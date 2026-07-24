
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
#ifdef VOICE_ASSIST_FF_FIR_LMS
#include "hal_trace.h"
#include "app_anc_assist.h"
#include "anc_assist.h"
#include "app_voice_assist_fir_anc_open_leak.h"
#include "app_voice_assist_fir_lms.h"
#include "app_voice_assist_anc.h"

static uint32_t _index;
static const uint8_t* const user_name = (const uint8_t*)"fir_anc_open_leak";

static int32_t _voice_assist_fir_anc_open_leak_callback(void * buf, uint32_t len, void *other)
{
    uint32_t *res = (uint32_t *)buf;
    uint32_t wear_leak_status = *res;
    VOICE_ASSIST_TRACE(0, "[%s] wear_leak_status = %d", __func__, wear_leak_status);
    if (wear_leak_status == 1) {
        // app_voice_assist_fir_lms_open();
    } else {
        VOICE_ASSIST_TRACE(1, "1111111111111111111111111111");
    }
    return 0;
}

static int32_t voice_assist_fir_anc_open_leak_peocess(voice_assist_process_frame_data_t *process_frame_data)
{
    uint32_t res[1];
    static const uint8_t* const ptr_name = (const uint8_t*)"anc";
    AncAssistRes *anc_assist_res = ((voice_anc_msg_t *)voice_assist_get_user_ptr(ptr_name))->res;
    ASSERT(anc_assist_res != NULL, "[%s] anc_assist_res is NULL", __func__);

    res[0] = anc_assist_res->fir_anc_wear_leak_changed;
    if (res[0]) {
         _voice_assist_fir_anc_open_leak_callback(res, 1, NULL);
    }

    return 0;
}

static int32_t voice_assist_fir_anc_open_leak_get_fs(void)
{
    return 16000;
}

static const voice_assist_algo_callback_t fir_anc_open_leak_assist_algo_dsp = {
    .cap_process    = voice_assist_fir_anc_open_leak_peocess,
    .get_fs         = voice_assist_fir_anc_open_leak_get_fs,
};

static int32_t voice_assist_fir_anc_open_leak_get_mic_map()
{
    uint32_t mic_map = 0;

    mic_map |= ANC_FF_MIC_CH_L;
    AncAssistConfig *cfg = app_voice_assist_anc_get_cfg();
    cfg->fir_anc_open_leak_en = true;

    return mic_map;
}

static const voice_assist_stream_callback_t voice_assist_fir_anc_open_leak_stream = {
    .get_mic_ch_map       = voice_assist_fir_anc_open_leak_get_mic_map,
};

static voice_assist_user_register_cfg voice_assist_fir_anc_open_leak_stream_cfg = {
    .voice_assist_callback = &voice_assist_fir_anc_open_leak_stream,
    .assist_algo_dsp = &fir_anc_open_leak_assist_algo_dsp,
};

int32_t app_voice_assist_fir_anc_open_leak_init(void)
{
    _index = app_voice_assist_stream_register(user_name, &voice_assist_fir_anc_open_leak_stream_cfg, NULL);
    app_voice_assist_dsp_register(&_index, user_name , &voice_assist_fir_anc_open_leak_stream_cfg);

    return 0;
}

int32_t app_voice_assist_fir_anc_open_leak_open(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] fir anc open leak start stream", __func__);
    app_anc_assist_open(_index);
    return 0;
}
int32_t app_voice_assist_fir_anc_open_leak_close(void)
{
    VOICE_ASSIST_TRACE(0, "[%s] fir anc open leak close stream", __func__);
    app_anc_assist_close(_index);
    return 0;
}
#endif

/***************************************************************************
 *
 * Copyright 2015-2025 BES.
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
#ifdef SPEECH_TX_1MIC_NS_48KF
#include "cmsis.h"
#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "plat_types.h"
#include "hal_location.h"
#include "heap_api.h"
#include "cp_accel.h"
#include "norflash_api.h"
#include "speech_1mic_ns_48kf.h"
#include "audio_datapath_adc.h"
#include "app_wireless_mic.h"
#include "app_utils.h"

static const Speech1MicNS48kfConfig tx_1mic_ns = {
    .bypass =           0,
    .nn_gain_db =       5,
    .wdrc_enable =      0,
    .pf_denoise_db =    -10,
};
static Speech1MicNS48kfState_ *speech_tx_1mic_ns_st = NULL;

#ifdef AUDIO_ALGO_CP_ACCEL

enum CP_ALGO_STATE_T {
    CP_ALGO_STATE_NONE = 0,
    CP_ALGO_STATE_IDLE,
    CP_ALGO_STATE_WORKING,
};

#define AUDIO_ALGO_CP_PROCESS_FRAME_LEN             (48 * 4 * AUDIO_CODEC_CAPTURE_STREAM_DMA_INTERVAL_US / 1000)

static bool audio_algo_cp_accel_inited = false;
static uint8_t audio_algo_cp_accel_state = CP_ALGO_STATE_NONE;

static uint8_t audio_audio_cp_process_input_buff[AUDIO_ALGO_CP_PROCESS_FRAME_LEN] = {0};
static uint8_t audio_audio_cp_process_output_buff[AUDIO_ALGO_CP_PROCESS_FRAME_LEN] = {0};

static unsigned int cp_process_main(uint8_t event);

static struct cp_task_desc TASK_DESC_CP_ALGO_48K_NS = {
    CP_ACCEL_STATE_CLOSED, cp_process_main, NULL, NULL, NULL};
/****************************function defination****************************/

static void audio_algo_cp_accel_init(void)
{
    uint8_t wait_times = 20;
    LOG_MSG(1, "enter %s", __func__);

    if (audio_algo_cp_accel_inited == false) {
        cp_accel_open(CP_TASK_VOICE_ALGO, &TASK_DESC_CP_ALGO_48K_NS);

        memset(audio_audio_cp_process_input_buff, 0x0, AUDIO_ALGO_CP_PROCESS_FRAME_LEN);
        memset(audio_audio_cp_process_output_buff, 0x0, AUDIO_ALGO_CP_PROCESS_FRAME_LEN);

        while ((cp_accel_init_done() == false) && (--wait_times > 0)) {
            hal_sys_timer_delay_us(200);
        }

        if (cp_accel_init_done() == false) {
            LOG_ERR(1, "%s wait cp timeout!", __func__);
            return;
        }

        audio_algo_cp_accel_state = CP_ALGO_STATE_IDLE;
    }

    audio_algo_cp_accel_inited = true;
}

static void audio_algo_cp_accel_deinit(void)
{
    LOG_MSG(1, "enter %s", __func__);

    if (audio_algo_cp_accel_inited) {

        cp_accel_close(CP_TASK_VOICE_ALGO);

        if (speech_tx_1mic_ns_st) {
            speech_1mic_ns_48kf_destroy(speech_tx_1mic_ns_st);
            speech_tx_1mic_ns_st = NULL;
        }

        audio_algo_cp_accel_state = CP_ALGO_STATE_NONE;
    }

    audio_algo_cp_accel_inited = false;
}

static bool audio_algo_cp_accel_is_running(void)
{
    return !!(cp_accel_init_done() && audio_algo_cp_accel_inited);
}

static void audio_algo_cp_accel_process(float* buff, uint16_t len)
{
    uint16_t wait_cnt = 0;

    if (AUDIO_ALGO_CP_PROCESS_FRAME_LEN != len) {
        LOG_ERR(2, "%s invalid frame len:%d", __func__, len);
        return;
    }

    if (audio_algo_cp_accel_is_running() == false) {
        return;
    }

    while (audio_algo_cp_accel_state == CP_ALGO_STATE_WORKING) {
        hal_sys_timer_delay_us(10);

        if (wait_cnt++ > 500) {
            LOG_ERR(0, "cp is busy!");
            return;
        }
    }

    if (audio_algo_cp_accel_state == CP_ALGO_STATE_IDLE) {
        memcpy(audio_audio_cp_process_input_buff, buff, len);

        memcpy(buff, audio_audio_cp_process_output_buff, len);

        audio_algo_cp_accel_state = CP_ALGO_STATE_WORKING;

        cp_accel_send_event_mcu2cp(CP_BUILD_ID(CP_TASK_VOICE_ALGO, CP_EVENT_VOICE_ALGO_PROCESSING));
    } else {
        LOG_ERR(1, "invalid cp accel state:%d", audio_algo_cp_accel_state);
    }
}

CP_TEXT_SRAM_LOC
static unsigned int cp_process_main(uint8_t event)
{
    if (event != CP_EVENT_VOICE_ALGO_PROCESSING) {
        TRACE(1, "invalid cp pro event:%d", event);
        return 0;
    }

    if (audio_algo_cp_accel_state == CP_ALGO_STATE_WORKING) {
        speech_1mic_ns_48kf_process(speech_tx_1mic_ns_st, (float*)audio_audio_cp_process_input_buff, AUDIO_ALGO_CP_PROCESS_FRAME_LEN);

        memcpy(audio_audio_cp_process_output_buff, audio_audio_cp_process_input_buff, AUDIO_ALGO_CP_PROCESS_FRAME_LEN);

        audio_algo_cp_accel_state = CP_ALGO_STATE_IDLE;
    } else {
        TRACE(2, "%s invalid cp accel state:%d", __func__, audio_algo_cp_accel_state);
    }

    return 0;
}
#endif

void audio_algo_ns_init(void)
{
    app_sysfreq_req(APP_SYSFREQ_USER_APP_NS, APP_SYSFREQ_208M);

#ifdef AUDIO_ALGO_CP_ACCEL
    audio_algo_cp_accel_init();
#endif

    if (speech_tx_1mic_ns_st == NULL) {
        speech_tx_1mic_ns_st = speech_1mic_ns_48kf_create((Speech1MicNS48kfConfig *)&tx_1mic_ns, default_allocator());
        ASSERT(speech_tx_1mic_ns_st, "%s speech_1mic_ns_48kf_create failed.", __func__);
    }
}

void audio_algo_ns_deinit(void)
{
#ifdef AUDIO_ALGO_CP_ACCEL
    audio_algo_cp_accel_deinit();
#endif

    if (speech_tx_1mic_ns_st) {
        speech_1mic_ns_48kf_destroy(speech_tx_1mic_ns_st);
        speech_tx_1mic_ns_st = NULL;
    }

    app_sysfreq_req(APP_SYSFREQ_USER_APP_NS, APP_SYSFREQ_32K);
}

void audio_algo_ns_process(float* buff, uint16_t len)
{
    if (speech_tx_1mic_ns_st == NULL) {
        return;
    }

    // uint32_t s_time, e_time;
    // s_time = hal_fast_sys_timer_get();

#ifdef AUDIO_ALGO_CP_ACCEL
    audio_algo_cp_accel_process(buff, len);
#else
    speech_1mic_ns_48kf_process(speech_tx_1mic_ns_st, buff, len / 4);
#endif

    // e_time = hal_fast_sys_timer_get();
    // LOG_DBG(3, "[speech_1mic_ns_48kf_process] cost %d us at freq %d", FAST_TICKS_TO_US(e_time - s_time), hal_sys_timer_calc_cpu_freq(5, 0));
}

#endif